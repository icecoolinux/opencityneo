
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

// Global settings
#include "globalvar.h"
extern GlobalVar gVars;

#define TEXTURE_FILE_WINDOW "graphism/gui/windowgui.png"
#define HEIGHT_TITLE_BAR 33
#define PIXELS_BORDER_WINDOW 30
#define SIZE_CLOSE_BUTTON 25

// Texture of the window: borders, title bar and corners.
Texture* GUIWindow::_textureWindow = NULL;

   /*=====================================================================*/
GUIWindow::GUIWindow()
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
)
{
	OPENCITY_DEBUG( "Pctor" );

// Initialize the position and dimension of the window
	_iX = 0;
	_iY = 0;
	_uiWidth = cuiW;
	_uiHeight = cuiH;

	titleBarClicked = false;

// Window title
	_lblTitle = new GUILabel(cuiW/2, cuiH-3*HEIGHT_TITLE_BAR/4, strTitle);
	_lblTitle->SetAlign(GUILabel::OC_ALIGN_CENTER);
	_lblTitle->SetForeground(OPENCITY_PALETTE[Color::OC_WHITE]);

// Load the texture of the window if it's not load.
	if (_textureWindow == NULL) {
		_textureWindow = new Texture( ocDataDirPrefix(TEXTURE_FILE_WINDOW) );
	}

// Container
	_pctr = new GUIContainer(ciX, ciY, cuiW, cuiH);

// Close button.
	_pbtnClose = new GUIButton(cuiW-HEIGHT_TITLE_BAR, cuiH-HEIGHT_TITLE_BAR, SIZE_CLOSE_BUTTON, SIZE_CLOSE_BUTTON, ocDataDirPrefix("graphism/gui/destroy"));

// Add elements into the container
	((GUIContainer*)_pctr)->Add(this);
	((GUIContainer*)_pctr)->Add(_pbtnClose);

// Visible for default
	((GUIContainer*)_pctr)->Set( OC_GUIMAIN_VISIBLE );
	Set( OC_GUIMAIN_VISIBLE );
}


   /*=====================================================================*/
GUIWindow::~GUIWindow()
{
	OPENCITY_DEBUG( "Dwin" );
	delete _pbtnClose;
	delete _lblTitle;
}

// Close de window
void GUIWindow::close()
{
	_pctr->Unset(OC_GUIMAIN_VISIBLE);
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
	glEnable( GL_BLEND );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	glColor3ub( OC_WHITE_COLOR );

// Draw corners.
	glBindTexture( GL_TEXTURE_2D, _textureWindow->GetName() );

	// Bottom left corner.
	glBegin( GL_QUADS );
	glTexCoord2f( 0, 0 );		glVertex2i( 0, 0 );
	glTexCoord2f( 0.5, 0 );		glVertex2i( PIXELS_BORDER_WINDOW, 0 );
	glTexCoord2f( 0.5, 0.5 );	glVertex2i( PIXELS_BORDER_WINDOW, PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0, 0.5 );		glVertex2i( 0, PIXELS_BORDER_WINDOW );
	glEnd();

	// Bottom right corner.
	glBegin( GL_QUADS );
	glTexCoord2f( 0.5, 0 );		glVertex2i( _uiWidth - PIXELS_BORDER_WINDOW, 0 );
	glTexCoord2f( 1, 0 );		glVertex2i( _uiWidth, 0 );
	glTexCoord2f( 1, 0.5 );		glVertex2i( _uiWidth, PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.5, 0.5 );	glVertex2i( _uiWidth - PIXELS_BORDER_WINDOW, PIXELS_BORDER_WINDOW );
	glEnd();

	// Top right corner.
	glBegin( GL_QUADS );
	glTexCoord2f( 0.5, 0.5 );	glVertex2i( _uiWidth - PIXELS_BORDER_WINDOW, _uiHeight - PIXELS_BORDER_WINDOW );
	glTexCoord2f( 1, 0.5 );		glVertex2i( _uiWidth, _uiHeight - PIXELS_BORDER_WINDOW );
	glTexCoord2f( 1, 1 );		glVertex2i( _uiWidth, _uiHeight );
	glTexCoord2f( 0.5, 1 );		glVertex2i( _uiWidth-PIXELS_BORDER_WINDOW, _uiHeight );
	glEnd();

	// Top left corner.
	glBegin( GL_QUADS );
	glTexCoord2f( 0, 0.5 );		glVertex2i( 0, _uiHeight - PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.5, 0.5 );	glVertex2i( PIXELS_BORDER_WINDOW, _uiHeight - PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.5, 1 );		glVertex2i( PIXELS_BORDER_WINDOW, _uiHeight );
	glTexCoord2f( 0, 1 );		glVertex2i( 0, _uiHeight );
	glEnd();

// Draw border

	// Border left
	glBegin( GL_QUADS );
	glTexCoord2f( 0.0, 0.25 );	glVertex2i( 0, PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.5, 0.25 );	glVertex2i( PIXELS_BORDER_WINDOW, PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.5, 0.75 );	glVertex2i( PIXELS_BORDER_WINDOW, _uiHeight - PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.0, 0.75 );	glVertex2i( 0, _uiHeight - PIXELS_BORDER_WINDOW );
	glEnd();

	// Border right
	glBegin( GL_QUADS );
	glTexCoord2f( 0.5, 0.25 );	glVertex2i( _uiWidth - PIXELS_BORDER_WINDOW, PIXELS_BORDER_WINDOW );
	glTexCoord2f( 1.0, 0.25 );	glVertex2i( _uiWidth, PIXELS_BORDER_WINDOW );
	glTexCoord2f( 1.0, 0.75 );	glVertex2i( _uiWidth, _uiHeight - PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.5, 0.75 );	glVertex2i( _uiWidth - PIXELS_BORDER_WINDOW, _uiHeight - PIXELS_BORDER_WINDOW );
	glEnd();

	// Border top
	glBegin( GL_QUADS );
	glTexCoord2f( 0.25, 0.5 );	glVertex2i( PIXELS_BORDER_WINDOW, _uiHeight - PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.75, 0.5 );	glVertex2i( _uiWidth - PIXELS_BORDER_WINDOW, _uiHeight - PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.75, 1.0 );	glVertex2i( _uiWidth - PIXELS_BORDER_WINDOW, _uiHeight );
	glTexCoord2f( 0.25, 1.0 );	glVertex2i( PIXELS_BORDER_WINDOW, _uiHeight );
	glEnd();

	// Border bottom
	glBegin( GL_QUADS );
	glTexCoord2f( 0.25, 0.0 );	glVertex2i( PIXELS_BORDER_WINDOW, 0 );
	glTexCoord2f( 0.75, 0.0 );	glVertex2i( _uiWidth - PIXELS_BORDER_WINDOW, 0 );
	glTexCoord2f( 0.75, 0.5 );	glVertex2i( _uiWidth - PIXELS_BORDER_WINDOW, PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.25, 0.5 );	glVertex2i( PIXELS_BORDER_WINDOW, PIXELS_BORDER_WINDOW );
	glEnd();

// Draw window's middle
	glBegin( GL_QUADS );
	glTexCoord2f( 0.25, 0.25 );	glVertex2i( PIXELS_BORDER_WINDOW, PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.75, 0.25 );	glVertex2i( _uiWidth - PIXELS_BORDER_WINDOW, PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.75, 0.75 );	glVertex2i( _uiWidth - PIXELS_BORDER_WINDOW, _uiHeight - PIXELS_BORDER_WINDOW );
	glTexCoord2f( 0.25, 0.75 );	glVertex2i( PIXELS_BORDER_WINDOW, _uiHeight - PIXELS_BORDER_WINDOW );
	glEnd();

// Display title
	_lblTitle->Display();

// Restore the old matrix and attribs
	glPopMatrix();
	glPopAttrib();
}

// Smart set window position
void
GUIWindow::SetLocation(const int & rciX, const int & rciY ) {

	int xWin = rciX;
	int yWin = rciY - _uiHeight;

	if( (xWin+_uiWidth) > gVars.guiScreenWidth )
		xWin = gVars.guiScreenWidth - _uiWidth;
	else if( xWin < 0 )
		xWin = 0;

	if( (yWin+_uiHeight) > gVars.guiScreenHeight )
		yWin = gVars.guiScreenHeight - _uiHeight;
	else if( yWin < 0 )
		yWin = 0;

	_pctr->SetLocation(xWin, yWin);
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

// The window must be contained in a guicontainer !
	assert( _pctr != NULL );

	// Move the window if title bar is clicked
	if(titleBarClicked) {
		// Move the container
		int ctrX, ctrY;
		_pctr->GetLocation(ctrX, ctrY);

		ctrX += rcsMouseEvent.xrel;
		ctrY -= rcsMouseEvent.yrel;

		_pctr->SetLocation(ctrX, ctrY);
	}

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
			if ( IsSet( OC_GUIMAIN_MOUSEOVER )) {
				Set( OC_GUIMAIN_CLICKED );

				// Clicked over title bar, window will traslate
				titleBarClicked = true;
			}
			else
				Unset( OC_GUIMAIN_CLICKED );
		}
	}
	// User released LMB.
	else if ( buttonEvent.state == SDL_RELEASED ) {
		// Left Mpuse Button
		if ( buttonEvent.button == SDL_BUTTON_LEFT ) {
			// Release window traslate
			titleBarClicked = false;
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
































