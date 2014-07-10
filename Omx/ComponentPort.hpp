#pragma once

#include "Omx.hpp"
#include "../Vcos/Vcos.hpp"

namespace Omx
{
	class CComponent;
	class CTunnel;

	class CComponentPort
	{
	friend class CComponent;
	friend class CTunnel;
	private:
		unsigned int m_nId;
		OMX_HANDLETYPE m_hComponent;
		VCOS_EVENT_T m_EnabledEvent;
		VCOS_EVENT_T m_DisabledEvent;
		VCOS_EVENT_T m_FlushedEvent;

	public:
		CComponentPort(CComponent& Component, unsigned int nId);
		
		void Disable(bool bWait = true);
		void Enable(bool bWait = true);
		bool Enabled();
		void Flush(bool bWait = true);
		void WaitToEnable();
		void WaitToDisable();
		void WaitToFlush();

	private:
		void OnDisabled();
		void OnEnabled();
		void OnFlushed();
	};
}