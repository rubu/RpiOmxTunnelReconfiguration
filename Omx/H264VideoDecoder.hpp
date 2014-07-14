#pragma once

#include "Component.hpp"
#include "Tunnel.hpp"
#include "../Vcos/HeapMemory.hpp"

#include <vector>
#include <stack>

namespace Omx
{
	class CH264VideoDecoder : public CComponent
	{
	private:
		CComponentPort m_InputPort;
		CComponentPort m_OutputPort;
		std::vector<OMX_BUFFERHEADERTYPE*> m_BufferHeaders;
		std::stack<OMX_BUFFERHEADERTYPE*> m_FreeBufferHeaders;
		VCOS_EVENT_T m_BufferFreedEvent;
		bool m_bWaitingForFreeBuffer;

	public:
		CH264VideoDecoder(size_t nInputBufferSize = 0, size_t nBufferCount = 0, CGraph* pGraph = nullptr) : CComponent("OMX.broadcom.video_decode", pGraph), m_InputPort(*this, 130), m_OutputPort(*this, 131), m_bWaitingForFreeBuffer(false)
		{
			std::string sBufferFreedEventEventName(m_sName);
			sBufferFreedEventEventName.append(":buffer_freed_event");
			CHECK_VCOS(vcos_event_create(&m_BufferFreedEvent, sBufferFreedEventEventName.c_str()), "failed to create vcos event");
			AddPort(m_InputPort);
			AddPort(m_OutputPort);
			m_InputPort.Disable();
			m_OutputPort.Disable();
			ChangeState(OMX_StateIdle);
			{
				// Set H.264/AVC
				OMX_VIDEO_PARAM_PORTFORMATTYPE VideoFormat;
				OMX_INIT_STRUCTURE(VideoFormat);
				VideoFormat.nPortIndex = 130;
				CHECK_OMX(OMX_GetParameter(m_hComponent, OMX_IndexParamVideoPortFormat, &VideoFormat), "failed to get video format");
				VideoFormat.eCompressionFormat = OMX_VIDEO_CodingAVC;
				CHECK_OMX(OMX_SetParameter(m_hComponent, OMX_IndexParamVideoPortFormat, &VideoFormat), "failed to set video format");
			}
			{
				// Set Annex B byte stream format
				OMX_NALSTREAMFORMATTYPE StreamFormat;
				OMX_INIT_STRUCTURE(StreamFormat);
				StreamFormat.nPortIndex = 130;
				CHECK_OMX(OMX_GetParameter(m_hComponent, static_cast<OMX_INDEXTYPE>(OMX_IndexParamNalStreamFormatSupported), &StreamFormat), "failed to query supported stream format parameters");
				CHECK_EXPRESSION(StreamFormat.eNaluFormat & OMX_NaluFormatOneNaluPerBuffer != 0, "one NAL unit per buffer is not supported by  OMX");
				CHECK_OMX(OMX_GetParameter(m_hComponent, static_cast<OMX_INDEXTYPE>(OMX_IndexParamNalStreamFormatSelect), &StreamFormat), "failed to query stream format parameters");
				StreamFormat.eNaluFormat = OMX_NaluFormatStartCodes;
				CHECK_OMX(OMX_SetParameter(m_hComponent, static_cast<OMX_INDEXTYPE>(OMX_IndexParamNalStreamFormatSelect), &StreamFormat), "failed to set stream format parameters");
			}
			size_t nBufferAlignment = 0;
			{
				// Adjust buffers to the size of network mtu
				OMX_PARAM_PORTDEFINITIONTYPE PortDefinition;
				OMX_INIT_STRUCTURE(PortDefinition);
				PortDefinition.nPortIndex = 130;
				CHECK_OMX(OMX_GetParameter(m_hComponent, OMX_IndexParamPortDefinition, &PortDefinition), "failed to get video format");
				// If size and count is zero fallback to the decoder defaults
				if (nBufferCount != 0 && nInputBufferSize != 0)
				{
					PortDefinition.nBufferSize = nInputBufferSize;
					PortDefinition.nBufferCountActual = nBufferCount;
					nBufferAlignment = PortDefinition.nBufferAlignment;
					CHECK_OMX(OMX_SetParameter(m_hComponent, OMX_IndexParamPortDefinition, &PortDefinition), "failed to et video format");
				}
				nBufferCount = PortDefinition.nBufferCountActual;
				nInputBufferSize = PortDefinition.nBufferSize;
			}
			m_InputPort.Enable(false);
			for (size_t nBufferIndex = 0; nBufferIndex < nBufferCount; ++nBufferIndex)
			{
				std::stringstream BufferName;
				BufferName << "H264 video decoder buffer " << nBufferIndex;
				OMX_BUFFERHEADERTYPE* pBufferHeader;
				Vcos::CHeapMemory* pHeapMemory = new Vcos::CHeapMemory(nInputBufferSize, nBufferAlignment, BufferName.str().c_str());
				CHECK_OMX(OMX_UseBuffer(m_hComponent, &pBufferHeader, 130, pHeapMemory, nInputBufferSize, *pHeapMemory), "failed to supply video decoder input buffer");
				m_BufferHeaders.push_back(pBufferHeader);
				m_FreeBufferHeaders.push(pBufferHeader);
			}
			m_InputPort.WaitToEnable();
			ChangeState(OMX_StateExecuting);
		}
		~CH264VideoDecoder()
		{
			try
			{
				for (size_t nBufferIndex = 0; nBufferIndex < m_BufferHeaders.size(); ++nBufferIndex)
				{
					Vcos::CHeapMemory* pHeapMemory = reinterpret_cast<Vcos::CHeapMemory*>(m_BufferHeaders[nBufferIndex]->pAppPrivate);
					delete pHeapMemory;
					OMX_ERRORTYPE nError = OMX_FreeBuffer(m_hComponent, 130, m_BufferHeaders[nBufferIndex]);
					if (nError != OMX_ErrorNone)
					{
						std::cerr << "Error: failed to free H264 decoder buffer" << std::endl;
					}
				}
				m_OutputPort.Disable();
				m_InputPort.Disable();
			}
			catch (std::exception& Exception)
			{
				std::cerr << "Error: " << Exception.what() << std::endl;
			}
		}
		OMX_ERRORTYPE EmptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)
		{
			m_FreeBufferHeaders.push(pBuffer);
			if (m_bWaitingForFreeBuffer)
			{
				vcos_event_signal(&m_BufferFreedEvent);
			}
		}
		OMX_BUFFERHEADERTYPE* GetInputBuffer()
		{
			if (m_FreeBufferHeaders.empty())
			{
				m_bWaitingForFreeBuffer = true;
				vcos_event_wait(&m_BufferFreedEvent);
				m_bWaitingForFreeBuffer = false;
			}
			OMX_BUFFERHEADERTYPE* pBuffer = m_FreeBufferHeaders.top();
			m_FreeBufferHeaders.pop();
			return pBuffer;
		}
		void ProcessBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
		{
			CHECK_OMX(OMX_EmptyThisBuffer(m_hComponent, pBuffer), "failed to process buffer");
		}
		CComponentPort& GetInputPort()
		{
			return m_InputPort;
		}
		CComponentPort& GetOutputPort()
		{
			return m_OutputPort;
		}
	};
}