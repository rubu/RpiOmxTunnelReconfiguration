#pragma once

extern "C"
{
#include "IL/OMX_Broadcom.h"
}

#include <sstream>
#include <stdexcept>
#include <iostream>

#define OMX_INIT_STRUCTURE(Structure) \
	memset(&(Structure), 0, sizeof(Structure)); \
	(Structure).nSize = sizeof(Structure); \
	(Structure).nVersion.s.nVersionMajor = OMX_VERSION_MAJOR; \
	(Structure).nVersion.s.nVersionMinor = OMX_VERSION_MINOR; \
	(Structure).nVersion.s.nRevision = OMX_VERSION_REVISION; \
	(Structure).nVersion.s.nStep = OMX_VERSION_STEP

#define CHECK_OMX(expression, error)  {OMX_ERRORTYPE nError = (expression); if (nError != OMX_ErrorNone){ std::stringstream ErrorStream; ErrorStream << __FILE__ << "(" << __LINE__ << "): " << Omx::OmxError(nError) << " - " << error; throw std::runtime_error(ErrorStream.str());}}

namespace Omx
{
	static const char* OmxError(OMX_ERRORTYPE nErrorCode)
	{
		switch (nErrorCode)
		{
		case OMX_ErrorInsufficientResources:
			return "OMX_ErrorInsufficientResources";
		case OMX_ErrorUndefined:
			return "OMX_ErrorUndefined";
		case OMX_ErrorInvalidComponentName:
			return "OMX_ErrorInvalidComponentName";
		case OMX_ErrorComponentNotFound:
			return "OMX_ErrorComponentNotFound";
		case OMX_ErrorInvalidComponent:
			return "OMX_ErrorInvalidComponent";
		case OMX_ErrorBadParameter:
			return "OMX_ErrorBadParameter";
		case OMX_ErrorNotImplemented:
			return "OMX_ErrorNotImplemented";
		case OMX_ErrorUnderflow:
			return "OMX_ErrorUnderflow";
		case OMX_ErrorOverflow:
			return "OMX_ErrorOverflow";
		case OMX_ErrorHardware:
			return "OMX_ErrorHardware";
		case OMX_ErrorInvalidState:
			return "OMX_ErrorInvalidState";
		case OMX_ErrorStreamCorrupt:
			return "OMX_ErrorStreamCorrupt";
		case OMX_ErrorPortsNotCompatible:
			return "OMX_ErrorPortsNotCompatible";
		case OMX_ErrorResourcesLost:
			return "OMX_ErrorResourcesLost";
		case OMX_ErrorNoMore:
			return "OMX_ErrorNoMore";
		case OMX_ErrorVersionMismatch:
			return "OMX_ErrorVersionMismatch";
		case OMX_ErrorNotReady:
			return "OMX_ErrorNotReady";
		case OMX_ErrorTimeout:
			return "OMX_ErrorTimeout";
		case OMX_ErrorSameState:
			return "OMX_ErrorSameState";
		case OMX_ErrorResourcesPreempted:
			return "OMX_ErrorResourcesPreempted";
		case OMX_ErrorPortUnresponsiveDuringAllocation:
			return "OMX_ErrorPortUnresponsiveDuringAllocation";
		case OMX_ErrorPortUnresponsiveDuringDeallocation:
			return "OMX_ErrorPortUnresponsiveDuringDeallocation";
		case OMX_ErrorPortUnresponsiveDuringStop:
			return "OMX_ErrorPortUnresponsiveDuringStop";
		case OMX_ErrorIncorrectStateTransition:
			return "OMX_ErrorIncorrectStateTransition";
		case OMX_ErrorIncorrectStateOperation:
			return "OMX_ErrorIncorrectStateOperation";
		case OMX_ErrorUnsupportedSetting:
			return "OMX_ErrorUnsupportedSetting";
		case OMX_ErrorUnsupportedIndex:
			return "OMX_ErrorUnsupportedIndex";
		case OMX_ErrorBadPortIndex:
			return "OMX_ErrorBadPortIndex";
		case OMX_ErrorPortUnpopulated:
			return "OMX_ErrorPortUnpopulated";
		case OMX_ErrorComponentSuspended:
			return "OMX_ErrorComponentSuspended";
		case OMX_ErrorDynamicResourcesUnavailable:
			return "OMX_ErrorDynamicResourcesUnavailable";
		case OMX_ErrorMbErrorsInFrame:
			return "OMX_ErrorMbErrorsInFrame";
		case OMX_ErrorFormatNotDetected:
			return "OMX_ErrorFormatNotDetected";
		case OMX_ErrorContentPipeOpenFailed:
			return "OMX_ErrorContentPipeOpenFailed";
		case OMX_ErrorContentPipeCreationFailed:
			return "OMX_ErrorContentPipeCreationFailed";
		case OMX_ErrorSeperateTablesUsed:
			return "OMX_ErrorSeperateTablesUsed";
		case OMX_ErrorTunnelingUnsupported:
			return "OMX_ErrorTunnelingUnsupported";
		case OMX_ErrorKhronosExtensions:
			return "OMX_ErrorKhronosExtensions";
		case OMX_ErrorVendorStartUnused:
			return "OMX_ErrorVendorStartUnused";
		case OMX_ErrorDiskFull:
			return "OMX_ErrorDiskFull";
		case OMX_ErrorMaxFileSize:
			return "OMX_ErrorMaxFileSize";
		case OMX_ErrorDrmUnauthorised:
			return "OMX_ErrorDrmUnauthorised";
		case OMX_ErrorDrmExpired:
			return "OMX_ErrorDrmExpired";
		case OMX_ErrorDrmGeneral:
			return "OMX_ErrorDrmGeneral";
		default:
			return "Unknown Omx error";
		}
	}
	class COmx
	{
	public:
		COmx()
		{
			CHECK_OMX(OMX_Init(), "failed to initialize OMX");
		}
		~COmx()
		{
			try
			{
				CHECK_OMX(OMX_Deinit(), "failed to deinitialize OMX");
			}
			catch (std::exception& Exception)
			{
				std::cerr << Exception.what() << std::endl;
			}
		}
	};
}