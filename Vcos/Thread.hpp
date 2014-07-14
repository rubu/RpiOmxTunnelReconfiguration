#pragma once

#include "Vcos.hpp"

namespace Vcos
{
	template <typename T>
	class CThreadT
	{
	private:
		typedef void *(T::*CThreadProc)(VCOS_EVENT_FLAGS_T&, VCOS_EVENT_T&);

		VCOS_THREAD_T m_Thread;

	public:
		CThreadProc m_pThreadProc;
		VCOS_EVENT_FLAGS_T m_EventFlags;
		VCOS_EVENT_T m_InitializedEvent;
		T* m_pInstance;

	public:
		static const VCOS_UNSIGNED s_nTerminationFlag = 1;

	public:
		CThreadT() : m_pInstance(NULL), m_pThreadProc(NULL)
		{
			CHECK_VCOS(vcos_event_create(&m_InitializedEvent, ""), "failed to create thread initialization event");
			CHECK_VCOS(vcos_event_flags_create(&m_EventFlags, ""), "failed to create thread event flags");
		}
		~CThreadT()
		{
			vcos_event_flags_set(&m_EventFlags, s_nTerminationFlag, VCOS_OR);
			void* pData;
			vcos_thread_join(&m_Thread, &pData);
			vcos_event_flags_delete(&m_EventFlags);
			vcos_event_delete(&m_InitializedEvent);
		}
		void Start(CThreadProc pThreadProc, T* pInstance, const char* pThreadName = "")
		{
			m_pThreadProc = pThreadProc;
			m_pInstance = pInstance;
			CHECK_VCOS(vcos_thread_create(&m_Thread, pThreadName, NULL, static_cast<VCOS_THREAD_ENTRY_FN_T>(&CThreadT::ThreadProc), this), "failed to create thread");
			CHECK_VCOS(vcos_event_wait(&m_InitializedEvent), "failed to wait for the thread to start");
		}
		VCOS_EVENT_FLAGS_T& GetEventFlags()
		{
			return m_EventFlags;
		}
		static void *ThreadProc(void* pArgument)
		{
			CThreadT* pThis = static_cast<CThreadT*>(pArgument);
			if (pThis->m_pInstance && pThis->m_pThreadProc)
			{
				(*pThis->m_pInstance.*pThis->m_pThreadProc)(pThis->m_EventFlags, pThis->m_InitializedEvent);
			}
		}
	};
}