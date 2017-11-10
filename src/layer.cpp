/***************************************************************************
						layer.cpp  -  description
							-------------------
	begin                : september 20th, 2003
	copyright            : (C) 2003-2007 by Duong Khang NGUYEN
	email                : neoneurone @ gmail com

	$Id: layer.cpp 375 2008-10-28 14:47:15Z neoneurone $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/


// OpenCity headers
#include "layer.h"
#include "structure.h"
#include "guicontainer.h"	// for the "Query" function
#include "guibutton.h"

// Static variables
uint Layer::_uiNumberLayer = 0;

Layer::Layer(){
	OPENCITY_DEBUG( "ctor" );

// IF called first time THEN create all the static variables
// we do it here, because we need an initialized OpenGL context
// if we do it on the declaration line, there will be runtime errors !
	if ( Layer::_uiNumberLayer++ > 0)
		return;
}


   /*=====================================================================*/
Layer::~Layer(){
	OPENCITY_DEBUG( "dtor" );

// IF not called for the last derived class THEN return
	if ( Layer::_uiNumberLayer-- > 1)
		return;
}


   /*=====================================================================*/
const uint
Layer::GetMaxLinear() const
{
	return _uiLayerWidth * _uiLayerLength - 1;
}


   /*=====================================================================*/
void
Layer::GetLayerSize(
	uint & w,
	uint & l) const
{
	w = _uiLayerWidth;
	l = _uiLayerLength;
}



























