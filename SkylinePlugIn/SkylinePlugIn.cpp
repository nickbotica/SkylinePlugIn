#include "stdafx.h"
#include "SkylinePlugIn.h"

#define MY_PLUGIN_NAME      "Skyline"
#define MY_PLUGIN_VERSION   "0.1.1" // Update resource version numbers as well
#define MY_PLUGIN_DEVELOPER "Nick Botica (999991)"
#define MY_PLUGIN_COPYRIGHT "Free to be distributed"


// used for debugging
const int TAG_ITEM_DEBUG = 0;
// display an 'A' if below transition, otherwise display 'F'
const int TAG_ITEM_ALTITUDE_PREFIX = 1;
// not sure if needed anymore
const int TAG_ITEM_ALTITUDE_TEMP = 2;
// Display the controller assigned speed with appended "KT"
const int TAG_ITEM_SPEED_ASSIGNED = 3;
// Display the ground speed with correct formatting
const int TAG_ITEM_GROUND_SPEED = 4;


//---CSkylinePlugIn------------------------------------------------

CSkylinePlugIn::CSkylinePlugIn()
	: CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE,
		MY_PLUGIN_NAME,
		MY_PLUGIN_VERSION,
		MY_PLUGIN_DEVELOPER,
		MY_PLUGIN_COPYRIGHT)
{
	//RegisterTagItemType("Debug", TAG_ITEM_DEBUG);
	RegisterTagItemType("Altitude prefix (A/F)", TAG_ITEM_ALTITUDE_PREFIX);
	RegisterTagItemType("Temporary altitude", TAG_ITEM_ALTITUDE_TEMP);
	RegisterTagItemType("Assigned speed (if set)", TAG_ITEM_SPEED_ASSIGNED);
	RegisterTagItemType("Ground speed", TAG_ITEM_GROUND_SPEED);

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

		case TAG_ITEM_DEBUG: {
			snprintf(sItemString, 16, "%d", FlightPlan.GetControllerAssignedData().GetClearedAltitude());
		} break;


		case TAG_ITEM_ALTITUDE_PREFIX:{

			if (RadarTarget.IsValid()) {
			// Above transition, e.g. 12,950 feet
				if (RadarTarget.GetPosition().GetFlightLevel() >= GetTransitionAltitude() - 50) {
					snprintf(sItemString, 16, "F");
				} else {
					snprintf(sItemString, 16, "A");
				}
			}
			// For Flightplan track
			else if (FlightPlan.IsValid()) {
				if (FlightPlan.GetFPTrackPosition().GetFlightLevel() >= GetTransitionAltitude() - 50) {
					snprintf(sItemString, 16, "F");
				} else {
					snprintf(sItemString, 16, "A");
				}
			}
			else {
				return;
			}

		} break;


		case TAG_ITEM_ALTITUDE_TEMP: {
			/*
			If no controller is online so no descent altidude has been given, the AC will descend
			when ready and the tag will display the flight plan final alt all the way to the ground. 
			*/
			if (!RadarTarget.IsValid() && !FlightPlan.IsValid()) {
				return;
			}

			int currentFL = RadarTarget.GetPosition().GetFlightLevel();
			int tempAlt = FlightPlan.GetControllerAssignedData().GetClearedAltitude();
			int finalAlt = FlightPlan.GetFinalAltitude();

			// if cleared for instrument approach
			if (tempAlt == 1) {
				snprintf(sItemString, 16, "APP");
			}
			// if cleared visual approach
			else if (tempAlt == 2) {
				snprintf(sItemString, 16, "VIS");
			}
			// if a temp altitude is set and it's not the final alt, display it
			else if ((tempAlt >= 100) && (tempAlt != finalAlt)) {
				snprintf(sItemString, 16, "%03d", tempAlt / 100);
			}
			// if aircraft is cruising at final altitude +-50ft display nothing
			else if ((currentFL >= finalAlt - 50) && (currentFL < finalAlt + 50)) {
				snprintf(sItemString, 16, "   ");
			}
			// no temp alt set display FP final alt
			else {
				snprintf(sItemString, 16, "%03d", finalAlt / 100);
			}

		} break;


		case TAG_ITEM_SPEED_ASSIGNED: {

			if (!FlightPlan.IsValid()) {
				return;
			}

			int asgdSpeed = FlightPlan.GetControllerAssignedData().GetAssignedSpeed();
			int asgdMach = FlightPlan.GetControllerAssignedData().GetAssignedMach();

			if (asgdSpeed > 0) {
				snprintf(sItemString, 16, "%03dKT", asgdSpeed);
			}
			else if (asgdMach > 0) {
				snprintf(sItemString, 16, "%03dMa", asgdMach);
			}
			else {
				snprintf(sItemString, 16, "");
			}
		} break;

		// Print with no leading 0's
		case TAG_ITEM_GROUND_SPEED:
		{
			if (!RadarTarget.IsValid()) {
				return;
			}

			int groundSpeed = RadarTarget.GetPosition().GetReportedGS();

			snprintf(sItemString, 16, "%3d", groundSpeed);
		} break;
	}
}