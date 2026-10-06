/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Color_Flag_Handler.h"
#include "HVC_RGB_Color_Pair.h"
#include "Helpers.h"
#include "Switch_Drawing_Mode.h"
#include "Composer_UI.h"
#include "Color_Plane_Selector.h"

#include "admBasic.h"

//const ASInt32 kIndicatorSize = IMAGE_INSET + IMAGE_INSET;
const ASInt32 kIndicatorSize = IMAGE_INSET * 2;


/*
inline ASPoint get_offset( ADMItemRef )
{
	ASPoint	offset = { 0 };

	return offset;
}
*/


Color_Flag_Handler::Color_Flag_Handler( ComposerUI& dialog_runner, const ASInt32 control_id, DisplayPixelsProc proc ) :
					base( dialog_runner, control_id ), 
					alt_base( proc )	//,	indicator_offset(get_offset(native()))
{
}




// Draw the color flag indicator
inline void Color_Flag_Handler::DrawIndicator( ADMDrawerRef environment, const ASRect& bounds ) const
{	
	// Calculate the size
#if WIN32
	ASRect	indicator_bounds = { bounds.left + mY, bounds.top + mX };
#else
	ASRect	indicator_bounds = { bounds.top + mX, bounds.left + mY };
#endif

	indicator_bounds.bottom = indicator_bounds.top + kIndicatorSize;
	indicator_bounds.right = indicator_bounds.left + kIndicatorSize;

//	Switch_Drawing_Mode	switch_mode( environment, kADMXORMode );
//	ADM_Access::drawing_suite()->FillOval( environment, &indicator_bounds );
	
	// draw black then white
	ASRGBColor black = {0,0,0};
	ASRGBColor white = {0xFFFF,0xFFFF,0xFFFF};

	ADM_Access::drawing_suite()->SetRGBColor( environment, &black );
	ADM_Access::drawing_suite()->DrawOval( environment, &indicator_bounds );
	
	// shrink rectangle
	indicator_bounds.top    += 1;
	indicator_bounds.left   += 1;
	indicator_bounds.right  -= 1;
	indicator_bounds.bottom -= 1;
	
	ADM_Access::drawing_suite()->SetRGBColor( environment, &white );
	ADM_Access::drawing_suite()->DrawOval( environment, &indicator_bounds );
}




void Color_Flag_Handler::Draw( ADMDrawerRef environment )
{
	ASRect	l_bounds = bounds();

#if WIN32
	// on Windows we need to get the control's window DC
	ASWindowRef winRef = GetWindowRef();
	HDC dc = GetDC( (HWND)winRef );

	ASPoint	image_origin = { IMAGE_INSET, IMAGE_INSET };
	copy_bit_map( image_origin, dc );											// Draw the flag image

	ReleaseDC( NULL, dc );

	// we need to erase the sides and top of the spectrum (and not the whole rectangle)
	ASRect eraseBounds = bounds( true );
	
	// erase left side
	eraseBounds.right = eraseBounds.left + IMAGE_INSET;
	ADM_Access::drawing_suite()->FillRect( environment, &eraseBounds );
	
	// erase right side
	eraseBounds = bounds( true );
	eraseBounds.left = eraseBounds.right - IMAGE_INSET;
	ADM_Access::drawing_suite()->FillRect( environment, &eraseBounds );

	// erase top side
	eraseBounds = bounds( true );
	eraseBounds.bottom = eraseBounds.top + IMAGE_INSET;
	ADM_Access::drawing_suite()->FillRect( environment, &eraseBounds );

	// erase bottom side
	eraseBounds = bounds( true );
	eraseBounds.top = eraseBounds.bottom - IMAGE_INSET;
	ADM_Access::drawing_suite()->FillRect( environment, &eraseBounds );
#else
	// Erase the area -- this is okay on the Mac where all drawing is double buffered.
	ADM_Access::drawing_suite()->Clear(environment);					
	ASPoint	image_origin = { l_bounds.top + IMAGE_INSET, l_bounds.left + IMAGE_INSET };

	copy_bit_map( image_origin, NULL );											// Draw the flag image
#endif
	
	// draw the frame
	l_bounds = bounds( true );
	draw_frame( environment, l_bounds );
	
	// now draw that little dot to indicate the selected dot.
	DrawIndicator( environment, l_bounds );								// Draw the indicator
}




ASBoolean Color_Flag_Handler::HandleTracking( ADMTrackerRef tracker )
{
	ASPoint	mouse_point;
	ADM_Access::tracking_suite()->GetPoint( tracker, &mouse_point );

	ComposerUI*			l_dialog		= GetComposerUI();
	const ASRect		control_bounds	= bounds( true );
	ASRect				adjustedBounds	= control_bounds;

	// inset the bounds rectangle...
	adjustedBounds.top    += IMAGE_INSET;
	adjustedBounds.left	  += IMAGE_INSET;
	adjustedBounds.right  -= IMAGE_INSET + 1;
	adjustedBounds.bottom -= IMAGE_INSET + 1;

	// pin mouse to make UI easier to use...
	if( mouse_point.v < adjustedBounds.top )
		mouse_point.v = adjustedBounds.top;
	
	if( mouse_point.h < adjustedBounds.left )
		mouse_point.h = adjustedBounds.left;
	
	if( mouse_point.v > adjustedBounds.bottom )
		mouse_point.v = adjustedBounds.bottom;
		
	if( mouse_point.h > adjustedBounds.right )
		mouse_point.h = adjustedBounds.right;

	// get tracking type... don't double select the color...
	ADMAction actionID = ADM_Access::tracking_suite()->GetAction( tracker );
	if( actionID == kADMButtonUpAction )
		return false;

//	XDEBUG_PRINT2( "mouse coord w/ inset: %d, %d\n", (short)(mouse_point.h - IMAGE_INSET), (short)(mouse_point.v - IMAGE_INSET) );

	// set the control values
	SetPosition( (short)(mouse_point.v - IMAGE_INSET), (short)(mouse_point.h - IMAGE_INSET) );

	// now tell the color selector...
	l_dialog->UpdateZDisplay( RAW_MAX - mX, mY );	

	// force the whole dialog to update, this seems to have the best results while dragging, etc...
	ADM_Access::dialog_suite()->Update( l_dialog->GetNative() );
	return true;
}


// EOF
