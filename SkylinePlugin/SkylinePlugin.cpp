#include "stdafx.h"
#include "SkylinePlugIn.h"

#define MY_PLUGIN_NAME      "Skyline"
#define MY_PLUGIN_VERSION   "0.1.3"
#define MY_PLUGIN_DEVELOPER "Nick Botica (999991)"
#define MY_PLUGIN_COPYRIGHT "Free to be distributed"

// display an 'A' if below transition, otherwise display 'F'
const int TAG_ITEM_ALTITUDE_PREFIX = 1;
// not sure if needed anymore
const int TAG_ITEM_ALTITUDE_TEMP = 2;
// Display the controller assigned speed with appended "KT"
const int TAG_ITEM_SPEED_ASSIGNED = 3;

//---CSkylinePlugIn------------------------------------------------

CSkylinePlugIn::CSkylinePlugIn()
	: CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE,
		MY_PLUGIN_NAME,
		MY_PLUGIN_VERSION,
		MY_PLUGIN_DEVELOPER,
		MY_PLUGIN_COPYRIGHT)
{
	RegisterTagItemType("Altitude prefix (A/F)", TAG_ITEM_ALTITUDE_PREFIX);
	RegisterTagItemType("Temporary altitude", TAG_ITEM_ALTITUDE_TEMP);
	RegisterTagItemType("Assigned speed (if set)", TAG_ITEM_SPEED_ASSIGNED);
}


//---~CSkylinePlugIn-----------------------------------------------

CSkylinePlugIn::~CSkylinePlugIn()
{
}


//---OnGetTagItem----------------------------------------------------

void CSkylinePlugIn::OnGetTagItem(CFlightPlan FlightPlan,
	CRadarTarget RadarTarget,
	int ItemCode,
	int TagData,
	char sItemString[16],
	int * pColorCode,
	COLORREF * pRGB,
	double * pFontSize)
{


	switch (ItemCode) {

	case TAG_ITEM_ALTITUDE_PREFIX: {

		if (!RadarTarget.IsValid()) {
			return;
		}

		if (RadarTarget.GetPosition().GetFlightLevel() >= GetTransitionAltitude() - 50) {
			snprintf(sItemString, 16, "F");
		}
		else {
			snprintf(sItemString, 16, "A");
		}
	} break;


	case TAG_ITEM_ALTITUDE_TEMP: {

		if (!RadarTarget.IsValid() && !FlightPlan.IsValid()) {
			return;
		}

		int currentFL = RadarTarget.GetPosition().GetFlightLevel();
		int tempAlt = FlightPlan.GetControllerAssignedData().GetClearedAltitude();
		int finalAlt = FlightPlan.GetFinalAltitude();

		// if cleared for instrument approach. Placeholder
		if (tempAlt == 1) {
			snprintf(sItemString, 16, "ILS");
		}
		// if cleared visual approach. Placeholder
		else if (tempAlt == 2) {
			snprintf(sItemString, 16, "VIS");
		}
		// if aircraft is cruising at final altitude display nothing.
		else if (currentFL == finalAlt) {
			snprintf(sItemString, 16, "");
		}
		// no temp alt set
		else if (tempAlt == 0) {
			snprintf(sItemString, 16, "%d", finalAlt);
		}
		// if a temp altitude is set
		else if (tempAlt >= 100) {
			snprintf(sItemString, 16, "%d", tempAlt);
		}

	} break;


	case TAG_ITEM_SPEED_ASSIGNED: {

		if (!FlightPlan.IsValid()) {
			return;
		}

		int asgdSpeed = FlightPlan.GetControllerAssignedData().GetAssignedSpeed();

		if (asgdSpeed != 0) {
			snprintf(sItemString, 16, "%03dKT", asgdSpeed);
		}
	} break;
	}
}