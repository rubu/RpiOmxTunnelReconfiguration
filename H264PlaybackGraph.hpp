#pragma once

#include "Omx/Graph.hpp"
#include "Omx/H264VideoDecoder.hpp"
#include "Omx/VideoRenderer.hpp"

class CH264PlaybackGraph : public Omx::CGraph
{
private:
	Omx::CH264VideoDecoder m_VideoDecoder;
	Omx::CVideoRenderer m_VideoRenderer;
	bool m_bTunnelActive;

public:
	CH264PlaybackGraph() : m_VideoDecoder(0, 0, this), m_bTunnelActive(false)
	{
#ifdef DEBUG
		std::cout << "Debug: Omx graph created" << std::endl;
#endif
	}
	~CH264PlaybackGraph()
	{
		try
		{
#ifdef DEBUG
			std::cout << "Debug: destroying Omx graph" << std::endl;
#endif
			DisableTunnel();
		}
		catch (std::exception& Exception)
		{
			std::cerr << "Error: " << Exception.what() << std::endl;
		}
	}
	OMX_BUFFERHEADERTYPE* GetInputBuffer()
	{
		return m_VideoDecoder.GetInputBuffer();
	}
	void OnPortSettingsChanged(Omx::CComponent* pComponent, unsigned int nPortId)
	{
		if (pComponent == &m_VideoDecoder && nPortId == 131)
		{
#ifdef DEBUG
			std::cout << "Debug: port settings changed for the video decoder output port" << std::endl;
#endif
			if (m_bTunnelActive == true)
			{
				DisableTunnel();
			}
#ifdef DEBUG
			std::cout << "Debug: setting up tunel and enabling video decoder output port and renderer input port" << std::endl;
#endif
			Omx::CTunnel(&m_VideoDecoder.GetOutputPort(), &m_VideoRenderer.GetInputPort());
			m_VideoDecoder.GetOutputPort().Enable();
			m_VideoRenderer.GetInputPort().Enable();
			m_bTunnelActive = true;
		}
	}
	void ProcessBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
	{
		m_VideoDecoder.ProcessBuffer(pBuffer);
	}

private:
	void DisableTunnel()
	{
		if (m_bTunnelActive == true)
		{
#ifdef DEBUG
			std::cout << "Debug: tunnel between decoder and render is active, flushing and disabling ports and tunnel" << std::endl;
#endif
			m_VideoRenderer.GetInputPort().Disable();
			m_VideoDecoder.GetOutputPort().Disable();
			Omx::CTunnel(&m_VideoDecoder.GetOutputPort(), NULL);
			Omx::CTunnel(&m_VideoDecoder.GetOutputPort(), NULL);
			m_bTunnelActive = false;
		}
	}
};