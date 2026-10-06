/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Composer_h
#define Composer_h

#include "PickerPlugin.h"
#include "Dialog_Runner.h"

class Composer : public PickerPlugin
{
	typedef PickerPlugin base;

	public:
		Composer();

	private:
		virtual void Pick();

		// Show the about box, return Done or NotDone
		virtual const bool About( AboutRecord* aboutRecord );

		void	show_about_box();
};


#endif

// EOF
