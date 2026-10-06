/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef _H_MCColorList
#define _H_MCColorList

#include "Custom_Draw_Control.h"
#include "MCColorbook.h"


class	MCColorList: public Custom_Draw_Control
{
	typedef Custom_Draw_Control	base;

	public:
		MCColorList( Dialog_Runner& dialog_runner, const ASInt32 control_id );
		
		// called from the dialog as we somehow don't get key strokes!
		bool			HandleKeyClick( ADMTrackerRef tracker );

	protected:
		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		Draw( ADMDrawerRef environment );
		
		// used to select a new color
		void				SelectNewColor( MCColorbookIndex );
		
		// this handles building a search string and well, searching for it...
		bool				HandleSearch( MCColorbook*, ADMTrackerRef tracker, ADMChar );

	private:
		ADMTime				mLastKeyTime;
		RVNameField			mSearch;		// this is the search criteria
};

#endif


// EOF
