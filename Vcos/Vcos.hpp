#pragma once

#include "interface/vcos/vcos.h"

#define CHECK_VCOS(expression, error)  {VCOS_STATUS_T nError = (expression); if (nError != VCOS_SUCCESS){ std::stringstream ErrorStream; ErrorStream << __FILE__ << "(" << __LINE__ << "): " << Vcos::VcosError(nError) << " - " << error; throw std::runtime_error(ErrorStream.str());}}

namespace Vcos
{
	static const char* VcosError(VCOS_STATUS_T nErrorCode)
	{
		switch (nErrorCode)
		{
		case VCOS_SUCCESS:
			return "VCOS_SUCCESS";
		case VCOS_EAGAIN:
			return "VCOS_EAGAIN";
		case VCOS_ENOENT:
			return "VCOS_ENOENT";
		case VCOS_ENOSPC:
			return "VCOS_ENOSPC";
		case VCOS_EINVAL:
			return "VCOS_EINVAL";
		case VCOS_EACCESS:
			return "VCOS_EACCESS";
		case VCOS_ENOMEM:
			return "VCOS_ENOMEM";
		case VCOS_ENOSYS:
			return "VCOS_ENOSYS";
		case VCOS_EEXIST:
			return "VCOS_EEXIST";
		case VCOS_ENXIO:
			return "VCOS_ENXIO";
		case VCOS_EINTR:
			return "VCOS_EINTR";
		default:
			return "Unknown Vcos error";
		}
	}
}