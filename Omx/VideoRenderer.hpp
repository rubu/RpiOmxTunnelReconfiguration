#pragma once

#include "Component.hpp"
#include "../Vcos/HeapMemory.hpp"

#include <vector>
#include <stack>

namespace Omx
{
	class CVideoRenderer : public CComponent
	{
	private:
		CComponentPort m_InputPort;

	public:
		CVideoRenderer() : CComponent("OMX.broadcom.video_render"), m_InputPort(*this, 90)
		{
			AddPort(m_InputPort);
			m_InputPort.Disable();
			ChangeState(OMX_StateIdle);
			ChangeState(OMX_StateExecuting);
		}
		~CVideoRenderer()
		{
			try
			{
				m_InputPort.Disable();
			}
			catch (std::exception& Exception)
			{
				std::cerr << "Error: " << Exception.what() << std::endl;
			}
		}
		CComponentPort& GetInputPort()
		{
			return m_InputPort;
		}
	};
}