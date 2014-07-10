#pragma once

#include "Omx.hpp"
#include "ComponentPort.hpp"
#include "Graph.hpp"
#include "../Vcos/Vcos.hpp"

#include <string>
#include <map>

namespace Omx
{
	class CComponent
	{
	friend class CComponentPort;
	
	typedef std::map<unsigned int, CComponentPort*> CPortMap;

	protected:
		std::string m_sName;
		OMX_HANDLETYPE m_hComponent;
		VCOS_EVENT_T m_StateChangedEvent;
		CPortMap m_Ports;
		CGraph* m_pGraph;

	public:
		CComponent(std::string sName, CGraph* pGraph = NULL);
		virtual ~CComponent();

		void AddPort(CComponentPort& Port);
		void ChangeState(OMX_STATETYPE State);
		OMX_ERRORTYPE EventHandler(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
		virtual OMX_ERRORTYPE EmptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer);
		virtual OMX_ERRORTYPE FillBufferDone(OMX_BUFFERHEADERTYPE* pBuffer);

		static OMX_ERRORTYPE EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
		static OMX_ERRORTYPE EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);
		static OMX_ERRORTYPE FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);
	};
}