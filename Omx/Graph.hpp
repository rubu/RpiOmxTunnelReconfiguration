#pragma once


namespace Omx
{
	class CComponent;

	class CGraph
	{
	public:
		virtual void OnPortSettingsChanged(CComponent* pComponent, unsigned int nPortId) = 0;
	};
}