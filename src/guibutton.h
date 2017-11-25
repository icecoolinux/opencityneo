/***************************************************************************
						guibutton.h    -  description
							-------------------
	begin                : march 22th, 2004
	copyright            : (C) 2004-2008 by Duong Khang NGUYEN
	email                : neoneurone @ gmail com

	$Id: guibutton.h 375 2008-10-28 14:47:15Z neoneurone $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef _OPENCITY_GUIBUTTON_H_
#define _OPENCITY_GUIBUTTON_H_ 1

#include "guimain.h"
#include "texture.h"


//========================================================================
/** One or two states button: onMouseOver and onMouseOut. The default
is a two state button
*/
class GUIButton : public GUIMain {
public:
	GUIButton();
	GUIButton(
		const int& rciX,
		const int& rciY,
		const uint& rcuiW,
		const uint& rcuiH,
		const string& strFile,
		bool hasMouseOver = true,
		bool hasActive = false);
	~GUIButton();


	bool
	IsActive();
	
	void
	SetActive(bool active);
	
	void
	SetBackground(
		const Color& color );

	void
	SetForeground(
		const Color& color );


//========================================================================
// Inherited methods from GUIMain
//========================================================================
	void
	Display() const;


//========================================================================
// Inherited methods from UI
//========================================================================
	void Keyboard( const SDL_KeyboardEvent& rcEvent );
	void MouseMotion( const SDL_MouseMotionEvent& rcEvent );
	void MouseButton( const SDL_MouseButtonEvent& rcEvent );
	void Expose( const SDL_ExposeEvent& rcEvent );
	void Resize( const SDL_ResizeEvent& rcEvent );


private:
	bool 		_active;					///< The button is active.
	uint		_uiHasMouseOver;			///< Has texture with mouse over (default is true)
	uint		_uiHasActive;				///< Has texture when is active (default is false)
	Texture		moTextureNormal;			///< Inactive button texture
	Texture		moTextureOver;				///< On mouse over button texture
	Texture		moTextureActive;			///< On active button texture

	Color		_cForeground;				///< Foreground color
	Color		_cBackground;				///< Background color
};

#endif































