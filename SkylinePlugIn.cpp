#include "stdafx.h"
#include <string>
#include "SkylinePlugIn.h"

#define MY_PLUGIN_NAME      "Skyline"
#define MY_PLUGIN_VERSION   "0.1.1" // Update resource version numbers as well
#define MY_PLUGIN_DEVELOPER "Nick Botica (999991)"
#define MY_PLUGIN_COPYRIGHT "Free to be distributed"


const int SL_TAG_ITEM_ALTITUDE_PREFIX = 1;
const int SL_TAG_ITEM_ALTITUDE_TEMP = 2;
const int SL_TAG_ITEM_SPEED_ASSIGNED = 3;
const int SL_TAG_ITEM_GROUND_SPEED = 4;
const int SL_TAG_ITEM_CALLSIGN = 5;
const int SL_TAG_ITEM_AIRCRAFT_CATEGORY = 6;
const int SL_TAG_ITEM_AIRCRAFT_TYPE = 7;
const int SL_TAG_ITEM_ALTITUDE = 8;
const int SL_TAG_ITEM_VERTICAL_SPEED_INDICATOR = 9;
const int SL_TAG_ITEM_TRACKING_CONTROLLER_ID = 10;
const int SL_TAG_ITEM_DESTINATION = 11;
const int SL_TAG_ITEM_ASSIGNED_SQUAWK = 12;
const int SL_TAG_ITEM_SCRATCH_PAD = 13;
const int SL_TAG_ITEM_ASSIGNED_STAR = 14;


//---CSkylinePlugIn------------------------------------------------

CSkylinePlugIn::CSkylinePlugIn()
	: CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE,
		MY_PLUGIN_NAME,
		MY_PLUGIN_VERSION,
		MY_PLUGIN_DEVELOPER,
		MY_PLUGIN_COPYRIGHT)
{
	RegisterTagItemType(".Altitude prefix (A/F)", SL_TAG_ITEM_ALTITUDE_PREFIX);
	RegisterTagItemType(".Temporary altitude", SL_TAG_ITEM_ALTITUDE_TEMP);
	RegisterTagItemType(".Assigned speed (if set)", SL_TAG_ITEM_SPEED_ASSIGNED);
	RegisterTagItemType(".Ground speed", SL_TAG_ITEM_GROUND_SPEED);
	RegisterTagItemType(".Callsign", SL_TAG_ITEM_CALLSIGN);
	RegisterTagItemType(".Aircraft category", SL_TAG_ITEM_AIRCRAFT_CATEGORY);
	RegisterTagItemType(".Aircraft type", SL_TAG_ITEM_AIRCRAFT_TYPE);
	RegisterTagItemType(".Altitude", SL_TAG_ITEM_ALTITUDE);
	RegisterTagItemType(".Vertical speed indicator", SL_TAG_ITEM_VERTICAL_SPEED_INDICATOR);
	RegisterTagItemType(".Tracking controller ID", SL_TAG_ITEM_TRACKING_CONTROLLER_ID);
	RegisterTagItemType(".Destination airport", SL_TAG_ITEM_DESTINATION);
	RegisterTagItemType(".Assigned squawk", SL_TAG_ITEM_ASSIGNED_SQUAWK);
	RegisterTagItemType(".Scratch pad", SL_TAG_ITEM_SCRATCH_PAD);
	RegisterTagItemType(".Assigned STAR", SL_TAG_ITEM_ASSIGNED_STAR);

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
	double * pFontSize) {

	// Set the colour based on the distance to the active sector.
	if (!FlightPlan.IsValid()) { return; }

	int entryTime = FlightPlan.GetSectorEntryMinutes();

	// aircraft either already in your sector OR 10 minutes out = green
	if (entryTime >= 0 && entryTime <= 10) {
		* pColorCode = TAG_COLOR_ASSUMED;
	// aircraft between 20-10 minutes away from your sector = blue
	} else if (entryTime >= 11 && entryTime <= 20) {
		* pColorCode = TAG_COLOR_NOTIFIED;
	// aircraft is not coming into your sector = white
	} else {
		//TODO: for debugging
		//* pColorCode = TAG_COLOR_NON_CONCERNED;
		* pColorCode = TAG_COLOR_ASSUMED;
	}


	switch (ItemCode) {

		// Displays 'A' or 'F' if below or above set transition.
		case SL_TAG_ITEM_ALTITUDE_PREFIX:{

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


		case SL_TAG_ITEM_ALTITUDE_TEMP: {
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


		case SL_TAG_ITEM_SPEED_ASSIGNED: {

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
		case SL_TAG_ITEM_GROUND_SPEED:
		{
			if (!RadarTarget.IsValid()) {
				return;
			}

			int groundSpeed = RadarTarget.GetPosition().GetReportedGS();

			snprintf(sItemString, 16, "%3d", groundSpeed);
		} break;


		case SL_TAG_ITEM_CALLSIGN: {
			snprintf(sItemString, 16, FlightPlan.GetCallsign());
		} break;


		case SL_TAG_ITEM_AIRCRAFT_CATEGORY:	{
			snprintf(sItemString, 16, "%c", FlightPlan.GetFlightPlanData().GetAircraftWtc());
		} break;


		// This could be in the form of e.g T/B738/L, B738/L, B738
		case SL_TAG_ITEM_AIRCRAFT_TYPE:
		{
			//TODO: fix me
			std::string str = FlightPlan.GetFlightPlanData().GetAircraftInfo();
			unsigned first = str.find("/") + 1;
			unsigned last = str.find_last_of("/");
			std::string acType = str.substr(first, last-first);

			snprintf(sItemString, 16, acType.c_str());
		} break;


		// below 0 just show as 000
		case SL_TAG_ITEM_ALTITUDE:
		{
			int currentFL = RadarTarget.GetPosition().GetFlightLevel() / 100;
			
			if (currentFL <= 0) {
				snprintf(sItemString, 16, "000");
			} else {
				//TODO: fix rounding
				snprintf(sItemString, 16, "%03d", currentFL);
			}
		} break;


		// Greater than or less than 100
		case SL_TAG_ITEM_VERTICAL_SPEED_INDICATOR:
		{
			int verticalSpeed = RadarTarget.GetVerticalSpeed();

			if (verticalSpeed < -100)
				snprintf(sItemString, 16, "|");
			else if (verticalSpeed > 100)
				snprintf(sItemString, 16, "^");
			else
				snprintf(sItemString, 16, " ");
		} break;


		case SL_TAG_ITEM_TRACKING_CONTROLLER_ID:
		{
			snprintf(sItemString, 16, FlightPlan.GetTrackingControllerId());
		} break;


		case SL_TAG_ITEM_DESTINATION:
		{
			snprintf(sItemString, 16, FlightPlan.GetFlightPlanData().GetDestination());
		} break;


		case SL_TAG_ITEM_ASSIGNED_SQUAWK:
		{
			const char * squawkReceived = RadarTarget.GetPosition().GetSquawk();
			const char * squawkAssigned = FlightPlan.GetControllerAssignedData().GetSquawk();
			
			// if both squawks are the same
			if (strcmp(squawkReceived, squawkAssigned) == 0) {
				snprintf(sItemString, 16, squawkReceived);
			} else {
				*pColorCode = TAG_COLOR_INFORMATION;
				snprintf(sItemString, 16, squawkAssigned);
			}
		} break;


		case SL_TAG_ITEM_SCRATCH_PAD:
		{
			snprintf(sItemString, 16, FlightPlan.GetControllerAssignedData().GetScratchPadString());
		} break;


		case SL_TAG_ITEM_ASSIGNED_STAR:
		{
			snprintf(sItemString, 16, FlightPlan.GetFlightPlanData().GetStarName());
		} break;

	}
}