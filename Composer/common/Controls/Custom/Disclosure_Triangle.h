/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Disclosure_Triangle_h
#define Disclosure_Triangle_h

#include "Custom_Draw_Control.h"
#include "Composer_UI.h"


class	Disclosure_Triangle : public Custom_Draw_Control
{
	typedef Custom_Draw_Control	base;

	public:
		Disclosure_Triangle( ComposerUI& dialog_runner, const ASInt32 control_id, VoidCallback p_callback ) :
			base( dialog_runner, control_id ),
			mCallback( p_callback ) 
			{}

	private:
		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		Draw( ADMDrawerRef environment );

		VoidCallback		mCallback;
};

#endif

// EOF
