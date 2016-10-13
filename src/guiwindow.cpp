
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/

// OpenCity headers
#include "guiwindow.h"
#include "guicontainer.h"

#define TEXTURE_FILE_WINDOW "graphism/gui/windowgui.png"

// Texture of the window: borders, title bar and corners.
Texture* GUIWindow::_textureWindow = NULL;

   /*=====================================================================*/
GUIWindow::GUIWindow():
_strTitle("")
{
	OPENCITY_DEBUG( "Dctor" );
}


   /*=====================================================================*/
GUIWindow::GUIWindow
(
	const int& ciX,
	const int& ciY,
	const uint& cuiW,
	const uint& cuiH,
	const string& strTitle
):
_strTitle(strTitle)
{
	OPENCITY_DEBUG( "Pctor" );

// Initialize the position and dimension of the window
	_iX = ciX;
	_iY = ciY;
	_uiWidth = cuiW;
	_uiHeight = cuiH;

// Load the texture of the window if it's not load.
	if (_textureWindow == NULL) {
		_textureWindow = new Texture( ocDataDirPrefix(TEXTURE_FILE_WINDOW) );
	}

// Container
	_pctr = new GUIContainer(ciX, ciY, cuiW, cuiH);
	((GUIContainer*)_pctr)->Add(this);

// Close button.
	close = new GUIButton(cuiW-30, cuiH-30, 30, 30, ocDataDirPrefix("graphism/gui/destroy"));

// Visible for default
	((GUIContainer*)_pctr)->Set( OC_GUIMAIN_VISIBLE );
	Set( OC_GUIMAIN_VISIBLE );
}


   /*=====================================================================*/
GUIWindow::~GUIWindow()
{
	OPENCITY_DEBUG( "Dwin" );
	delete close;
}


   /*=====================================================================*/
void
GUIWindow::Display() const
{
// Return immediatly if this is NOT visible
	if ( !IsSet( OC_GUIMAIN_VISIBLE ) )
		return;

// Translate the window to the correct position
	glPushAttrib( GL_ENABLE_BIT );
	glDisable( GL_LIGHTING );
	glPushMatrix();
	// glTranslatef( _iX, _iY, 0.0 );

// Activate the texture 2D processing
	glEnable( GL_TEXTURE_2D );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

	glColor3ub( OC_WHITE_COLOR );

// Draw title bar.
	glBindTexture( GL_TEXTURE_2D, _textureWindow->GetName() );
	glBegin( GL_QUADS );
	glTexCoord2i( 0, 0 );	glVertex2i( 0, 0 );
	glTexCoord2i( 1, 0 );	glVertex2i( _uiWidth-1, 0 );
	glTexCoord2i( 1, 1 );	glVertex2i( _uiWidth-1, _uiHeight-1 );
	glTexCoord2i( 0, 1 );	glVertex2i( 0, _uiHeight-1 );
	glEnd();

// Restore the old matrix and attribs
	glPopMatrix();
	glPopAttrib();

	// Button close.
	close->Display();
}


   /*=====================================================================*/
void
GUIWindow::Keyboard( const SDL_KeyboardEvent& rcEvent )
{}


   /*=====================================================================*/
void
GUIWindow::MouseMotion( const SDL_MouseMotionEvent& rcsMouseEvent )
{
	static int realX, realY;
	static int winW, winH;
	static int myX, myY;

/* Always calculate the mouse over state.
// Return immediatly if this is NOT visible
	if ( !IsSet( OC_GUIMAIN_VISIBLE ) )
		return;
*/

// The button must be contained in a guicontainer !
	assert( _pctr != NULL );

// Calculate the real X,Y from the mouse X,Y;
	((GUIContainer*)_pctr)->GetWinWH( winW, winH );
	realX = rcsMouseEvent.x;
	realY = winH - rcsMouseEvent.y - 1;

// Calculate my absolute coordinates via the container
	_pctr->GetLocation( myX, myY );
	myX += _iX;
	myY += _iY;

//debug
//cout << "realX: " << realX << " / realY: " << realY << endl;

   // if the mouse position is within the range, turn on boolMouseOver
	if ( (realX >= myX) && (realX <= (int)(myX + _uiWidth))
	  && (realY >= myY) && (realY <= (int)(myY + _uiHeight))) {
		Set( OC_GUIMAIN_MOUSEOVER );
	}
	else {
		Unset( OC_GUIMAIN_MOUSEOVER | OC_GUIMAIN_CLICKED );
	}
}


   /*=====================================================================*/
void
GUIWindow::MouseButton( const SDL_MouseButtonEvent& buttonEvent )
{
// Return immediatly if this is NOT visible
	if ( !IsSet( OC_GUIMAIN_VISIBLE ) )
		return;

// IF the user clicked the LMB when the mouse is over the control
// THEN turn on the onClick state
// otherwise turn it off
	if ( buttonEvent.state == SDL_PRESSED ) {
		if ( buttonEvent.button == SDL_BUTTON_LEFT ) {
			if ( IsSet( OC_GUIMAIN_MOUSEOVER ))
				Set( OC_GUIMAIN_CLICKED );
			else
				Unset( OC_GUIMAIN_CLICKED );
		}
	}
}


   /*=====================================================================*/
void
GUIWindow::Expose( const SDL_ExposeEvent& rcEvent )
{
	this->Display();
}


   /*=====================================================================*/
void
GUIWindow::Resize( const SDL_ResizeEvent& rcEvent )
{
}
































