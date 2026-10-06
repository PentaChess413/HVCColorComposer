/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	Source code rewritten by: Alex Lelievre ([PII Redacted]), 2006.
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Selected_Color_Display.h"
#include "Composer_UI.h"
#include "Custom_UI.h"

#include "admDrawer.h"
#include "admBasic.h"




void Selected_Color_Display::Draw( ADMDrawerRef environment )
{
	ASRect	control_bounds = bounds( true );

	draw_frame( environment, control_bounds );

	ASInt32 old_bottom = control_bounds.bottom;
	
	// draw top patch
	control_bounds.bottom = (control_bounds.bottom - control_bounds.top) / 2 + control_bounds.top;
	mColor.Draw( environment, &control_bounds, false );
	
	// draw bottom patch (the original color)
	control_bounds.top = control_bounds.bottom;
	control_bounds.bottom = old_bottom;
	fill_with_color( environment, control_bounds, mColor.GetOriginalColorForDisplay() );
}



ASBoolean Selected_Color_Display::HandleTracking( ADMTrackerRef tracker )
{
	if( ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonUpAction ) )
	{
		// we check to see if we are in the lower half of the display rectangle...  !!@
		ASRect	control_bounds = bounds( true );
		
		// form lower half rectangle
		control_bounds.top += (control_bounds.bottom - control_bounds.top) / 2;

		ASPoint	mousePoint = {0};
		ADM_Access::tracking_suite()->GetPoint( tracker, &mousePoint );
		
		// Windows api !!@ make portable...later
#if WIN32		
		if( PtInRect( (RECT*)&control_bounds, *((POINT*)&mousePoint) ) )
#else
		if( PtInRect( *((Point*)&mousePoint), (Rect*)&control_bounds ) )
#endif
		{
			MCColor originalColor = mColor.GetOriginalColor();
		
			// need to select the original color... send it thru the normal channels, don't set it directly!
			if( GetComposerUI() )
				GetComposerUI()->SelectNewColor( originalColor );
			
			// some of these controls are used in both the picker and the custom libraries dialog...
			if( GetLibrariesUI() )
				GetLibrariesUI()->SelectNewColor( originalColor );
		}
		
		return true;
	}
	
	return false;
}


// EOF
