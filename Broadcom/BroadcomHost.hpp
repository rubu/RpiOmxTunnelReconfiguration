#pragma once

extern "C"
{
#include "bcm_host.h"
}

namespace Broadcom
{
	class CBroadcomHost
	{
	public:
		CBroadcomHost()
		{
			bcm_host_init();
		}
		~CBroadcomHost()
		{
			bcm_host_deinit();
		}
	};
}

