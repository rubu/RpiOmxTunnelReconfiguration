#include "ComponentPort.hpp"
#include "Component.hpp"

namespace Omx
{
	CComponentPort::CComponentPort(CComponent& Component, unsigned int nId) : m_hComponent(Component.m_hComponent), m_nId(nId)
	{
		std::stringstream EnabledEventName, DisabledEventName, FlushedEventName;
		EnabledEventName << Component.m_sName << ":" << m_nId << ":enabled_event";
		DisabledEventName << Component.m_sName << ":" << m_nId << ":disabled_event";
		FlushedEventName << Component.m_sName << ":" << m_nId << ":flushed_event";
		CHECK_VCOS(vcos_event_create(&m_EnabledEvent, EnabledEventName.str().c_str()), "failed to create vcos event");
		CHECK_VCOS(vcos_event_create(&m_DisabledEvent, DisabledEventName.str().c_str()), "failed to create vcos event");
		CHECK_VCOS(vcos_event_create(&m_FlushedEvent, FlushedEventName.str().c_str()), "failed to create vcos event");
	}
	void CComponentPort::Disable(bool bWait)
	{
		if (!Enabled())
		{
			return;
		}
		CHECK_OMX(OMX_SendCommand(m_hComponent, OMX_CommandPortDisable, m_nId, NULL), "failed to disable port");
		if (bWait)
		{
			WaitToDisable();
		}
	}
	void CComponentPort::Enable(bool bWait)
	{
		if (Enabled())
		{
			return;
		}
		CHECK_OMX(OMX_SendCommand(m_hComponent, OMX_CommandPortEnable, m_nId, NULL), "failed to enable port");
		if (bWait)
		{
			WaitToEnable();
		}
	}
	bool CComponentPort::Enabled()
	{
		OMX_PARAM_PORTDEFINITIONTYPE PortDefinition;
		OMX_INIT_STRUCTURE(PortDefinition);
		PortDefinition.nPortIndex = m_nId;
		CHECK_OMX(OMX_GetParameter(m_hComponent, OMX_IndexParamPortDefinition, &PortDefinition), "failed to get port status");
		return PortDefinition.bEnabled;
	}
	void CComponentPort::Flush(bool bWait)
	{
		CHECK_OMX(OMX_SendCommand(m_hComponent, OMX_CommandFlush, m_nId, NULL), "failed to enable port");
		if (bWait)
		{
			WaitToFlush();
		}
	}
	void CComponentPort::WaitToDisable()
	{
		CHECK_VCOS(vcos_event_wait(&m_DisabledEvent), "failed to wait for status change");
	}
	void CComponentPort::WaitToEnable()
	{
		CHECK_VCOS(vcos_event_wait(&m_EnabledEvent), "failed to wait for status change");
	}
	void CComponentPort::WaitToFlush()
	{
		CHECK_VCOS(vcos_event_wait(&m_FlushedEvent), "failed to wait for status change");
	}
	void CComponentPort::OnDisabled()
	{
		vcos_event_signal(&m_DisabledEvent);
	}
	void CComponentPort::OnEnabled()
	{
		vcos_event_signal(&m_EnabledEvent);
	}
	void CComponentPort::OnFlushed()
	{
		vcos_event_signal(&m_FlushedEvent);
	}
}