#pragma once

#include "Omx.hpp"
#include "ComponentPort.hpp"
#include "Graph.hpp"
#include "../Vcos/Vcos.hpp"
#include "../Vcos/Thread.hpp"

#include <string>
#include <map>
#include <deque>

namespace Omx
{
	class CComponent
	{
	friend class CComponentPort;
	
	typedef std::map<unsigned int, CComponentPort*> CPortMap;
	typedef Vcos::CThreadT<CComponent> CThread;

	class CComponentEvent
	{
	friend class CComponent;
	private:
		OMX_EVENTTYPE m_EventType;
		OMX_U32 m_nData1;
		OMX_U32 m_nData2;
		OMX_PTR m_pEventData;
	};

	protected:
		std::string m_sName;
		OMX_HANDLETYPE m_hComponent;
		VCOS_EVENT_T m_StateChangedEvent;
		CPortMap m_Ports;
		CGraph* m_pGraph;
		CThread m_EventThread;
		static const VCOS_UNSIGNED s_nNewEventFlag = 2;
		std::deque<CComponentEvent> m_EventQueue;
		VCOS_MUTEX_T m_EventQueueMutex;

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

	protected:
		void *EventThreadProc(VCOS_EVENT_FLAGS_T& EventFlags, VCOS_EVENT_T& InitialzedEvent);

	};
}