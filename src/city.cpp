/***************************************************************************
						city.cpp  -  description
							-------------------
	begin                : may 28th, 2003
	copyright            : (C) 2003-2015 by Duong Khang NGUYEN
	email                : neoneurone @ gmail com

	$Id: city.cpp 476 2015-05-26 19:47:59Z neoneurone $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/

// Useful enumerations
#include "enum.h"

// OpenCity headers
#include "city.h"
#include "vehicle.h"
#include "mainsim.h"				// Simulator
#include "structure.h"
#include "buildinglayer.h"
#include "guibar.h"					// RCI bar
#include "guibutton.h"				// Tool button
#include "guilabel.h"				// Status label
#include "guicontainer.h"			// GUI control container
#include "guiwindow.h"
#include "agentpolice.h"
#include "agentdemonstrator.h"
#include "agentrobber.h"
#include "systime.h"
#include "windowfactory.h"

// Global settings
#include "globalvar.h"
extern GlobalVar gVars;

// Standard headers
#include <sstream>					// For text output with data conversion

using namespace std;

// Local defines
#define OC_ACTION_FACTOR 10
#define GUIBUTTON_POSITION_QUERY	54, 4, 24, 24
#define GUIBUTTON_POSITION_PAUSE	54, 35, 24, 24
#define GUIBUTTON_POSITION_PLAY		81, 35, 24, 24
#define GUIBUTTON_POSITION_FAST		108, 35, 24, 24

#define SIZE_BUTTON_TOOLCIRCLE 45
#define RADIO_TOOLCIRCLE 80
#define DIST_CENTER (RADIO_TOOLCIRCLE - SIZE_BUTTON_TOOLCIRCLE/2)
#define GUIBUTTON_POSITION_1		RADIO_TOOLCIRCLE - DIST_CENTER*0.5f - SIZE_BUTTON_TOOLCIRCLE/2, RADIO_TOOLCIRCLE + DIST_CENTER*0.87f - SIZE_BUTTON_TOOLCIRCLE/2,	SIZE_BUTTON_TOOLCIRCLE, SIZE_BUTTON_TOOLCIRCLE
#define GUIBUTTON_POSITION_2		RADIO_TOOLCIRCLE - DIST_CENTER - SIZE_BUTTON_TOOLCIRCLE/2, 		RADIO_TOOLCIRCLE - SIZE_BUTTON_TOOLCIRCLE/2, 						SIZE_BUTTON_TOOLCIRCLE, SIZE_BUTTON_TOOLCIRCLE
#define GUIBUTTON_POSITION_3		RADIO_TOOLCIRCLE - DIST_CENTER*0.5f - SIZE_BUTTON_TOOLCIRCLE/2, RADIO_TOOLCIRCLE - DIST_CENTER*0.87f - SIZE_BUTTON_TOOLCIRCLE/2, 	SIZE_BUTTON_TOOLCIRCLE, SIZE_BUTTON_TOOLCIRCLE
#define GUIBUTTON_POSITION_4		RADIO_TOOLCIRCLE + DIST_CENTER*0.5f - SIZE_BUTTON_TOOLCIRCLE/2, RADIO_TOOLCIRCLE - DIST_CENTER*0.87f - SIZE_BUTTON_TOOLCIRCLE/2,	SIZE_BUTTON_TOOLCIRCLE, SIZE_BUTTON_TOOLCIRCLE
#define GUIBUTTON_POSITION_5		RADIO_TOOLCIRCLE + DIST_CENTER - SIZE_BUTTON_TOOLCIRCLE/2, 		RADIO_TOOLCIRCLE - SIZE_BUTTON_TOOLCIRCLE/2, 						SIZE_BUTTON_TOOLCIRCLE, SIZE_BUTTON_TOOLCIRCLE
#define GUIBUTTON_POSITION_6		RADIO_TOOLCIRCLE + DIST_CENTER*0.5f - SIZE_BUTTON_TOOLCIRCLE/2, RADIO_TOOLCIRCLE + DIST_CENTER*0.87f - SIZE_BUTTON_TOOLCIRCLE/2, 	SIZE_BUTTON_TOOLCIRCLE, SIZE_BUTTON_TOOLCIRCLE

   /*=====================================================================*/
City::City
(
	const uint width,
	const uint length,
	const bool bGUIEnabled
):
_bGUIEnabled( bGUIEnabled ),
_bStatusVisible( true ),
_uiIncome( 0 ),
_liCityFund( OC_FUND_START ),
_uiPopulation( 0 ),

_uiDay( 1 ),
_uiMonth( 1 ),
_uiYear( 0 ),

_uiWidth( width ),
_uiLength( length ),

_bLMBPressedOverMap( false ),
_bLMBPressed( false ),
_bRMBPressed( false ),

_bMoveCamera(false),
_bRotateCamera(false),
_iXMove(0),
_iYMove(0),

_eCurrentLayer( OC_LAYER_BUILDING ),
_eSpeed( OC_SPEED_NORMAL ),
_eCurrentTool( OC_TOOL_NONE ),

_pctrMenu( NULL )
{
	OPENCITY_DEBUG( "City ctor - default parameters" );

// Set the windows's WH
	_iWinWidth = gVars.guiScreenWidth;
	_iWinHeight = gVars.guiScreenHeight;

// Registering our call-back interface for SDL events' treatment
	ocSetNewUI( this );

// Keyboard bool table's initialization
	int i;
	for (i = 0; i < KEY_NUMBER; i++)
		_abKeyPressed[i] = false;

// Initialize the statistics
	Ressource res = {0, 0, 0, 0, 0, 0, 0, 0};
	for (i = 0; i < OC_MAX_RESSOURCE_RECORD; i++) {
		_dqRessource.push_back(res);
	}

// Layers' initialization
	_apLayer[ OC_LAYER_BUILDING ] = new BuildingLayer( *this );
	gVars.gpMapMgr->SetLayer( _apLayer[OC_LAYER_BUILDING] );

// Put few trees on the building layer
	_CreateTree();

// Pathfinder initialization
	gVars.gpPathFinder = new PathFinder(
		(BuildingLayer*)_apLayer[ OC_LAYER_BUILDING ],
		gVars.gpMapMgr,
		_uiWidth, _uiLength );

// Simulators' initialization
	_pMSim = new MainSim( (BuildingLayer*)_apLayer[ OC_LAYER_BUILDING ], gVars.gpMapMgr );

	if (_bGUIEnabled) {
	// Debug toolcircle
		boolPathGo = false;
		uiPathStartW = 0; uiPathStartH = 0;
		uiPathStopW = 0; uiPathStopH = 0;
		_pctrPath = new GUIContainer( 100, 100, 140, 140, ocDataDirPrefix( "graphism/gui/toolcircle_bg.png" ));
		pbtnPathStart = new GUIButton( 20,  20,  30, 30, ocDataDirPrefix( "graphism/gui/raise" ));
		pbtnPathStop1 = new GUIButton( 60,  0,   30, 30, ocDataDirPrefix( "graphism/gui/lower" ));
		pbtnPathStop2 = new GUIButton( 100, 20,  30, 30, ocDataDirPrefix( "graphism/gui/lower" ));
		pbtnTestBuilding = new GUIButton(  20,  70, 30, 30, ocDataDirPrefix( "graphism/gui/residential" ));
		_pctrPath->Add( pbtnPathStart );
		_pctrPath->Add( pbtnPathStop1 );
		_pctrPath->Add( pbtnPathStop2 );
		_pctrPath->Add( pbtnTestBuilding );
		pvehicle = NULL;

	// Create the GUI
		_CreateGUI();
	}
}


   /*=====================================================================*/
City::~City()
{
	OPENCITY_DEBUG( "City dtor" );

	if (_bGUIEnabled) {
	// Delete the GUI
		_DeleteGUI();

	// testing, delete the pathfinder
		delete _pctrPath;
		delete pbtnTestBuilding;
		delete pbtnPathStart;
		delete pbtnPathStop1;
		delete pbtnPathStop2;
	}

// delete all the simulators
	delete _pMSim;

// Delete the pathfinder
	delete gVars.gpPathFinder;
	gVars.gpPathFinder = NULL;

// delete only the OC_LAYER_BUILDING instead of delete [] _apLayer
	delete _apLayer[ OC_LAYER_BUILDING ];
	_apLayer[ OC_LAYER_BUILDING ] = NULL;

// this must be done AFTER deleting the layers
//	delete gVars.gpMapMgr;
}


   /*=====================================================================*/
void
City::SaveTo( std::fstream& rfs )
{
	OPENCITY_DEBUG( __PRETTY_FUNCTION__ << "saving" );

// Save the data
	rfs << _uiIncome << std::ends;
	rfs << _liCityFund << std::ends;
	rfs << _uiPopulation << std::ends;
	rfs << _uiDay << std::ends;
	rfs << _uiMonth << std::ends;
	rfs << _uiYear << std::ends;
	rfs << _uiWidth << std::ends;
	rfs << _uiLength << std::ends;

// Store the ressource statistics
	Ressource res;
	int i;
	for (i = 0; i < OC_MAX_RESSOURCE_RECORD; i++) {
		res = _dqRessource[i];

		rfs << res.fund << std::ends;
		rfs << res.population << std::ends;
		rfs << res.r << std::ends;
		rfs << res.c << std::ends;
		rfs << res.i << std::ends;
		rfs << res.w << std::ends;
		rfs << res.e << std::ends;
		rfs << res.g << std::ends;
	}
}


   /*=====================================================================*/
void
City::LoadFrom( std::fstream& rfs )
{
	OPENCITY_DEBUG( __PRETTY_FUNCTION__ << "loading" );

// Load the data
	rfs >> _uiIncome; rfs.ignore();
	rfs >> _liCityFund; rfs.ignore();
	rfs >> _uiPopulation; rfs.ignore();
	rfs >> _uiDay; rfs.ignore();
	rfs >> _uiMonth; rfs.ignore();
	rfs >> _uiYear; rfs.ignore();
	rfs >> _uiWidth; rfs.ignore();
	rfs >> _uiLength; rfs.ignore();

// Load the ressource statistics
	Ressource res;
	int i;
	for (i = 0; i < OC_MAX_RESSOURCE_RECORD; i++) {
		rfs >> res.fund; rfs.ignore();
		rfs >> res.population; rfs.ignore();
		rfs >> res.r; rfs.ignore();
		rfs >> res.c; rfs.ignore();
		rfs >> res.i; rfs.ignore();
		rfs >> res.w; rfs.ignore();
		rfs >> res.e; rfs.ignore();
		rfs >> res.g; rfs.ignore();

		_dqRessource[i] = res;
	}
}


   /*=====================================================================*/
void City::SetCurrentLayer( OPENCITY_CITY_LAYER enumNewLayer )
{}


   /*=====================================================================*/
void City::Run()
{
	static uint uiNumberFrame = 0;

// Run simulation.
	_pMSim->Run();
	
// Send the movement manager the move order
	gVars.gpMoveMgr->Move();

// IF the audio is enable THEN autoplay the background music
// TODO: optimize this
	if (gVars.gboolUseAudio && !gVars.gpAudioMgr->PlayingMusic()) {
		gVars.gpAudioMgr->PlayNextMusic();
	}

// Do not process further if we are in pause mode
	if (_eSpeed == OC_SPEED_PAUSE)
		return;

// IF not new day THEN return
	uiNumberFrame++;
	if ( _eSpeed == OC_SPEED_NORMAL && uiNumberFrame*gVars.guiMsPerFrame <= OC_MS_PER_DAY )
		return;
	if ( _eSpeed == OC_SPEED_FAST && uiNumberFrame*gVars.guiMsPerFrame <= OC_MS_PER_DAY_FAST )
		return;

// New day
	uint r = _pMSim->GetValue(Simulator::OC_RESIDENTIAL);
	uint c = _pMSim->GetValue(Simulator::OC_COMMERCIAL);
	uint i = _pMSim->GetValue(Simulator::OC_INDUSTRIAL);

	if ( ++_uiDay > 30 ) {
	// New month
		_uiDay = 1;

	// Calculate the current population
		_uiPopulation = r + c/2 + i/3;

		_DoBill();
		
		if ( ++_uiMonth > 12 ) {
		// New year
			_uiMonth = 1;
			_uiYear++;

			_RecordRessource();
		}
	}
	uiNumberFrame = 0;

// IF the GUI is not enabled THEN do not update it
	if (!_bGUIEnabled)
		return;

// Update the GUI information every 3 days
	if ( _uiDay%3 == 0 ) {
		uint initialValue = 0;

	// Update the RCI bar value
		initialValue = (r+c+i) + 1;
		_pbarResidence->SetInitialValue( initialValue );
		_pbarCommerce->SetInitialValue( initialValue );
		_pbarIndustry->SetInitialValue( initialValue );
		_pbarResidence->SetValue( r );
		_pbarCommerce->SetValue( c );
		_pbarIndustry->SetValue( i );

	// Update the power bar value
		initialValue = _pMSim->GetMaxValue(Simulator::OC_ELECTRIC) + 1;
		_pbarPower->SetInitialValue( initialValue );
		uint p = _pMSim->GetValue(Simulator::OC_ELECTRIC) > 0 ? _pMSim->GetValue(Simulator::OC_ELECTRIC) : 0;
		_pbarPower->SetValue( p );

	// Request the renderer to update the minimap
		gVars.gpRenderer->bMinimapChange = true;
	}
}


   /*=====================================================================*/
void City::Display()
{
	static ostringstream ossStatus;
	static bool boolKeyDown;
	static int iMouseX, iMouseY;
	static int mouseState;

	bool drawScene = true;

// Get mouse state
	mouseState = SDL_GetMouseState( &iMouseX, &iMouseY );

// Process key events such as: up, down, left, right etc..
	boolKeyDown = _HandleKeyPressed();

// Process move and rotation mouse.
	_ProcessMoveRotationMouse();

// IF the mouse reach the border of the screen, translate the map
// NOTE: unused at the moment because it disturbs the GUI
//	_HandleMouseXY();

// NOTE: We can move the following part to City::uiMouseMotion
// however, in this case City::uiMouseMotion is called each time
// when the mouse moves, and this is no good.
	if ( _eCurrentTool != OC_TOOL_NONE ) {
	// Get the current selected voxel.
		gVars.gpRenderer->GetSelectedWLFrom(
					iMouseX, iMouseY,
					_uiMapW, _uiMapL,
					gVars.gpMapMgr, _apLayer[ _eCurrentLayer ] );
		
	// The user is dragging
		if(_bLMBPressedOverMap) {
		// IF the user is dragging with the left mouse button THEN
			if ( mouseState & SDL_BUTTON(1) ) {
				_uiMapW2 = _uiMapW;
				_uiMapL2 = _uiMapL;
				
			// draw the map with the highlighted area
				gVars.gpRenderer->DisplayHighlight(
					gVars.gpMapMgr, _apLayer[ _eCurrentLayer ],
					_uiMapW1, _uiMapL1,
					_uiMapW2, _uiMapL2,
					_eCurrentTool );

				drawScene = false;
			}
		}
	}

// Display the screen as usual
	if (drawScene)
		gVars.gpRenderer->Display( gVars.gpMapMgr, _apLayer[ _eCurrentLayer ] );

// Display build preview
	_BuildPreview();

// Display the city's funds
	ossStatus.str("");
	ossStatus << _liCityFund;
	_plblFund->SetText( ossStatus.str() );

// Display the city population
	ossStatus.str("");
	ossStatus << _uiPopulation;
	_plblPopulation->SetText( ossStatus.str() );

// display the date
	ossStatus.str("");
	ossStatus << _uiDay << "/" << _uiMonth << "/" << _uiYear;
	_plblDate->SetText( ossStatus.str() );


// Display all the contained movements
//	gVars.gpMoveMgr->Move();		// called by Run()
	gVars.gpMoveMgr->Display();

// FIXME: buggy MAS environment
//	gVars.gpEnvironment->displayAgent();

// Display the status bar
	_pctrStatus->Display();

// Display the current container
	_pctr->Display();

// Display windows.
	((GUIContainer*)_pwStatistics->GetContainer())->Display();
	if(_pwQwery != NULL)
		((GUIContainer*)_pwQwery->GetContainer())->Display();

// Display the menu
	if (_pctrMenu != NULL)
		_pctrMenu->Display();

// Display mouse pointer
	_pctrToolMouse[_eCurrentTool]->SetLocation(iMouseX, _iWinHeight - iMouseY - SIZE_MOUSE_POINTER);
	_pctrToolMouse[_eCurrentTool]->Display();

// Swap the buffers and update the screen
	SDL_GL_SwapBuffers();
}


   /*=====================================================================*/
Layer*
City::GetLayer( OPENCITY_CITY_LAYER enumLayer ) const
{
	return _apLayer[ enumLayer ];
}


   /*=====================================================================*/
const void
City::GetWL(
	uint & w, uint & l ) const
{
	w = _uiWidth;
	l = _uiLength;
}


   /*=====================================================================*/
   /*                    BASE CLASS 'UI' IMPLEMENTATION                   */
   /*=====================================================================*/



   /*=====================================================================*/
void City::Keyboard( const SDL_KeyboardEvent& rcEvent )
{
//	OPENCITY_DEBUG( "Keydown event received" );

// SDL_KEYDOWN or SDL_PRESSED
	if (rcEvent.type == SDL_KEYDOWN) {
	// test if ALT is pressed
		if (rcEvent.keysym.mod & KMOD_ALT) {
			_abKeyPressed[KEY_ALT] = true;
		}

	// key symbols treatment
		switch (rcEvent.keysym.sym) {
		case SDLK_PAGEUP:
			_abKeyPressed[KEY_PAGEUP] = true;
			break;
		case SDLK_PAGEDOWN:
			_abKeyPressed[KEY_PAGEDOWN] = true;
			break;

		case SDLK_UP:
		case SDLK_w:
			_abKeyPressed[KEY_UP] = true;

			break;
		case SDLK_DOWN:
		case SDLK_s:
			_abKeyPressed[KEY_DOWN] = true;
			break;
		case SDLK_RIGHT:
		case SDLK_d:
			_abKeyPressed[KEY_RIGHT] = true;
			break;
		case SDLK_LEFT:
		case SDLK_a:
			_abKeyPressed[KEY_LEFT] = true;
			break;

	// Establish a connection to OCZen
		case SDLK_z:
			OPENCITY_NET_CODE netCode;
			netCode = gVars.gpNetworking->Open( gVars.gsZenServer );
			switch (netCode) {
				case OC_NET_CLIENT_CONNECTED:
					OPENCITY_INFO( "OpenCity is already connected to a server." );
					break;
				case OC_NET_CLIENT_ACCEPTED:
					OPENCITY_INFO( "The connection request has been accepted." );
					break;
				case OC_NET_CLIENT_REJECTED:
					OPENCITY_INFO( "The connection request has been rejected. Is the server full ?" );
					break;
				default:
					OPENCITY_INFO( "The connection to \"" << gVars.gsZenServer << "\" has failed." );
			}
			break;

		case SDLK_n:	// set the tool to "None"
			_SetCurrentTool(  OC_TOOL_NONE );
			break;
		case SDLK_r:	// set tool for "zone residential"
			_SetCurrentTool( OC_TOOL_ZONE_RES );
			break;
		case SDLK_c:	// set tool for "zone commercial"
			_SetCurrentTool( OC_TOOL_ZONE_COM );
			break;
		case SDLK_i:	// set tool for "zone industrial"
			_SetCurrentTool( OC_TOOL_ZONE_IND );
			break;

		case SDLK_p:	// set tool for "building road"
			_SetCurrentTool( OC_TOOL_ROAD );
			break;
		case SDLK_l:	// set tool for building electric lines
			_SetCurrentTool( OC_TOOL_ELINE );
			break;
		case SDLK_e:	// set tool for building electric plants
			_SetCurrentTool( OC_TOOL_EPLANT_NUCLEAR );
			break;

		case SDLK_u:	// height up
			_SetCurrentTool( OC_TOOL_HEIGHT_UP );
			break;
		case SDLK_q:	//query tool
			_SetCurrentTool( OC_TOOL_QUERY );
			break;
		case SDLK_x:	// destroy
			_SetCurrentTool( OC_TOOL_DESTROY );
			break;


		case SDLK_b:	// toggle structures on/off
			gVars.gpRenderer->ToggleStructure();
			break;
		case SDLK_f:	// toggle wireframe on/off
			gVars.gpRenderer->ToggleWireFrame();
			break;
		case SDLK_g:	// toggle grid on/off
			gVars.gpRenderer->ToggleGrid();
			break;
		case SDLK_k:	// toggle compass on/off
		// Do not mess up the menu
			if (_pctrMenu != NULL)
				break;

			_bStatusVisible = !_bStatusVisible;
			gVars.gpRenderer->ToggleCompass();
			if (_bStatusVisible)
				_pctrStatus->Set( OC_GUIMAIN_VISIBLE );
			else
				_pctrStatus->Unset( OC_GUIMAIN_VISIBLE );
			break;
		case SDLK_o:	// toggle projection mode
			gVars.gpRenderer->ToggleProjection();
			break;
		case SDLK_t:	// toggle terrain display
			gVars.gpRenderer->ToggleTerrain();
			break;

		case SDLK_INSERT: // zoom in
			_abKeyPressed[KEY_INSERT] = true;
			break;
		case SDLK_DELETE: // zoom out
			_abKeyPressed[KEY_DELETE] = true;
			break;


	// manipulating the music player
		case SDLK_GREATER:
			gVars.gpAudioMgr->PlayNextMusic();
			break;
		case SDLK_LESS:
			gVars.gpAudioMgr->PlayPreviousMusic();
			break;
		case SDLK_m:
			gVars.gpAudioMgr->ToggleMusic();
			break;


	// Save and load
		case SDLK_F2:
			_Save( ocSaveDirPrefix( "opencity.save" ) );
			break;
		case SDLK_F6:
			_Load( ocSaveDirPrefix( "opencity.save" ) );
			break;


		case SDLK_h:
			gVars.gpRenderer->Home();
			break;


		case SDLK_ESCAPE:	// Close toolcircle, quit tool, close static window, close query window, Open/close the main menu.
			if (_pctrMenu == NULL) {
				if (_pctr->IsSet( OC_GUIMAIN_VISIBLE )) { // Close toolcircle
					_pctr->Unset( OC_GUIMAIN_VISIBLE );
					_pctr = _pctrMain; // Back to main menu toolcircle.
				}
				else if (_eCurrentTool != OC_TOOL_NONE) // Close tool
					_SetCurrentTool( OC_TOOL_NONE );
				else if (_pwStatistics != NULL && ((GUIContainer*)_pwStatistics->GetContainer())->IsSet(OC_GUIMAIN_VISIBLE)) // Close static window
					((GUIContainer*)_pwStatistics->GetContainer())->Unset(OC_GUIMAIN_VISIBLE);
				else if (_pwQwery != NULL && ((GUIContainer*)_pwQwery->GetContainer())->IsSet(OC_GUIMAIN_VISIBLE)) // Close query window
				{
					delete _pwQwery;
					_pwQwery = NULL;
				}
				else // Open the main menu
					_LoadMenu();
			}
			else {
				_UnloadMenu(); // Close the main menu
			}
			break;

#ifndef NDEBUG
	// Testing PathFinder
		case SDLK_j:
			_pctr->ResetAttribute( OC_GUIMAIN_CLICKED | OC_GUIMAIN_MOUSEOVER );

		// Toggle Main <-> Pathfinding toolcircle
			if (_pctr == _pctrPath ) {
				_pctr = _pctrMain;
			}
			else {
				_pctr = _pctrPath;
			}
			if ( _pctr->IsSet( OC_GUIMAIN_VISIBLE ) == false ) {
				_pctr->Set( OC_GUIMAIN_VISIBLE );
			}
			break;

	// MAS test toolcircle
		case SDLK_v:
			_pctr->ResetAttribute( OC_GUIMAIN_CLICKED | OC_GUIMAIN_MOUSEOVER );

		// Toggle Main <-> MAS toolcircle
			if ( _pctr == _pctrMAS ) {
				_pctr = _pctrMain;
			}
			else {
				_pctr = _pctrMAS;
			}
			if ( _pctr->IsSet( OC_GUIMAIN_VISIBLE ) == false ) {
				_pctr->Set( OC_GUIMAIN_VISIBLE );
			}
			break;
#endif

		default:
			break;
		}
	}
// SDL_KEYUP or SDL_RELEASED
	else {
	// test if ALT is released
		if (!(rcEvent.keysym.mod & KMOD_ALT)) {
			_abKeyPressed[KEY_ALT] = false;
		}

	// other key symbols treatment
		switch (rcEvent.keysym.sym) {

		case SDLK_PAGEUP:
			_abKeyPressed[KEY_PAGEUP] = false;
			break;
		case SDLK_PAGEDOWN:
			_abKeyPressed[KEY_PAGEDOWN] = false;
			break;

		case SDLK_UP:
		case SDLK_w:
			_abKeyPressed[KEY_UP] = false;
			break;
		case SDLK_DOWN:
		case SDLK_s:
			_abKeyPressed[KEY_DOWN] = false;
			break;
		case SDLK_RIGHT:
		case SDLK_d:
			_abKeyPressed[KEY_RIGHT] = false;
			break;
		case SDLK_LEFT:
		case SDLK_a:
			_abKeyPressed[KEY_LEFT] = false;
			break;

		case SDLK_INSERT: // zoom in
			_abKeyPressed[KEY_INSERT] = false;
			break;
		case SDLK_DELETE: // zoom out
			_abKeyPressed[KEY_DELETE] = false;
			break;

		default:
			break;
		} // switch
	} // key released

}


   /*=====================================================================*/
void
City::MouseMotion( const SDL_MouseMotionEvent& rcEvent )
{
//	OPENCITY_DEBUG("Mouse moved");

	// We process the menu first
	if (_pctrMenu != NULL)
		_pctrMenu->MouseMotion( rcEvent );
	else
	{
		_pwStatistics->GetContainer()->MouseMotion( rcEvent );
		if(_pwQwery != NULL)
			_pwQwery->GetContainer()->MouseMotion( rcEvent );
		_pctr->MouseMotion( rcEvent );
		_pctrStatus->MouseMotion( rcEvent );
		
		// Right mouse button is pressed, left button isn't pressed and the mouse move more than 3 pixels.
		// Reset and active the move of the camera.
		if( _bRMBPressed && !_bLMBPressed && ( abs(_iXMove)>3 || abs(_iYMove)>3 ) )
		{
			_bMoveCamera = true;
			_iXMove = 0;
			_iYMove = 0;
		}

		// For the move or rotation of the camera.
		_iXMove += rcEvent.xrel;
		_iYMove += rcEvent.yrel;
	}
}


   /*=====================================================================*/
void
City::MouseButton( const SDL_MouseButtonEvent& rcsMBE )
{
//	OPENCITY_DEBUG("Mouse button event received" );

// IF the menu is opened THEN process the mouse click on the menu and return
	if (_pctrMenu != NULL ) {
		_pctrMenu->MouseButton( rcsMBE );
		if ( _pctrMenu->GetClick() != 0 ) {
			_HandleMenuClick();
			_bLMBPressedOverMap = false;
		}
		return;
	}

// Process the mouse click on the windows
	_pwStatistics->GetContainer()->MouseButton(rcsMBE);
	if ( ((GUIContainer*)_pwStatistics->GetContainer())->GetClick() != 0 ) {
		_HandleWinStatistics();
		_bLMBPressedOverMap = false;
		return;
	}
	if(_pwQwery != NULL) {
		_pwQwery->GetContainer()->MouseButton(rcsMBE);
		if ( ((GUIContainer*)_pwQwery->GetContainer())->GetClick() != 0 ) {
			_HandleWinQuery();
			_bLMBPressedOverMap = false;
			return;
		}
	}

// Process the mouse click on the status bar
	_pctrStatus->MouseButton( rcsMBE );
	if ( _pctrStatus->GetClick() != 0 ) {
		_HandleStatusClick();
		_bLMBPressedOverMap = false;
		return;
	}

// Process the click concerning the GUI
	_pctr->MouseButton( rcsMBE );
	if ( _pctr->GetClick() != 0 ) {
		_HandleGUIClick();
		_bLMBPressedOverMap = false;
		return;
	}

	// The user didn't click on a GUI object so we look for clicks on the map
	switch (rcsMBE.state) 
	{
		case SDL_PRESSED:
		{
			_bLMBPressedOverMap = false;
			if (rcsMBE.button == SDL_BUTTON_LEFT)
 			{
				_bLMBPressed = true;
				
				// If I'm in the toolcircle, I remove it.
				if (_pctr->IsSet( OC_GUIMAIN_VISIBLE ))
				{
					_pctr->Unset( OC_GUIMAIN_VISIBLE );
					_pctr = _pctrMain; // Back to main toolcircle.
				}
				// I'm not in the toolcircle.
				else
				{
					// If the right mouse is pressed, active rotate camera.
					if(_bRMBPressed)
					{
						_bRotateCamera = true;
						_bMoveCamera = false; // And desactive camera move.
					}
					
					// User did click in the map.
					if (!_pctrStatus->IsInside(rcsMBE.x, _iWinHeight - rcsMBE.y) && gVars.gpRenderer->GetSelectedWLFrom(rcsMBE.x, rcsMBE.y, _uiMapW1, _uiMapL1, gVars.gpMapMgr, _apLayer[ _eCurrentLayer ]))
						_bLMBPressedOverMap = true;
				}
			} //if
//debug begin
//SDL_GL_SwapBuffers(); // uncomment this if you want to know how it works
//SDL_Delay( 500 );
//debug end

			// RMB (right mouse button) close/open the toolcircle
			if (rcsMBE.button == SDL_BUTTON_RIGHT) 
			{
				// Hide toolcircle.
				_pctr->Unset( OC_GUIMAIN_VISIBLE );
				_pctr = _pctrMain; // Back to main toolcircle.
				_bRMBPressed = true;
				
				// Reset the move of mouse.
				_iXMove = 0;
				_iYMove = 0;
				
				// If the left mouse is pressed, active rotate camera.
				if(_bLMBPressed)
				{
					_bRotateCamera = true;
					_bMoveCamera = false; // And desactive camera move.
				}
			}

			// Wheel button pressed
			if (rcsMBE.button == SDL_BUTTON_MIDDLE) {

				// Rotate the camera
				_bRotateCamera = true;
				_iXMove = 0;
				_iYMove = 0;
			}

		// Wheel button forward
			if (rcsMBE.button == 4) {
			// move right if CTRL pressed
				if (SDL_GetModState() & KMOD_CTRL)
					gVars.gpRenderer->MoveRight();

			// move up if SHIFT pressed
				if (SDL_GetModState() & KMOD_SHIFT)
					gVars.gpRenderer->MoveUp();

			// zoom in if nothing pressed
				if (!(SDL_GetModState() & (KMOD_SHIFT | KMOD_CTRL)))
					gVars.gpRenderer->ZoomIn();
			}

		// Wheel button backward
			if (rcsMBE.button == 5) {
			// move right if CTRL pressed
				if (SDL_GetModState() & KMOD_CTRL)
					gVars.gpRenderer->MoveLeft();

			// move up if SHIFT pressed
				if (SDL_GetModState() & KMOD_SHIFT)
					gVars.gpRenderer->MoveDown();

			// zoom in if nothing pressed
				if (!(SDL_GetModState() & (KMOD_SHIFT | KMOD_CTRL)))
					gVars.gpRenderer->ZoomOut();
			}

		break;
		} //case SDL_PRESSED

	   //-------------------------------------------------------
		case SDL_RELEASED: {
			// Left Mouse Button.
			if(rcsMBE.button == SDL_BUTTON_LEFT)
			{
				_bLMBPressed = false;
				
				// IF Ctrl not pressed,
				// AND dragging enabled
				// AND mouse button was correctly released in the map
				// THEN do tool
				if (! (SDL_GetModState() & KMOD_CTRL)
					&& _bLMBPressedOverMap
					&& gVars.gpRenderer->GetSelectedWLFrom(
						rcsMBE.x, rcsMBE.y,
						_uiMapW2, _uiMapL2,
						gVars.gpMapMgr,
						_apLayer[ _eCurrentLayer ] ))
				{
//debug
//cout << "W2: " << _uiMapW2 << "/" << "H2: "
//     << _uiMapL2 << endl;
//test pathfinding
					_TestPathfinding();
					_DoTool( rcsMBE );
				} //if

				_bLMBPressedOverMap = false;
				
				// Stop the move with the mouse.
				_bRotateCamera = false;
			}
			// RMB (right mouse button).
			if (rcsMBE.button == SDL_BUTTON_RIGHT) 
			{
				// Show toolcircle if not ckicked on top the status bar and not move/rotate the camera.
				if( (!_pctrStatus->IsSet(OC_GUIMAIN_VISIBLE) || !_pctrStatus->IsInside(rcsMBE.x, _iWinHeight - rcsMBE.y)) && !_bMoveCamera && !_bRotateCamera )
				{
					_pctr->SetLocation( rcsMBE.x - 70, _iWinHeight - rcsMBE.y - 70 );
					_pctr->Set( OC_GUIMAIN_VISIBLE );
				}

				// Stop the move with the mouse.
				_bRotateCamera = false;
				_bMoveCamera = false;
				_bRMBPressed = false;
			}

		// WMB (wheel mouse button)
			if (rcsMBE.button == SDL_BUTTON_MIDDLE) {
				// Stop the rotate
				_bRotateCamera = false;
			}

			break;

		} //case SDL_RELEASED

	} // switch
}


   /*=====================================================================*/
void
City::Expose( const SDL_ExposeEvent& rcEvent )
{
	OPENCITY_DEBUG( "Expose event received" );

	gVars.gpRenderer->Display( gVars.gpMapMgr, _apLayer[ _eCurrentLayer ] );
	_pctr->Expose( rcEvent );
	_pctrStatus->Expose( rcEvent );
	if (_pctrMenu != NULL) {
		_pctrMenu->Expose( rcEvent );
	}

	SDL_GL_SwapBuffers();
}


   /*=====================================================================*/
void City::Resize( const SDL_ResizeEvent& rcEvent )
{
	OPENCITY_DEBUG( "Resize event received" );

// Set the new window's size
	_iWinWidth = rcEvent.w;
	_iWinHeight = rcEvent.h;
	gVars.gpRenderer->SetWinSize( _iWinWidth, _iWinHeight );

// Resize the main status bar and reposition it
	_pctrStatus->Resize( rcEvent );
	_pctrStatus->SetLocation( (_iWinWidth-512)/2, 0 );

// Tell the containers about the event
	_pctrMain->Resize( rcEvent );
	_pctrL->Resize( rcEvent );
	_pctrT->Resize( rcEvent );
	_pctrZ->Resize( rcEvent );
	_pctrG->Resize( rcEvent );
	_pctrN->Resize( rcEvent );
	_pctrS->Resize( rcEvent );
	_pctrPath->Resize( rcEvent );
	_pctrMAS->Resize( rcEvent );

	if (_pctrMenu != NULL) {
		_pctrMenu->Resize( rcEvent );
		_pctrMenu->SetSize( _iWinWidth, _iWinHeight );
		_CenterMenu();
	}
}














   /*=====================================================================*/
   /*                        PRIVATE     METHODS                          */
   /*=====================================================================*/






void City::_CreateTree()
{
// Create a new tree density map
	int* treeDensity = gVars.gpMapMaker->getTreeDensity();

// Build the trees according to the density map
	uint cost = 0;
	for (uint l = 0, linear = 0; l < _uiLength; l++)
	for (uint w = 0; w < _uiWidth; w++, linear++) {
		if (treeDensity[linear] > 0) {
			_apLayer[ OC_LAYER_BUILDING ]->BuildStructure( w, l, w, l, OC_STRUCTURE_FLORA, cost, _liCityFund );
		}
	}

	delete [] treeDensity;
}


   /*=====================================================================*/
void
City::_CreateGUI()
{
	ostringstream ossTemp;

// Load the mouse pointer for tools
	_pctrToolMouse[OC_TOOL_NONE] 				= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/normal.png" ));
	_pctrToolMouse[OC_TOOL_DESTROY] 			= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/destroy.png" ));
	_pctrToolMouse[OC_TOOL_ZONE_RES]			= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/residential.png" ));
	_pctrToolMouse[OC_TOOL_ZONE_COM]			= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/commercial.png" ));
	_pctrToolMouse[OC_TOOL_ZONE_IND]			= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/industrial.png" ));
	_pctrToolMouse[OC_TOOL_HEIGHT_UP]			= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/raise.png" ));
	_pctrToolMouse[OC_TOOL_HEIGHT_DOWN]			= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/lower.png" ));
	_pctrToolMouse[OC_TOOL_ROAD]				= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/road.png" ));
	_pctrToolMouse[OC_TOOL_ELINE]				= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/power_line.png" ));
	_pctrToolMouse[OC_TOOL_EPLANT_COAL]			= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/power_plant_coal.png" ));
	_pctrToolMouse[OC_TOOL_EPLANT_NUCLEAR]		= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/power_plant_nuclear.png" ));
	_pctrToolMouse[OC_TOOL_PARK]				= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/park.png" ));
	_pctrToolMouse[OC_TOOL_FLORA]				= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/tree.png" ));
	_pctrToolMouse[OC_TOOL_FIRE]				= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/fire.png" ));

	_pctrToolMouse[OC_TOOL_POLICE]				= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/police.png" ));
	_pctrToolMouse[OC_TOOL_HOSPITAL]			= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/hospital.png" ));
	_pctrToolMouse[OC_TOOL_EDUCATION]			= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/education.png" ));
	_pctrToolMouse[OC_TOOL_QUERY]				= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/query.png" ));

	_pctrToolMouse[OC_TOOL_AGENT_POLICE]		= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/normal.png" ));
	_pctrToolMouse[OC_TOOL_AGENT_DEMONSTRATOR]	= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/normal.png" ));
	_pctrToolMouse[OC_TOOL_AGENT_ROBBER]		= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/normal.png" ));
	_pctrToolMouse[OC_TOOL_TEST_BUILDING]		= new GUIContainer( 0, 0, SIZE_MOUSE_POINTER, SIZE_MOUSE_POINTER, ocDataDirPrefix( "graphism/gui/mouse/normal.png" ));

// Show normal mouse pointer
	for(int i=0; i<OC_TOOL_NUMBER; i++)
		_pctrToolMouse[i]->Unset( OC_GUIMAIN_VISIBLE );
	_pctrToolMouse[OC_TOOL_NONE]->Set( OC_GUIMAIN_VISIBLE );

// The button query in status bar
	_btnQuery = new GUIButton( GUIBUTTON_POSITION_QUERY, ocDataDirPrefix( "graphism/gui/status/query" ));
	_btnQuery->Set( OC_GUIMAIN_VISIBLE );

// The status bar
	_pbtnPause = new GUIButton( GUIBUTTON_POSITION_PAUSE, ocDataDirPrefix( "graphism/gui/status/speed_pause" ), true, true);
	_pbtnPlay  = new GUIButton( GUIBUTTON_POSITION_PLAY, ocDataDirPrefix( "graphism/gui/status/speed_play" ), true, true);
	_pbtnPlay->SetActive(true);
	_pbtnFast  = new GUIButton( GUIBUTTON_POSITION_FAST, ocDataDirPrefix( "graphism/gui/status/speed_fast" ), true, true);

	ossTemp << _liCityFund;
	_plblFund = new GUILabel( 125, 11, 80, 10, ossTemp.str() );
	_plblFund->SetAlign( GUILabel::OC_ALIGN_RIGHT );
	_plblFund->SetForeground( OPENCITY_PALETTE[Color::OC_WHITE] );

	ossTemp.str("");
	ossTemp << _uiPopulation;
	_plblPopulation = new GUILabel( 240, 11, 80, 10, ossTemp.str() );
	_plblPopulation->SetAlign( GUILabel::OC_ALIGN_RIGHT );
	_plblPopulation->SetForeground( OPENCITY_PALETTE[Color::OC_WHITE] );

	ossTemp.str("");
	ossTemp << _uiDay << "/" << _uiMonth << "/" << _uiYear;
	_plblDate = new GUILabel( 348, 11, 80, 10, ossTemp.str() );
	_plblDate->SetAlign( GUILabel::OC_ALIGN_CENTER );
	_plblDate->SetForeground( OPENCITY_PALETTE[Color::OC_WHITE] );

	_pbarResidence = new GUIBar( 5, 5, 7, 53 );
	_pbarResidence->SetForeground( OPENCITY_PALETTE[Color::OC_GREEN] );

	_pbarCommerce = new GUIBar( 18, 5, 7, 53 );
	_pbarCommerce->SetForeground( OPENCITY_PALETTE[Color::OC_BLUE] );

	_pbarIndustry = new GUIBar( 29, 5, 7, 53 );
	_pbarIndustry->SetForeground( OPENCITY_PALETTE[Color::OC_YELLOW] );

	_pbarPower = new GUIBar( 42, 5, 7, 53 );
	_pbarPower->SetForeground( OPENCITY_PALETTE[Color::OC_PINK] );

	_pctrStatus = new GUIContainer( (_iWinWidth-512)/2, 0, 512, 64, ocDataDirPrefix( "graphism/gui/main_status_bar.png" ) );
	_pctrStatus->Add( _pbtnPause );
	_pctrStatus->Add( _pbtnPlay );
	_pctrStatus->Add( _pbtnFast );
	_pctrStatus->Add( _btnQuery );
	_pctrStatus->Add( _plblFund );
	_pctrStatus->Add( _plblPopulation );
	_pctrStatus->Add( _plblDate );
	_pctrStatus->Add( _pbarResidence );
	_pctrStatus->Add( _pbarCommerce );
	_pctrStatus->Add( _pbarIndustry );
	_pctrStatus->Add( _pbarPower );
	_pctrStatus->Set( OC_GUIMAIN_VISIBLE );

// Windows.
	_pwStatistics = WindowFactory::createStatsWindow();
	((GUIContainer*)_pwStatistics->GetContainer())->Unset(OC_GUIMAIN_VISIBLE);
	_pwQwery = NULL;

// GUI main toolcircle
	pbtnZ = new GUIButton( GUIBUTTON_POSITION_1, ocDataDirPrefix( "graphism/gui/zone" ));
	pbtnS = new GUIButton( GUIBUTTON_POSITION_5, ocDataDirPrefix( "graphism/gui/save" ));
	pbtnL = new GUIButton( GUIBUTTON_POSITION_2, ocDataDirPrefix( "graphism/gui/power" ));
	pbtnP = new GUIButton( GUIBUTTON_POSITION_3, ocDataDirPrefix( "graphism/gui/road" ));
	pbtnX = new GUIButton( GUIBUTTON_POSITION_4, ocDataDirPrefix( "graphism/gui/bulldozer" ));
	pbtnG = new GUIButton( GUIBUTTON_POSITION_6, ocDataDirPrefix( "graphism/gui/government" ));

	_pctrMain = new GUIContainer( 100, 100, RADIO_TOOLCIRCLE*2, RADIO_TOOLCIRCLE*2, ocDataDirPrefix( "graphism/gui/toolcircle_bg.png" ) );
	_pctrMain->Add( pbtnZ );
	_pctrMain->Add( pbtnS );
	_pctrMain->Add( pbtnL );
	_pctrMain->Add( pbtnP );
	_pctrMain->Add( pbtnX );
	_pctrMain->Add( pbtnG );


// GUI Z toolcircle for RCI buttons
	pbtnZB = new GUIButton( GUIBUTTON_POSITION_1, ocDataDirPrefix( "graphism/gui/back" ));
	pbtnZR = new GUIButton( GUIBUTTON_POSITION_2, ocDataDirPrefix( "graphism/gui/residential" ));
	pbtnZC = new GUIButton( GUIBUTTON_POSITION_3, ocDataDirPrefix( "graphism/gui/commercial" ));
	pbtnZI = new GUIButton( GUIBUTTON_POSITION_4, ocDataDirPrefix( "graphism/gui/industrial" ));

	_pctrZ = new GUIContainer( 100, 100, RADIO_TOOLCIRCLE*2, RADIO_TOOLCIRCLE*2, ocDataDirPrefix( "graphism/gui/toolcircle_bg.png" ) );
	_pctrZ->Add( pbtnZB );
	_pctrZ->Add( pbtnZR );
	_pctrZ->Add( pbtnZC );
	_pctrZ->Add( pbtnZI );


// GUI L toolcircle ( electric lines, electric plants )
	pbtnLB = new GUIButton( GUIBUTTON_POSITION_2, ocDataDirPrefix( "graphism/gui/back" ));
	pbtnLL = new GUIButton( GUIBUTTON_POSITION_3, ocDataDirPrefix( "graphism/gui/power_line" ));
	pbtnLN = new GUIButton( GUIBUTTON_POSITION_4, ocDataDirPrefix( "graphism/gui/power_plant_nuclear" ));
	pbtnLC = new GUIButton( GUIBUTTON_POSITION_5, ocDataDirPrefix( "graphism/gui/power_plant_coal" ));

	_pctrL = new GUIContainer( 100, 100, RADIO_TOOLCIRCLE*2, RADIO_TOOLCIRCLE*2, ocDataDirPrefix( "graphism/gui/toolcircle_bg.png" ) );
	_pctrL->Add( pbtnLB );
	_pctrL->Add( pbtnLL );
	_pctrL->Add( pbtnLN );
	_pctrL->Add( pbtnLC );


// GUI T toolcircle ( raise, lower terrain )
	pbtnTU = new GUIButton( GUIBUTTON_POSITION_2, ocDataDirPrefix( "graphism/gui/raise" ));
	pbtnTD = new GUIButton( GUIBUTTON_POSITION_3, ocDataDirPrefix( "graphism/gui/lower" ));
	pbtnTB = new GUIButton( GUIBUTTON_POSITION_4, ocDataDirPrefix( "graphism/gui/back" ));
	pbtnTX = new GUIButton( GUIBUTTON_POSITION_1, ocDataDirPrefix( "graphism/gui/destroy" ));
	pbtnTQ = new GUIButton( GUIBUTTON_POSITION_5, ocDataDirPrefix( "graphism/gui/query" ));

	_pctrT = new GUIContainer( 100, 100, RADIO_TOOLCIRCLE*2, RADIO_TOOLCIRCLE*2, ocDataDirPrefix( "graphism/gui/toolcircle_bg.png" ) );
	_pctrT->Add( pbtnTB );
	_pctrT->Add( pbtnTU );
	_pctrT->Add( pbtnTD );
	_pctrT->Add( pbtnTX );
	_pctrT->Add( pbtnTQ );


// GUI Gouvernement toolcircle ( park, education, hospital, police and fire )
	pbtnGB = new GUIButton( GUIBUTTON_POSITION_6, ocDataDirPrefix( "graphism/gui/back" ));
	pbtnGP = new GUIButton( GUIBUTTON_POSITION_1, ocDataDirPrefix( "graphism/gui/park" ));
	pbtnGE = new GUIButton( GUIBUTTON_POSITION_5, ocDataDirPrefix( "graphism/gui/education" ));
	pbtnGH = new GUIButton( GUIBUTTON_POSITION_2, ocDataDirPrefix( "graphism/gui/hospital" ));
	pbtnGL = new GUIButton( GUIBUTTON_POSITION_3, ocDataDirPrefix( "graphism/gui/police" ));
	pbtnGF = new GUIButton( GUIBUTTON_POSITION_4, ocDataDirPrefix( "graphism/gui/fire" ));

	_pctrG = new GUIContainer( 100, 100, RADIO_TOOLCIRCLE*2, RADIO_TOOLCIRCLE*2, ocDataDirPrefix( "graphism/gui/toolcircle_bg.png" ) );
	_pctrG->Add( pbtnGB );
	_pctrG->Add( pbtnGP );
	_pctrG->Add( pbtnGE );
	_pctrG->Add( pbtnGH );
	_pctrG->Add( pbtnGL );
	_pctrG->Add( pbtnGF );


// Create the nature container
	pbtnNB = new GUIButton( GUIBUTTON_POSITION_1, ocDataDirPrefix( "graphism/gui/back" ));
	pbtnNP = new GUIButton( GUIBUTTON_POSITION_6, ocDataDirPrefix( "graphism/gui/park_city" ));
	pbtnNT = new GUIButton( GUIBUTTON_POSITION_5, ocDataDirPrefix( "graphism/gui/tree" ));

	_pctrN = new GUIContainer( 100, 100, RADIO_TOOLCIRCLE*2, RADIO_TOOLCIRCLE*2, ocDataDirPrefix( "graphism/gui/toolcircle_bg.png" ) );
	_pctrN->Add( pbtnNB );
	_pctrN->Add( pbtnNP );
	_pctrN->Add( pbtnNT );


// Create save/load buttons and the container
	pbtnSL = new GUIButton( GUIBUTTON_POSITION_1, ocDataDirPrefix( "graphism/gui/save_load" ));
	pbtnSS = new GUIButton( GUIBUTTON_POSITION_6, ocDataDirPrefix( "graphism/gui/save_save" ));
	pbtnSB = new GUIButton( GUIBUTTON_POSITION_5, ocDataDirPrefix( "graphism/gui/back" ));

	_pctrS = new GUIContainer( 100, 100, RADIO_TOOLCIRCLE*2, RADIO_TOOLCIRCLE*2, ocDataDirPrefix( "graphism/gui/toolcircle_bg.png" ) );
	_pctrS->Add( pbtnSB );
	_pctrS->Add( pbtnSS );
	_pctrS->Add( pbtnSL );


// MAS toolcircle
	_pctrMAS = new GUIContainer( 100, 100, RADIO_TOOLCIRCLE*2, RADIO_TOOLCIRCLE*2 );
	pbtnMASPolice = new GUIButton( 20,  20,  30, 30, ocDataDirPrefix( "graphism/gui/police" ));
	pbtnMASDemonstrator = new GUIButton( 60,  0,   30, 30, ocDataDirPrefix( "graphism/gui/demonstrator" ));
	pbtnMASRobber = new GUIButton( 100, 20,  30, 30, ocDataDirPrefix( "graphism/gui/robber" ));
	_pctrMAS->Add( pbtnMASPolice );
	_pctrMAS->Add( pbtnMASDemonstrator );
	_pctrMAS->Add( pbtnMASRobber );

	pbtnMASRobber->Unset( OC_GUIMAIN_VISIBLE );

// the current _pctr points to the MAIN one
	_pctr = _pctrMain;
}


   /*=====================================================================*/
void
City::_DeleteGUI()
{
// MAS toolcircle
	delete _pctrMAS;
	delete pbtnMASRobber;
	delete pbtnMASDemonstrator;
	delete pbtnMASPolice;

// Load/save toolcircle
	delete _pctrS;
	delete pbtnSB;
	delete pbtnSS;
	delete pbtnSL;

// Nature toolcircle
	delete _pctrN;
	delete pbtnNB;
	delete pbtnNP;
	delete pbtnNT;

// GUI G toolcircle ( park, education, hospital, police and fire )
	delete _pctrG;
	delete pbtnGB;
	delete pbtnGP;
	delete pbtnGE;
	delete pbtnGH;
	delete pbtnGL;
	delete pbtnGF;

// GUI T toolcircle ( raise, lower terrain )
	delete _pctrT;
	delete pbtnTB;
	delete pbtnTU;
	delete pbtnTD;
	delete pbtnTX;
	delete pbtnTQ;

// GUI L toolcircle ( electric lines, electric plants )
	delete _pctrL;
	delete pbtnLB;
	delete pbtnLL;
	delete pbtnLN;
	delete pbtnLC;

// GUI Z toolcircle
	delete _pctrZ;
	delete pbtnZB;
	delete pbtnZR;
	delete pbtnZC;
	delete pbtnZI;

// GUI main toolcircle
	delete _pctrMain;
	delete pbtnZ;
	delete pbtnS;
	delete pbtnL;
	delete pbtnP;
	delete pbtnX;
	delete pbtnG;

// Delete the windows.
	delete _pwStatistics;
	if(_pwQwery != NULL)
		delete _pwQwery;

// Delete the status bar
	delete _pctrStatus;
	delete _pbarPower;
	delete _pbarIndustry;
	delete _pbarCommerce;
	delete _pbarResidence;
	delete _plblFund;
	delete _plblPopulation;
	delete _plblDate;
	delete _pbtnPlay;
	delete _pbtnFast;
	delete _pbtnPause;

// The query button
	delete _btnQuery;
	_btnQuery = NULL;

// Tools mouse pointer
	for(int i=0; i<OC_TOOL_NUMBER; i++) {
		delete _pctrToolMouse[i];
		_pctrToolMouse[i] = NULL;
	}
}


   /*=====================================================================*/
void
City::_LoadMenu()
{
	_pbtnMenuNew  = new GUIButton( 0, 0, 128, 128, ocDataDirPrefix("graphism/gui/main_menu_new") );
	_pbtnMenuLoad = new GUIButton( 0, 0, 128, 128, ocDataDirPrefix("graphism/gui/main_menu_load") );
	_pbtnMenuSave = new GUIButton( 0, 0, 128, 128, ocDataDirPrefix("graphism/gui/main_menu_save") );
	_pbtnMenuQuit = new GUIButton( 0, 0, 128, 128, ocDataDirPrefix("graphism/gui/main_menu_quit") );

	_pctrMenu = new GUIContainer(
		0, 0, _iWinWidth, _iWinHeight, ocDataDirPrefix("graphism/gui/main_menu_bg.png")
	);
	_pctrMenu->Add( _pbtnMenuNew );
	_pctrMenu->Add( _pbtnMenuLoad );
	_pctrMenu->Add( _pbtnMenuSave );
	_pctrMenu->Add( _pbtnMenuQuit );
	_pctrMenu->Set( OC_GUIMAIN_VISIBLE );

// Hide the status bar and the compass
	if (_bStatusVisible) {
		gVars.gpRenderer->ToggleCompass();
		_pctrStatus->Unset( OC_GUIMAIN_VISIBLE );
	}

	_CenterMenu();
}


   /*=====================================================================*/
void
City::_CenterMenu()
{
	assert( _pctrMenu != NULL );

// Center the menu
	int x = _iWinWidth/2;
	int y = _iWinHeight/2;
	_pbtnMenuNew->SetLocation( x-264, y );
	_pbtnMenuLoad->SetLocation( x-64, y );
	_pbtnMenuSave->SetLocation( x+136, y );
	_pbtnMenuQuit->SetLocation( _iWinWidth-150, 22 );

// Push the mouse motion event to activate the over state if necessary
	SDL_Event event;
	int mouseX, mouseY;
	event.type = SDL_MOUSEMOTION;
	event.motion.type = SDL_MOUSEMOTION;
	event.motion.state = SDL_GetMouseState(&mouseX, &mouseY);
	event.motion.x = mouseX;
	event.motion.y = mouseY;
	event.motion.xrel = 0;
	event.motion.yrel = 0;
	SDL_PushEvent(&event);
}


   /*=====================================================================*/
void
City::_UnloadMenu()
{
	delete _pctrMenu;
	delete _pbtnMenuQuit;
	delete _pbtnMenuSave;
	delete _pbtnMenuLoad;
	delete _pbtnMenuNew;

	_pctrMenu = NULL;

// IF the status bar was visible THEN display it now
	if (_bStatusVisible) {
		gVars.gpRenderer->ToggleCompass();
		_pctrStatus->Set( OC_GUIMAIN_VISIBLE );
	}
}


   /*=====================================================================*/
void
City::_SetCurrentTool( const OPENCITY_TOOL_CODE& tool )
{
	_pctrToolMouse[_eCurrentTool]->Unset( OC_GUIMAIN_VISIBLE );
	_eCurrentTool = tool;
	_pctrToolMouse[_eCurrentTool]->Set( OC_GUIMAIN_VISIBLE );
}


   /*=====================================================================*/
void
City::_DoTool(
	const SDL_MouseButtonEvent & sdlMBEvent )
{
	if ( _eCurrentTool == OC_TOOL_NONE )
		return;

	static uint cost;
	static OPENCITY_ERR_CODE enumErrCode;
	static int w1, h1, w2, h2;

	Structure* pstruct = NULL;		// for MAS

	cost = 0;
	w1 = _uiMapW1;
	h1 = _uiMapL1;
	w2 = _uiMapW2;
	h2 = _uiMapL2;
	OPENCITY_SWAP( w1, w2, int );
	OPENCITY_SWAP( h1, h2, int );




	switch (_eCurrentTool) {
	case OC_TOOL_ZONE_RES:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_RES, cost, _liCityFund )) == OC_ERR_FREE) {
			gVars.gpAudioMgr->PlaySound( OC_SOUND_RCI );
		}
		break;

	case OC_TOOL_ZONE_COM:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_COM, cost, _liCityFund )) == OC_ERR_FREE) {
			gVars.gpAudioMgr->PlaySound( OC_SOUND_RCI );
		}
		break;

	case OC_TOOL_ZONE_IND:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_IND, cost, _liCityFund )) == OC_ERR_FREE) {
			gVars.gpAudioMgr->PlaySound( OC_SOUND_RCI );
		}
		break;

	case OC_TOOL_ROAD:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_ROAD, cost, _liCityFund )) == OC_ERR_FREE) {
			gVars.gpAudioMgr->PlaySound( OC_SOUND_ROAD );
		}
		break;

	case OC_TOOL_ELINE:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_ELINE, cost, _liCityFund )) == OC_ERR_FREE) {
			gVars.gpAudioMgr->PlaySound( OC_SOUND_ELINE );
		}
		break;

	case OC_TOOL_EPLANT_COAL:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_EPLANT_COAL, cost, _liCityFund )) == OC_ERR_FREE) {
			_pMSim->AddStructure( _uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2, Simulator::OC_ELECTRIC );
			gVars.gpAudioMgr->PlaySound( OC_SOUND_EPLANT );
		}
		break;

	case OC_TOOL_EPLANT_NUCLEAR:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_EPLANT_NUCLEAR, cost, _liCityFund )) == OC_ERR_FREE) {
			_pMSim->AddStructure( _uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2, Simulator::OC_ELECTRIC );
			gVars.gpAudioMgr->PlaySound( OC_SOUND_EPLANT );
		}
		break;

	case OC_TOOL_PARK:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_PARK, cost, _liCityFund )) == OC_ERR_FREE) {
			gVars.gpAudioMgr->PlaySound( OC_SOUND_PARK );
		}
		break;

	case OC_TOOL_FLORA:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_FLORA, cost, _liCityFund )) == OC_ERR_FREE) {
			gVars.gpAudioMgr->PlaySound( OC_SOUND_PARK );
		}
		break;

	case OC_TOOL_FIRE:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_FIREDEPT, cost, _liCityFund )) == OC_ERR_FREE) {
// not used			_pMSim->AddStructure( _uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2, MainSim::OC_MICROSIM_ELE );
			gVars.gpAudioMgr->PlaySound( OC_SOUND_EPLANT );
		}
		break;

	case OC_TOOL_POLICE:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_POLICEDEPT, cost, _liCityFund )) == OC_ERR_FREE) {
// not used			_pMSim->AddStructure( _uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2, MainSim::OC_MICROSIM_ELE );
			gVars.gpAudioMgr->PlaySound( OC_SOUND_EPLANT );
		}
		break;

	case OC_TOOL_HOSPITAL:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_HOSPITALDEPT, cost, _liCityFund )) == OC_ERR_FREE) {
// not used			_pMSim->AddStructure( _uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2, MainSim::OC_MICROSIM_ELE );
			gVars.gpAudioMgr->PlaySound( OC_SOUND_EPLANT );
		}

		break;

	case OC_TOOL_EDUCATION:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_EDUCATIONDEPT, cost, _liCityFund )) == OC_ERR_FREE) {
// not used			_pMSim->AddStructure( _uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2, MainSim::OC_MICROSIM_ELE );
			gVars.gpAudioMgr->PlaySound( OC_SOUND_EPLANT );
		}
		break;

	case OC_TOOL_TEST_BUILDING:
		if ((enumErrCode = _apLayer[ _eCurrentLayer ]->
			BuildStructure(
				_uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2,
				OC_STRUCTURE_TEST, cost, _liCityFund )) == OC_ERR_FREE) {
			gVars.gpAudioMgr->PlaySound( OC_SOUND_EPLANT );
		}
		break;

	case OC_TOOL_AGENT_POLICE:
		assert( gVars.gpKernel != NULL );
		assert( gVars.gpEnvironment != NULL );
		pstruct = _apLayer[ OC_LAYER_BUILDING ]->GetStructure( _uiMapW1, _uiMapL1 );
		if ((pstruct != NULL) && (pstruct->GetCode() == OC_STRUCTURE_ROAD))
		new AgentPolice(*gVars.gpKernel, *gVars.gpEnvironment, _uiMapW1, _uiMapL1);
		break;

	case OC_TOOL_AGENT_DEMONSTRATOR:
		assert( gVars.gpKernel != NULL );
		assert( gVars.gpEnvironment != NULL );
		pstruct = _apLayer[ OC_LAYER_BUILDING ]->GetStructure( _uiMapW1, _uiMapL1 );
		if ((pstruct != NULL) && (pstruct->GetCode() == OC_STRUCTURE_ROAD))
		new AgentDemonstrator(*gVars.gpKernel, *gVars.gpEnvironment, _uiMapW1, _uiMapL1);
		break;

	case OC_TOOL_AGENT_ROBBER:
		assert( gVars.gpKernel != NULL );
		assert( gVars.gpEnvironment != NULL );
		pstruct = _apLayer[ OC_LAYER_BUILDING ]->GetStructure( _uiMapW1, _uiMapL1 );
		if ((pstruct != NULL) && (pstruct->GetCode() == OC_STRUCTURE_ROAD))
		new AgentRobber(*gVars.gpKernel, *gVars.gpEnvironment, _uiMapW1, _uiMapL1);
		break;

//FIXME: cost
	case OC_TOOL_HEIGHT_UP:
		if(_liCityFund < 5) {
			cost = 0;
			break;
		}
		enumErrCode = gVars.gpMapMgr->ChangeHeight( _uiMapW1, _uiMapL1, OC_MAP_UP );
		if ( enumErrCode == OC_ERR_FREE ) {
			gVars.gpRenderer->bHeightChange = true;
			gVars.gpAudioMgr->PlaySound( OC_SOUND_TERRAIN );
			cost = 5;		// Quick hack
		}
		break;

	case OC_TOOL_HEIGHT_DOWN:
		if(_liCityFund < 5) {
			cost = 0;
			break;
		}
		enumErrCode = gVars.gpMapMgr->ChangeHeight( _uiMapW1, _uiMapL1, OC_MAP_DOWN );
		if ( enumErrCode == OC_ERR_FREE ) {
			gVars.gpRenderer->bHeightChange = true;
			gVars.gpAudioMgr->PlaySound( OC_SOUND_TERRAIN );
			cost = 5;		// Quick hack
		}
		break;

	case OC_TOOL_QUERY:
	// Remove old query window
		if( _pwQwery != NULL ) {
			delete _pwQwery;
			_pwQwery = NULL;
		}

	// Get the new query window
		_pwQwery = _apLayer[ _eCurrentLayer ]->QueryStructure( _uiMapW1, _uiMapL1 );

	// Set new location of the window
		uint widthWin, heightWin;
		_pwQwery->GetSize(widthWin, heightWin);
		_pwQwery->SetLocation( sdlMBEvent.x + widthWin/2, _iWinHeight - sdlMBEvent.y + widthWin/2);

		enumErrCode = OC_ERR_SOMETHING;		// avoid to calculate the cost
		break;

	case OC_TOOL_DESTROY:
	// If it is a part of a bigger structure (an EPLANT for example),
	// the whole structure will be then destroyed
	// The following part tell the simulators to remove the collected data concerning
	// the structures which are going to be destroyed
		_pMSim->RemoveStructure( _uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2 );
		enumErrCode = _apLayer[ _eCurrentLayer ]->
			DestroyStructure( _uiMapW1, _uiMapL1, _uiMapW2, _uiMapL2, cost, _liCityFund );
		if (enumErrCode == OC_ERR_FREE) {
			gVars.gpAudioMgr->PlaySound( OC_SOUND_DESTROY );
		}
		break;

	default:
		enumErrCode = OC_ERR_SOMETHING;// which tool is this ?
	} // switch

	if (enumErrCode == OC_ERR_FREE) {
		_liCityFund -= cost;
	}
}


   /*=====================================================================*/
bool
City::_HandleKeyPressed()
{
	int actionFactor = 1;
	int key;
	bool boolKeyDown = false;		// There is no key pressed

// Key multiplier
	if (_abKeyPressed[KEY_ALT] == true)
		actionFactor = OC_ACTION_FACTOR;
	else
		actionFactor = 1;

// Look for pressed keys
	for (key = KEY_UP; key < KEY_NUMBER; key++) {
		if (_abKeyPressed[key] == true) {
			switch (key) {
			case KEY_UP:
				gVars.gpRenderer->MoveDown(actionFactor);
				break;
			case KEY_DOWN:
				gVars.gpRenderer->MoveUp(actionFactor);
				break;
			case KEY_RIGHT:
				gVars.gpRenderer->MoveLeft(actionFactor);
				break;
			case KEY_LEFT:
				gVars.gpRenderer->MoveRight(actionFactor);
				break;

			case KEY_PAGEUP:
				gVars.gpRenderer->RotateLeft(actionFactor);
				break;
			case KEY_PAGEDOWN:
				gVars.gpRenderer->RotateRight(actionFactor);
				break;

			case KEY_INSERT: // zoom in
				gVars.gpRenderer->ZoomIn();
				break;
			case KEY_DELETE: // zoom out
				gVars.gpRenderer->ZoomOut();
				break;
			} // switch

		// There's at least one key pressed
			boolKeyDown = true;
		} // if
	} // for

// Tell the caller that there's whether at least a key pressed or not
	return boolKeyDown;
}


   /*=====================================================================*/

// Process move and rotation mouse.
 void
City::_ProcessMoveRotationMouse()
{
// Move camera.
	if( _bMoveCamera ){
		gVars.gpRenderer->MoveLeft( ((float)_iXMove)*0.15f );
		gVars.gpRenderer->MoveUp( ((float)_iYMove)*0.15f );
		_iXMove = 0;
		_iYMove = 0;
	}
// Rotate the camera.
	else if( _bRotateCamera ){
		gVars.gpRenderer->RotateLeft( ((float)_iXMove)*0.1f );
	// TODO: rotate the camera up and down
		_iXMove = 0;
		_iYMove = 0;
	}
}

   /*=====================================================================*/
void
City::_RecordRessource()
{
// Accumulate the income each month
	uint r, c, i;
	uint w = 0, e = 0, g = 0;
	r = _pMSim->GetValue(Simulator::OC_RESIDENTIAL);
	c = _pMSim->GetValue(Simulator::OC_COMMERCIAL);
	i = _pMSim->GetValue(Simulator::OC_INDUSTRIAL);

// Create a new record
	Ressource res;
	res.fund = _liCityFund;
	res.population = _uiPopulation;
	res.r = r;
	res.c = c;
	res.i = i;
	res.w = w;
	res.e = e;
	res.g = g;

// Pop the oldest record if available (safe)
	if (!_dqRessource.empty())
		_dqRessource.pop_front();
	_dqRessource.push_back(res);
}


   /*=====================================================================*/
void
City::_DoBill( )
{
	uint maintenance;
	uint index, surface;
	Structure* pStruct;
	uint r, c, i;

	surface = _uiWidth * _uiLength;
	maintenance = 0;
	for (index = 0; index < surface; index++) {
		pStruct = _apLayer[ OC_LAYER_BUILDING ]->GetLinearStructure( index );
		if (pStruct != NULL)
			maintenance +=
				gVars.gpPropertyMgr->Get(OC_MAINTENANCE_COST, pStruct->GetCode());
	}

	_liCityFund -= maintenance;

// Accumulate the income each month
	r = _pMSim->GetValue(Simulator::OC_RESIDENTIAL);
	c = _pMSim->GetValue(Simulator::OC_COMMERCIAL);
	i = _pMSim->GetValue(Simulator::OC_INDUSTRIAL);

	_uiIncome += (r*OC_R_INCOME_TAX + c*OC_C_INCOME_TAX + i*OC_I_INCOME_TAX) / 100;

// Here is the gouvernment's help for this year :D
	_uiIncome += _uiIncome * OC_INCOME_HELP/100;
	_liCityFund += _uiIncome;
	OPENCITY_INFO(
		"Maintenace " << maintenance <<
		" Income: " << _uiIncome <<
		" d/m/y: " << _uiDay << "/" << _uiMonth << "/" << _uiYear <<
		" R/C/I: " << r << "/" << c << "/" << i
	);

	_uiIncome = 0;
}


   /*=====================================================================*/
void
City::_HandleMenuClick()
{
	assert( _pctrMenu != NULL );

	uint uiObject = _pctrMenu->GetClick();

	switch (uiObject) {
		case 1:
			ocRestart();
			break;

		case 2:		// Load
			_Load( ocSaveDirPrefix( "opencity.save" ) );
			break;
		case 3:		// Save
			_Save( ocSaveDirPrefix( "opencity.save" ) );
			break;

		case 4:		// Quit button
			ocQuit();
			break;

		default:
			OPENCITY_DEBUG( "Menu design error");
			assert(0);
	}

	_UnloadMenu();
}

   /*=====================================================================*/
void
City::_HandleWinStatistics()
{
	assert( _pwStatistics != NULL );

	uint uiObject = ((GUIContainer*)_pwStatistics->GetContainer())->GetClick();

	switch (uiObject) {
		case 1:		// Window
			break;

		case 2:		// Close button
			_pwStatistics->close();
			break;

		default:
			OPENCITY_DEBUG( "Statistics window error");
			assert(0);
	}
}

   /*=====================================================================*/
void
City::_HandleWinQuery()
{
	assert( _pwQwery != NULL );

	uint uiObject = ((GUIContainer*)_pwQwery->GetContainer())->GetClick();

	switch (uiObject) {
		case 1:		// Window
			break;

		case 2:		// Close button
			_pwQwery->close();
			break;

		default:
			OPENCITY_DEBUG( "Query window error");
			assert(0);
	}
}

   /*=====================================================================*/
void
City::_HandleStatusClick()
{
	uint uiObject = _pctrStatus->GetClick();

// WARNING: the GUI button displays the current speed
	switch (uiObject) {
		case 1:		// Click on Pause button
			if( _pbtnPause->IsActive() )
			{
				OPENCITY_DEBUG( "Normal speed mode" );
				_eSpeed = OC_SPEED_NORMAL;
				_pbtnPause->SetActive(false);
				_pbtnPlay->SetActive(true);
			}
			else {
				OPENCITY_DEBUG( "Pause mode" );
				_eSpeed = OC_SPEED_PAUSE;
				_pbtnPause->SetActive(true);
				_pbtnPlay->SetActive(false);
				_pbtnFast->SetActive(false);
			}
			break;

		case 2:		// Click on Play button
			OPENCITY_DEBUG( "Normal speed mode" );
			_eSpeed = OC_SPEED_NORMAL;
			_pbtnPause->SetActive(false);
			_pbtnPlay->SetActive(true);
			_pbtnFast->SetActive(false);
			break;

		case 3:		// Click on Fast button
			OPENCITY_DEBUG( "Fast speed mode" );
			_eSpeed = OC_SPEED_FAST;
			_pbtnPause->SetActive(false);
			_pbtnPlay->SetActive(false);
			_pbtnFast->SetActive(true);
			break;
			
		case 4:		// Click on Query tool
			OPENCITY_DEBUG( "Query tool" );
			if( _eCurrentTool == OC_TOOL_QUERY )
				_SetCurrentTool( OC_TOOL_NONE );
			else
				_SetCurrentTool( OC_TOOL_QUERY );
			break;

		default:
			OPENCITY_DEBUG( "WARNING: What is this control -> " << uiObject);
			//assert(0);
			break;
	} // switch

	_pctrStatus->ResetAttribute( OC_GUIMAIN_CLICKED /*| OC_GUIMAIN_MOUSEOVER*/ );
}


   /*=====================================================================*/
void
City::_HandleGUIClick()
{
	uint uiObject;
	GUIContainer* pOldContainer;
	int iX, iY;

	pOldContainer = _pctr;
	uiObject = _pctr->GetClick();

// is this the main container ?
	if (_pctr == _pctrMain)
	switch (uiObject) {
		case 1: // switch to Z toolcircle
			_pctr = _pctrZ;
			_pctr->Set( 1, OC_GUIMAIN_MOUSEOVER );
			break;
		case 2: // load/save toolcircle
			_pctr = _pctrS;
			_pctr->Set( 1, OC_GUIMAIN_MOUSEOVER );
			break;
		case 3:  // L button, open the L toolcircle
			_pctr = _pctrL;
			_pctr->Set( 1, OC_GUIMAIN_MOUSEOVER );
			break;
		case 4:  // P button, set tool for "building road"
			_SetCurrentTool( OC_TOOL_ROAD );
			break;
		case 5: // T button, open the "Terrain" toolcircle
			_pctr = _pctrT;
			_pctr->Set( 1, OC_GUIMAIN_MOUSEOVER );
			break;
		case 6: // G button, open the government toolcircle
			_pctr = _pctrG;
			_pctr->Set( 1, OC_GUIMAIN_MOUSEOVER );
			break;

		default: // never reached
			OPENCITY_DEBUG( "WARNING: What's going wrong ?" );
			assert(0);
			break;
	}

// the user clicked on the Zone toolcircle
	else if (_pctr == _pctrZ )
	switch (uiObject) {
		case 1: // back button, open the main toolcircle
			_pctr = _pctrMain;
			_pctr->Set( 1, OC_GUIMAIN_MOUSEOVER );
			break;
		case 2: // R button
			_SetCurrentTool( OC_TOOL_ZONE_RES );
			break;
		case 3:  // C button, set tool for "zone commercial"
			_SetCurrentTool( OC_TOOL_ZONE_COM );
			break;
		case 4:  // I button, set tool for "zone industrial"
			_SetCurrentTool( OC_TOOL_ZONE_IND );
			break;

		default:
			OPENCITY_DEBUG("Design error");
			assert( 0 );
			break;
	}

// the user clicked on the eLectric toolcircle
	else if (_pctr == _pctrL)
	switch (uiObject) {
		case 1: // back button, open the main toolcircle
			_pctr = _pctrMain;
		// highlight the previous button under the mouse cursor
			_pctr->Set( 3, OC_GUIMAIN_MOUSEOVER );
			break;
		case 2:  // L button, set tool for building electric lines
			_SetCurrentTool( OC_TOOL_ELINE );
			break;
		case 3:  // set tool for building nuclear power plant
			_SetCurrentTool( OC_TOOL_EPLANT_NUCLEAR );
			break;
		case 4:  // set tool for building coal power plant
			_SetCurrentTool( OC_TOOL_EPLANT_COAL );
			break;

		default:
			OPENCITY_DEBUG( "WARNING: What's going wrong ?" );
			assert(0);
			break;
	}

// the user clicked on the Terrain toolcircle
	else if (_pctr == _pctrT)
	switch (uiObject) {
		case 1: // back button, open the main toolcircle
			_pctr = _pctrMain;
			_pctr->Set( 5, OC_GUIMAIN_MOUSEOVER );
			break;
		case 2:  // height up
			_SetCurrentTool( OC_TOOL_HEIGHT_UP );
			break;
		case 3:  // height down
			_SetCurrentTool( OC_TOOL_HEIGHT_DOWN );
			break;
		case 4:  // destroy tool
			_SetCurrentTool( OC_TOOL_DESTROY );
			break;
		case 5: // query tool
			_SetCurrentTool( OC_TOOL_QUERY );
			break;

		default:
			OPENCITY_DEBUG( "WARNING: What's going wrong ?" );
			assert(0);
			break;
	}

// the user clicked on the government toolcircle
	else if (_pctr == _pctrG)
	switch (uiObject) {
		case 1: // back button, open the main toolcircle
			_pctr = _pctrMain;
			_pctr->Set( 6, OC_GUIMAIN_MOUSEOVER );
			break;
		case 2:  // nature toolcircle
			_pctr = _pctrN;
			_pctr->Set( 1, OC_GUIMAIN_MOUSEOVER );
			break;
		case 3:
			_SetCurrentTool( OC_TOOL_EDUCATION );
			break;
		case 4:
			_SetCurrentTool( OC_TOOL_HOSPITAL );
			break;
		case 5:
			_SetCurrentTool( OC_TOOL_POLICE );
			break;
		case 6:
			_SetCurrentTool( OC_TOOL_FIRE );
			break;

		default:
			OPENCITY_DEBUG( "WARNING: Unknown command" );
			assert(0);
			break;
	}

// the user clicked on the "nature" toolcircle
	else if (_pctr == _pctrN)
	switch (uiObject) {
		case 1: // back button, open the government toolcircle
			_pctr = _pctrG;
			_pctr->Set( 2, OC_GUIMAIN_MOUSEOVER );
			break;
		case 2:  // build park
			_SetCurrentTool( OC_TOOL_PARK );
			break;
		case 3:  // build tree
			_SetCurrentTool( OC_TOOL_FLORA );
			break;

		default:
			OPENCITY_DEBUG( "WARNING: Unknown tool" );
			assert(0);
			break;
	}

// the user clicked on the load/save toolcircle
	else if (_pctr == _pctrS)
	switch (uiObject) {
		case 1: // back button, open the main toolcircle
			_pctr = _pctrMain;
			_pctr->Set( 2, OC_GUIMAIN_MOUSEOVER );
			break;
		case 2:  // save
			_Save( ocSaveDirPrefix( "opencity.save" ) );
			break;
		case 3:  // load
			_Load( ocSaveDirPrefix( "opencity.save" ) );
			break;

		default:
			OPENCITY_DEBUG( "WARNING: Unknown command" );
			assert(0);
			break;
	}

// the user clicked on the Path toolcircle
	else if (_pctr == _pctrPath)
	switch (uiObject) {
		case 1: // start button
			_SetCurrentTool( OC_TOOL_NONE );
			boolPathGo = false;
//debug cout << "changed to false" << endl;
			break;
		case 2: // stop button
			_SetCurrentTool( OC_TOOL_NONE );
			boolPathGo = true;
			this->uiVehicleType = Vehicle::VEHICLE_BUS;
			break;
		case 3: // stop button
			_SetCurrentTool( OC_TOOL_NONE );
			boolPathGo = true;
			this->uiVehicleType = Vehicle::VEHICLE_SPORT;
//debug cout << "changed to true" << endl;
			break;
		case 4: // build test building
			_SetCurrentTool( OC_TOOL_TEST_BUILDING );
			break;
		default:
			break;
	}

// The user has clicked on the MAS toolcircle
	else if (_pctr == _pctrMAS)
	switch (uiObject) {
		case 1: // start button
			_SetCurrentTool( OC_TOOL_AGENT_POLICE );
			break;
		case 2: // stop button
			_SetCurrentTool( OC_TOOL_AGENT_DEMONSTRATOR );
			break;
		case 3: // stop button
			_SetCurrentTool( OC_TOOL_AGENT_ROBBER );
			break;

		default:
			break;
	}


// IF the container has been changed then we reset
// the MouseOver & Clicked attributes
// otherwise we reset only the Clicked attribute
	if (_pctr != pOldContainer) {
		pOldContainer->ResetAttribute( OC_GUIMAIN_CLICKED | OC_GUIMAIN_MOUSEOVER );
		pOldContainer->GetLocation( iX, iY );
		_pctr->SetLocation( iX, iY );
		_pctr->Set( OC_GUIMAIN_VISIBLE );
	}
	else {
		pOldContainer->ResetAttribute( OC_GUIMAIN_CLICKED );
	}
	pOldContainer->Unset( OC_GUIMAIN_VISIBLE );
}


   /*=====================================================================*/
// Unused at the moment because it disturbs the GUI
void
City::_HandleMouseXY()
{
	#define OC_MOUSE_AUTOSCROLL 15

	static int mouseX, mouseY;

// return immediately if the app doesn't have mouse focus
	if (!(SDL_GetAppState() & SDL_APPMOUSEFOCUS ))
		return;

	SDL_GetMouseState( &mouseX, &mouseY );

// handle horizontal automatic map translation
	if ((mouseX < OC_MOUSE_AUTOSCROLL) && !(mouseY < OC_MOUSE_AUTOSCROLL))
		gVars.gpRenderer->MoveRight();
	if ((mouseX >= _iWinWidth-OC_MOUSE_AUTOSCROLL) && !(mouseY < OC_MOUSE_AUTOSCROLL))
		gVars.gpRenderer->MoveLeft();

// handle vertical automatic map translation
	if (!(mouseX < OC_MOUSE_AUTOSCROLL)
	  &&!(mouseX >= _iWinWidth-OC_MOUSE_AUTOSCROLL) && (mouseY < OC_MOUSE_AUTOSCROLL))
		gVars.gpRenderer->MoveDown();
	if ((mouseY >= _iWinHeight-OC_MOUSE_AUTOSCROLL))
		gVars.gpRenderer->MoveUp();

// handle map rotation
	if ((mouseX < OC_MOUSE_AUTOSCROLL) && (mouseY < OC_MOUSE_AUTOSCROLL))
		gVars.gpRenderer->RotateLeft();

	if ((mouseY < OC_MOUSE_AUTOSCROLL) && (mouseX >= _iWinWidth-OC_MOUSE_AUTOSCROLL))
		gVars.gpRenderer->RotateRight();
}


   /*=====================================================================*/
void
City::_TestPathfinding() {
	if (_pctr == _pctrPath) {
		if (this->boolPathGo == false) {
			this->uiPathStartW = _uiMapW2;
			this->uiPathStartH = _uiMapL2;
		}
		else {
		//TODO: put this somewhere else
			vector<Destination> vdest;

			this->uiPathStopW = _uiMapW2;
			this->uiPathStopH = _uiMapL2;

		// Buses prefer short distance
			if (this->uiVehicleType == Vehicle::VEHICLE_BUS) {
				gVars.gpPathFinder->findShortestPath(
					uiPathStartW, uiPathStartH,
					uiPathStopW, uiPathStopH,
					vdest,
					PathFinder::OC_DISTANCE );
			}
		// Sport vehicle prefer less traffic
			else if (this->uiVehicleType == Vehicle::VEHICLE_SPORT) {
				gVars.gpPathFinder->findShortestPath(
					uiPathStartW, uiPathStartH,
					uiPathStopW, uiPathStopH,
					vdest,
					PathFinder::OC_TRAFFIC );
			}

		// Now create the new vehicle if a path was found
			if ( vdest.size() > 0 ) {
				pvehicle = new Vehicle(
					(Vehicle::VEHICLE_TYPE)this->uiVehicleType );
				pvehicle->SetPath( vdest );	// path init
				pvehicle->Start();		// vehicle init
				if (gVars.gpMoveMgr->Add( pvehicle ) < 0) {
					OPENCITY_DEBUG("MoveMgr full");
					delete pvehicle;
				}
			}
		}
	}
//debug: pathfinding
/*
cout << "StW: " << uiPathStartW << " / " << " StH: " << uiPathStartH
     << " SpW: " << uiPathStopW << " / " << "SpH: " << uiPathStopH << endl;
*/
}


   /*=====================================================================*/
void
City::_BuildPreview()
{
	static OPENCITY_STRUCTURE_CODE scode = OC_STRUCTURE_UNDEFINED;
	static OPENCITY_GRAPHIC_CODE gcode = OC_EMPTY;
	static OPENCITY_TOOL_CODE tcode = OC_TOOL_NONE;
	static OPENCITY_ERR_CODE ecode = OC_ERR_FREE;


// Get the corresponding structure code of the tool
	if (tcode != _eCurrentTool) {
		tcode = _eCurrentTool;
		switch (tcode) {
		/* not implemented yet
			case OC_TOOL_ZONE_RES:
				scode = OC_STRUCTURE_RES;
				break;
			case OC_TOOL_ZONE_COM:
				scode = OC_STRUCTURE_COM;
				break;
			case OC_TOOL_ZONE_IND:
				scode = OC_STRUCTURE_IND;
				break;
			case OC_TOOL_ROAD:
				scode = OC_STRUCTURE_ROAD;
				break;
			case OC_TOOL_ELINE:
				scode = OC_STRUCTURE_ELINE;
				break;
		*/

			case OC_TOOL_EPLANT_COAL:
				scode = OC_STRUCTURE_EPLANT_COAL;
				break;
			case OC_TOOL_EPLANT_NUCLEAR:
				scode = OC_STRUCTURE_EPLANT_NUCLEAR;
				break;
			case OC_TOOL_PARK:
				scode = OC_STRUCTURE_PARK;
				break;
			case OC_TOOL_FLORA:
				scode = OC_STRUCTURE_FLORA;
				break;
			case OC_TOOL_FIRE:
				scode = OC_STRUCTURE_FIREDEPT;
				break;
			case OC_TOOL_POLICE:
				scode = OC_STRUCTURE_POLICEDEPT;
				break;
			case OC_TOOL_HOSPITAL:
				scode = OC_STRUCTURE_HOSPITALDEPT;
				break;
			case OC_TOOL_EDUCATION:
				scode = OC_STRUCTURE_EDUCATIONDEPT;
				break;

			case OC_TOOL_TEST_BUILDING:
				scode = OC_STRUCTURE_TEST;
				break;

			default:
				scode = OC_STRUCTURE_UNDEFINED;
				break;
		} // switch
	} // if

// Get the corresponding graphic code
	if ( scode != OC_STRUCTURE_UNDEFINED ) {

		ecode = _apLayer[ _eCurrentLayer ]->
			BuildPreview( _uiMapW, _uiMapL, scode, gcode, _liCityFund );

		if (ecode == OC_ERR_FREE) {
			gVars.gpRenderer->DisplayBuildPreview(
				_uiMapW, _uiMapL, OC_GREEN_COLOR, gcode );
		}
		else {
			gVars.gpRenderer->DisplayBuildPreview(
				_uiMapW, _uiMapL, OC_RED_COLOR, gcode );
		}
	}
}


   /*=====================================================================*/
bool
City::_Save( const string& strFilename )
{
	fstream fs;

	fs.open( strFilename.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );
	if (!fs.good()) {
		OPENCITY_DEBUG( "File opening error: " << strFilename );
		return false;
	}

// Save the signature and version
	fs << "OpenCity_" << ocStrVersion() << std::endl;
	fs << ocLongVersion() << std::ends;

// Save city data
	this->SaveTo( fs );

// Save map data
	gVars.gpMapMgr->SaveTo( fs );

// Save layers's data
	_apLayer[ OC_LAYER_BUILDING ]->SaveTo( fs );

// Save simulators data
	_pMSim->SaveTo( fs );

	fs.close();
	return true;
}


   /*=====================================================================*/
bool
City::_Load( const string& strFilename )
{
	fstream fs;
	uint w, l;

	fs.open( strFilename.c_str(), ios_base::in | ios_base::binary );
	if (!fs.good()) {
		OPENCITY_DEBUG( "File opening error in: " << strFilename );
		return false;
	}

// Load the signature and version
	string strSignature;
	long currentVersion = ocLongVersion();
	long lVersion;

	getline( fs, strSignature );
	fs >> lVersion; fs.ignore();

// Version checking
	if (lVersion > currentVersion) {
		OPENCITY_INFO( "Failed to load a more recent save file: " << strFilename );
		OPENCITY_INFO( "Current OpenCity version is: " << currentVersion );
		OPENCITY_INFO( "The save file version is : " << lVersion );
		return false;
	}

// Remove all moving objects
	gVars.gpMoveMgr->Remove();

// Load city data
	OPENCITY_INFO( "Loading save file from " << strSignature );
	this->LoadFrom( fs );

// Load map data
	gVars.gpMapMgr->LoadFrom( fs );
	gVars.gpRenderer->bHeightChange = true;
	gVars.gpRenderer->bMinimapChange = true;

// Load layers' data
	_apLayer[ OC_LAYER_BUILDING ]->LoadFrom( fs );

// Load simulators' data
	_pMSim->LoadFrom( fs );

// Manually add the structures to the simulators
	for ( w = 0; w < _uiWidth; w++ ) {
		for ( l = 0; l < _uiLength; l++ ) {
			_pMSim->AddStructure( w, l, w, l );
		}
	}

// Refresh/recalculate the simulators' value
	_pMSim->RefreshSimValue();

	fs.close();
	return true;
}




































