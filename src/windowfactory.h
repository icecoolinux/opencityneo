/***************************************************************************
						windowfactory.h    -  description
							-------------------
	begin                : november, 23th 2017
	copyright            : (C) 2006 by Icecool
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

#ifndef _OPENCITY_WINDOWFACTORY_H_
#define _OPENCITY_WINDOWFACTORY_H_ 1

#include "guiwindow.h"
#include "structure.h"

//========================================================================
/** Windows factory
*/
class WindowFactory {
public:

// Create a query window.
	static GUIWindow* createQueryWindow(Structure* pstruct);

// Create a static window.
	static GUIWindow* createStatsWindow();

private:

//========================================================================
// Private methods
//========================================================================

	WindowFactory();
};

#endif
































