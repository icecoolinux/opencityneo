/***************************************************************************
						mainsim.h  -  description
							-------------------
	begin                : february 21th, 2006
	copyright            : (C) 2006-2007 by Duong Khang NGUYEN
	email                : neoneurone @ gmail com

	$Id: mainsim.h 450 2010-11-21 19:11:43Z neoneurone $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef _OPENCITY_MAINSIM_H_
#define _OPENCITY_MAINSIM_H_ 1

#include "main.h"
#include "simulator.h"
#include "systime.h"

//========================================================================
/** Handles the simulation of the commercial sub-system.
*/
class MainSim : public Simulator
{

public:


//========================================================================
/** Constructs a new MainSim object.
	\param pblayer A pointer to the BuildingLayer
	\param pmap A pointer to the gobal Map object
	\todo Remove pblayer and pmap. Use global pointer for them
*/
	MainSim(
		BuildingLayer* pblayer,
		Map* pmap );


//========================================================================
/** Destructs a MainSim object.
*/
	~MainSim();


//========================================================================
/** Overload the base method in order to call all other microsims' method
	\param rfs A reference to a file stream which is ready for writing
*/
	void
	SaveTo( std::fstream& rfs );


//========================================================================
/** Overload the base method in order to call all other microsims' method
	\param rfs A reference to a file stream which is ready for reading
*/
	void
	LoadFrom( std::fstream& rfs );


//========================================================================
/** Run a step of the simulation
*/
	int
	Run();


//========================================================================
/** Notifies the micro simulators that a new structure has been added to the
system
	\param w1,l1,w2,l2 The area used by the new structure
	\param sim The optional microsim identifier
*/
	void
	AddStructure(
		const uint w1, const uint l1,
		const uint w2, const uint l2,
		const OPENCITY_SIMULATOR sim = OC_SIMULATOR_DEFAULT );


//========================================================================
/** Notify the micro simulators that a structure has been removed from the
system
	\param w1,l1,w2,l2 The area used by the structure
	\param sim The optional microsim identifier
*/
	void
	RemoveStructure(
		const uint w1, const uint l1,
		const uint w2, const uint l2,
		const OPENCITY_SIMULATOR sim = OC_SIMULATOR_DEFAULT );


//========================================================================
/** Refresh the values of all the micro simulators. This method is called
automatically by Run() every 3 turns
	\see Run()
*/
	void
	RefreshSimValue();


//========================================================================
/** Return the global total value of the micro simulator
	\param sim The micro simulator to query the information
	\return The value
*/
	const int
	GetValue( const OPENCITY_SIMULATOR sim ) const;


//========================================================================
/** Return the possible maximum value of the micro simulator.
	\param sim The micro simulator to query the information
	\return The value
	\note Currently, this method only works for the electricity micro
simulator.
*/
	const int
	GetMaxValue( const OPENCITY_SIMULATOR sim ) const;


private:
	/** Table of pointers to Simulator object */
	Simulator* _tpSimulator[Simulator::OC_SIMULATOR_NUMBER];
	
	uint times;	//< For refresh the simulator values.
	unsigned long nextSimulation;	//< Time the next simulation execute.
};

#endif
