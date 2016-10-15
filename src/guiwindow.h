/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef _OPENCITY_GUIWINDOW_H_
#define _OPENCITY_GUIWINDOW_H_ 1

#include "guimain.h"
#include "guibutton.h"
#include "texture.h"


//========================================================================
/** A window gui for display information in a window.
*/
class GUIWindow : public GUIMain {
public:
	GUIWindow();
	GUIWindow(
		const int& ciX,
		const int& ciY,
		const uint& cuiW,
		const uint& cuiH,
		const string& strTitle);
	~GUIWindow();

	// Close de window
	void close();

//========================================================================
// Inherited methods from GUIMain
//========================================================================
	void
	Display() const;

	// Smart set window position
	void
	SetLocation(
		const int & rciX,
		const int & rciY );

//========================================================================
// Inherited methods from UI
//========================================================================
	void Keyboard( const SDL_KeyboardEvent& rcEvent );
	void MouseMotion( const SDL_MouseMotionEvent& rcEvent );
	void MouseButton( const SDL_MouseButtonEvent& rcEvent );
	void Expose( const SDL_ExposeEvent& rcEvent );
	void Resize( const SDL_ResizeEvent& rcEvent );


private:
	string _strTitle;
	GUIButton* _pbtnClose;
	bool titleBarClicked;
	static Texture* _textureWindow;
};

#endif
































