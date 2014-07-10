#pragma once

#include "Vcos.hpp"

namespace Vcos
{
	class CHeapMemory
	{
	private:
		size_t m_nSize;
		unsigned char* m_pHeapMemory;

	public:
		CHeapMemory(size_t nSize, size_t nAlignment, const char* pMemoryIdentifier) : m_nSize(nSize), m_pHeapMemory(reinterpret_cast<unsigned char*>(vcos_malloc_aligned(m_nSize, nAlignment, pMemoryIdentifier)))
		{
		}
		CHeapMemory(CHeapMemory&& HeapMemory)
		{
			m_pHeapMemory = HeapMemory.m_pHeapMemory;
			m_nSize = HeapMemory.m_nSize;
			HeapMemory.m_pHeapMemory = nullptr;
			HeapMemory.m_nSize = 0;
		}
		~CHeapMemory()
		{
			if (m_pHeapMemory)
			{
				vcos_free(m_pHeapMemory);
			}
		}
		operator unsigned char*()
		{
			return m_pHeapMemory;
		}
		size_t GetSize()
		{
			return m_nSize;
		}
	};
}