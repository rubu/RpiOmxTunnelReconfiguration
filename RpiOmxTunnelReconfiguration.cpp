#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "Error.hpp"
#include "H264PlaybackGraph.hpp"
#include "Broadcom/BroadcomHost.hpp"
#include "Omx/Omx.hpp"

int main(int argc, char *argv[])
{
	try
	{
		FILE* pH264File = fopen("sample.h264", "rb");
		CHECK_EXPRESSION(pH264File, "failed to open sample.h264");
		Broadcom::CBroadcomHost BroadcommHost;
		Omx::COmx Omx;
		CH264PlaybackGraph Graph;
		size_t nBytesRead;
		bool bFirstPacket = true;
		do
		{
			OMX_BUFFERHEADERTYPE* pBuffer = Graph.GetInputBuffer();
			nBytesRead = fread(pBuffer->pBuffer, 1, pBuffer->nAllocLen - pBuffer->nFilledLen, pH264File);
			pBuffer->nFilledLen += nBytesRead;
			if (bFirstPacket)
			{
				pBuffer->nFlags = OMX_BUFFERFLAG_STARTTIME;
				bFirstPacket = false;
			}
			else
			{
				pBuffer->nFlags = OMX_BUFFERFLAG_TIME_UNKNOWN;
			}
			if (nBytesRead == 0)
			{
				pBuffer->nFlags |= OMX_BUFFERFLAG_EOS;
			}
			Graph.ProcessBuffer(pBuffer);
		}
		while (nBytesRead != 0);
		std::cout << "Debug: end of sample.h264 reached" << std::endl;
		std::cout << "Press ENTER to exit..." << std::endl;
		std::cin.get();
		fclose(pH264File);
	}
	catch (std::exception& Exception)
	{
		std::cerr << "Error: " << Exception.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}