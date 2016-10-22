#pragma once
#include <cstdio>
#include "EuroScopePlugIn.h"

using namespace EuroScopePlugIn;

class CSkylinePlugIn :
	public EuroScopePlugIn::CPlugIn
{
public:
	CSkylinePlugIn();
	virtual ~CSkylinePlugIn();

	void OnGetTagItem(CFlightPlan FlightPlan,
		CRadarTarget RadarTarget,
		int ItemCode,
		int TagData,
		char sItemString[16],
		int * pColorCode,
		COLORREF * pRGB,
		double * pFontSize);


};

