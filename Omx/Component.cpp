#include "Component.hpp"
#include "ComponentPort.hpp"

#include <utility>

namespace Omx
{
	CComponent::CComponent(std::string sName, CGraph* pGraph) : m_sName(std::move(sName)), m_hComponent(NULL), m_pGraph(pGraph)
	{
		std::string sStateChangedEventName(m_sName);
		sStateChangedEventName.append(":state_changed_event");
		CHECK_VCOS(vcos_event_create(&m_StateChangedEvent, sStateChangedEventName.c_str()), "failed to create vcos event");
		std::string sEventThreadName(m_sName);
		sEventThreadName.append(":event_thread");
		m_EventThread.Start(&CComponent::EventThreadProc, this, sEventThreadName.c_str());
		std::string sEventQueueMutexName(m_sName);
		sEventQueueMutexName.append(":event_queue_mutex");
		vcos_mutex_create(&m_EventQueueMutex, sEventThreadName.c_str());
		OMX_CALLBACKTYPE Callbacks;
		Callbacks.EventHandler = &CComponent::EventHandler;
		Callbacks.EmptyBufferDone = &CComponent::EmptyBufferDone;
		Callbacks.FillBufferDone = &CComponent::FillBufferDone;
		std::string sError("failed to create component ");
		sError.append(m_sName);
		CHECK_OMX(OMX_GetHandle(&m_hComponent, const_cast<char*>(m_sName.c_str()), this, &Callbacks), sError.c_str());
	}
	CComponent::~CComponent()
	{
		try
		{
			ChangeState(OMX_StateIdle);
			CHECK_OMX(OMX_FreeHandle(m_hComponent), "failed to free component");
		}
		catch (std::exception& Exception)
		{
			std::cerr << "Error: " << Exception.what() << std::endl;
		}
	}
	void CComponent::AddPort(CComponentPort& Port)
	{
		m_Ports[Port.m_nId] = &Port;
	}
	void CComponent::ChangeState(OMX_STATETYPE State)
	{
		OMX_STATETYPE CurrentState;
		CHECK_OMX(OMX_GetState(m_hComponent, &CurrentState), "failed to query the current state of the component");
		if (CurrentState == State)
		{
			return;
		}
		CHECK_OMX(OMX_SendCommand(m_hComponent, OMX_CommandStateSet, State, NULL), "component state transtition failed");
		CHECK_VCOS(vcos_event_wait(&m_StateChangedEvent), "failed to wait for component state transition");
	}
	OMX_ERRORTYPE CComponent::EventHandler(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
	{
		switch (eEvent)
		{
		case OMX_EventCmdComplete:
			{
			switch (nData1)
				{
				case OMX_CommandStateSet:
					vcos_event_signal(&m_StateChangedEvent);
					break;
				case OMX_CommandPortEnable:
				case OMX_CommandPortDisable:
					{
						CPortMap::iterator PortIterator = m_Ports.find(nData2);
						if (PortIterator != m_Ports.end())
						{
							if (nData1 == OMX_CommandPortEnable)
							{
								PortIterator->second->OnEnabled();
							}
							else if (nData1 == OMX_CommandPortDisable)
							{
								PortIterator->second->OnDisabled();
							}
						}
					}
					break;
				case OMX_CommandFlush:
					{
						CPortMap::iterator PortIterator = m_Ports.find(nData2);
						if (PortIterator != m_Ports.end())
						{
							PortIterator->second->OnFlushed();
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		case OMX_EventPortSettingsChanged:
			if (m_pGraph)
			{
				CComponentEvent Event;
				Event.m_EventType = OMX_EventPortSettingsChanged;
				Event.m_nData1 = nData1;
				Event.m_nData2 = nData2;
				Event.m_pEventData = pEventData;
				{
					CHECK_VCOS(vcos_mutex_lock(&m_EventQueueMutex), "failed to lock event queue mutex");
					m_EventQueue.push_back(Event);
					vcos_mutex_unlock(&m_EventQueueMutex);
				}
				vcos_event_flags_set(&m_EventThread.GetEventFlags(), s_nNewEventFlag, VCOS_OR);
			}
			break;
		case OMX_EventError:
			std::cerr << "Error: Component " << m_sName << " generated error " << Omx::OmxError(static_cast<OMX_ERRORTYPE>(nData1)) << std::endl;
			break;
		default:
			break;
		}
	}
	OMX_ERRORTYPE CComponent::EmptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)
	{
	}
	OMX_ERRORTYPE CComponent::FillBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)
	{
	}
	OMX_ERRORTYPE CComponent::EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
	{
		CComponent* pComponent = reinterpret_cast<CComponent*>(pAppData);
		return pComponent->EventHandler(eEvent, nData1, nData2, pEventData);
	}
	OMX_ERRORTYPE CComponent::EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
	{
		CComponent* pComponent = reinterpret_cast<CComponent*>(pAppData);
		return pComponent->EmptyBufferDone(pBuffer);
	}
	OMX_ERRORTYPE CComponent::FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
	{
		CComponent* pComponent = reinterpret_cast<CComponent*>(pAppData);
		return pComponent->FillBufferDone(pBuffer);
	}
	void *CComponent::EventThreadProc(VCOS_EVENT_FLAGS_T& EventFlags, VCOS_EVENT_T& InitialzedEvent)
	{
		vcos_event_signal(&InitialzedEvent);
		VCOS_UNSIGNED nEvents;
		try
		{
			for (;;)
			{
				CHECK_VCOS(vcos_event_flags_get(&EventFlags, CThread::s_nTerminationFlag | s_nNewEventFlag, VCOS_CONSUME, VCOS_SUSPEND, &nEvents), "failed to wait for events");
				if (nEvents & CThread::s_nTerminationFlag)
				{
					// Component is being destoryed
					break;
				}
				else if (nEvents & s_nNewEventFlag)
				{
					// New event that cannot be handled in the notification callback
					bool bEventsPending = false;
					do
					{

						CComponentEvent Event;
						{
							CHECK_VCOS(vcos_mutex_lock(&m_EventQueueMutex), "failed to lock event queue mutex");
							Event = m_EventQueue.front();
							m_EventQueue.pop_front();
							bEventsPending = !m_EventQueue.empty();
							vcos_mutex_unlock(&m_EventQueueMutex);
						}
						switch (Event.m_EventType)
						{
						case OMX_EventPortSettingsChanged:
							{
								if (m_pGraph)
								{
									m_pGraph->OnPortSettingsChanged(this, Event.m_nData1);
								}
							}
							break;
						default:
							break;
						}
					}
					while (bEventsPending);
				}
			}
		}
		catch (std::exception& Exception)
		{
			std::cerr << "Error: " << Exception.what() << std::endl;
		}
		return NULL;
	}
}