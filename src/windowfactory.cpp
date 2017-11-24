/***************************************************************************
						windowfactory.cpp   -   description
							-------------------
	begin                : november 23th, 2017
	copyright            : (C) 2006-2008 by Icecool
	email                : icecoolinux @ gmail com
	
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/

#include "windowfactory.h"
#include "guicontainer.h"

// Global settings
#include "globalvar.h"
extern GlobalVar gVars;

// Create a query window.
GUIWindow* 
WindowFactory::createQueryWindow(
	Structure* pstruct
)
{
	// Window's info
	char titleQuery[200];
	int xWin, yWin, widthWin, heightWin;

// Set window size and position
	widthWin = 250;
	heightWin = 300;
	xWin = (gVars.guiScreenWidth - widthWin) / 2;
	yWin = (gVars.guiScreenHeight - heightWin) / 2;

// Variables of Structure's info
	GUILabel* _lblEnergy = 		new GUILabel(widthWin*0.1, heightWin*0.80, "Energy: ");
	GUILabel* _lblWater = 		new GUILabel(widthWin*0.1, heightWin*0.75, "Water: ");
	GUILabel* _lblGas = 		new GUILabel(widthWin*0.1, heightWin*0.70, "Gas: ");
	GUILabel* _lblHasEnergy = 	new GUILabel(widthWin*0.4, heightWin*0.80, "No");
	GUILabel* _lblHasWater = 	new GUILabel(widthWin*0.4, heightWin*0.75, "No");
	GUILabel* _lblHasGas = 		new GUILabel(widthWin*0.4, heightWin*0.70, "No");
	_lblHasEnergy->SetForeground(OPENCITY_PALETTE[Color::OC_RED]);
	_lblHasWater->SetForeground(OPENCITY_PALETTE[Color::OC_RED]);
	_lblHasGas->SetForeground(OPENCITY_PALETTE[Color::OC_RED]);

	// Initialize
	strcpy(titleQuery, "Nothing");

// Look for the RCI structures around and WEG properties
	if ( pstruct != NULL ) {

		switch(pstruct->GetCode()) {
			// Is residential
			case OC_STRUCTURE_RES:
				strcpy(titleQuery, "Residential");
				break;
			// Is commercial
			case  OC_STRUCTURE_COM:
				strcpy(titleQuery, "Commercial");
				break;
			// Is industrial
			case OC_STRUCTURE_IND:
				strcpy(titleQuery, "Industry");
				break;
			// Is coal energy plant
			case OC_STRUCTURE_EPLANT_COAL:
				strcpy(titleQuery, "Coal energy plant");
				break;
			// Is gas energy plant
			case OC_STRUCTURE_EPLANT_GAS:
				strcpy(titleQuery, "Gas energy plant");
				break;
			// Is oil energy plant
			case OC_STRUCTURE_EPLANT_OIL:
				strcpy(titleQuery, "Oil energy plant");
				break;
			// Is nuclear energy plant
			case OC_STRUCTURE_EPLANT_NUCLEAR:
				strcpy(titleQuery, "Nuclear energy plant");
				break;
			// Is a park
			case OC_STRUCTURE_PARK:
				strcpy(titleQuery, "Park");
				break;
			// Is a tree
			case OC_STRUCTURE_FLORA:
				strcpy(titleQuery, "Tree");
				break;
			// Is a road
			case OC_STRUCTURE_ROAD:
				strcpy(titleQuery, "Road");
				break;
			// Is electric line
			case OC_STRUCTURE_ELINE:
				strcpy(titleQuery, "Electric line");
				break;
			// Is fire dept
			case OC_STRUCTURE_FIREDEPT:
				strcpy(titleQuery, "Fire dept");
				break;
			// Is police
			case OC_STRUCTURE_POLICEDEPT:
				strcpy(titleQuery, "Police");
				break;
			// Is hospital
			case OC_STRUCTURE_HOSPITALDEPT:
				strcpy(titleQuery, "Hospital");
				break;
			// Is military
			case OC_STRUCTURE_MILITARYDEPT:
				strcpy(titleQuery, "Military");
				break;
			// Is school
			case OC_STRUCTURE_EDUCATIONDEPT:
				strcpy(titleQuery, "School");
				break;
			// Structure without info query
			default:
				strcpy(titleQuery, "Unknow structure");
				break;
		}

		// Has energy
		if ( pstruct->IsSet( OC_STRUCTURE_E ) ) {
			_lblHasEnergy->SetText("Yes");
			_lblHasEnergy->SetForeground(OPENCITY_PALETTE[Color::OC_GREEN]);
		}
		// Has water
		if ( pstruct->IsSet( OC_STRUCTURE_W ) ) {
			_lblHasWater->SetText("Yes");
			_lblHasWater->SetForeground(OPENCITY_PALETTE[Color::OC_GREEN]);
		}
		// Has gas
		if ( pstruct->IsSet( OC_STRUCTURE_G ) ) {
			_lblHasGas->SetText("Yes");
			_lblHasGas->SetForeground(OPENCITY_PALETTE[Color::OC_GREEN]);
		}
	}

// Create query window
	GUIWindow* winQuery = new GUIWindow( xWin, yWin, widthWin, heightWin, titleQuery );

	((GUIContainer*)winQuery->GetContainer())->Add(_lblEnergy);
	((GUIContainer*)winQuery->GetContainer())->Add(_lblWater);
	((GUIContainer*)winQuery->GetContainer())->Add(_lblGas);
	((GUIContainer*)winQuery->GetContainer())->Add(_lblHasEnergy);
	((GUIContainer*)winQuery->GetContainer())->Add(_lblHasWater);
	((GUIContainer*)winQuery->GetContainer())->Add(_lblHasGas);

	return winQuery;
}

// Create a static window.
GUIWindow* WindowFactory::createStatsWindow()
{
	GUIWindow* statsWin = new GUIWindow(gVars.guiScreenWidth*0.10f, 
										gVars.guiScreenHeight*0.10f, 
										gVars.guiScreenWidth*0.80f, 
										gVars.guiScreenHeight*0.80f,
										"Statistics");
	
	return statsWin;
}

