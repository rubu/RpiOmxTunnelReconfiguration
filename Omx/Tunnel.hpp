#pragma once

#include "ComponentPort.hpp"

namespace Omx
{
	class CTunnel
	{
	public:
		CTunnel(CComponentPort* pSource, CComponentPort* pSink)
		{
			OMX_HANDLETYPE hSource = pSource ? pSource->m_hComponent : 0, hSink = pSink ? pSink->m_hComponent : 0;
			unsigned int nSourcePort = pSource ? pSource->m_nId : 0, nSinkPort = pSink ? pSink->m_nId : 0;
			CHECK_OMX(OMX_SetupTunnel(hSource, nSourcePort, hSink, nSinkPort), "failed to setup tunnel");
		}
	};
};