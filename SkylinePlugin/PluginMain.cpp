#include "stdafx.h"
#include "SkylinePlugIn.h"

CSkylinePlugIn * pMyPlugin = NULL;


//---EuroScopePlugInInit-----------------------------------------------

void __declspec (dllexport) EuroScopePlugInInit(EuroScopePlugIn::CPlugIn ** ppPlugInInstance)
{
	// create the instance
	*ppPlugInInstance = pMyPlugin = new CSkylinePlugIn;
}


//---EuroScopePlugInExit-----------------------------------------------

void __declspec (dllexport) EuroScopePlugInExit()
{
	// delete the instance
	delete pMyPlugin;
}