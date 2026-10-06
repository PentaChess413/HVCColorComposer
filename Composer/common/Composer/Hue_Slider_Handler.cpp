/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "PhotoShopSDK.h"

#include "Hue_Slider_Handler.h"
#include "Composer_UI.h"

#include "Helpers.h"
#include "Color_Plane_Selector.h"

#include "admDrawer.h"
#include "admTracker.h"


enum 
{	
	INDICATOR_HEIGHT = 7, 
	INDICATOR_WIDTH = 5,
	
	IMAGE_WIDTH_INSET = INDICATOR_WIDTH + 1, 
	IMAGE_HEIGHT_INSET = INDICATOR_HEIGHT / 2 
};



Hue_Slider_Handler::Hue_Slider_Handler( ComposerUI& dialog_runner, const ASInt32 control_id, DisplayPixelsProc displayProc ) :
	base( dialog_runner, control_id ), 
	alt_base( displayProc )
{
}



void Hue_Slider_Handler::Draw( ADMDrawerRef environment )
{
#if WIN32
	ASRect l_bounds = bounds( true );

	// on Windows we need to get the control's window DC
	ASWindowRef winRef = GetWindowRef();
	HDC dc = GetDC( (HWND)winRef );

	// Draw the flag image -erhm, the spectrum...
	ASPoint	image_origin = { IMAGE_WIDTH_INSET, IMAGE_HEIGHT_INSET };
	copy_bit_map( image_origin, dc );		

	ReleaseDC( NULL, dc );
	
	// we need to erase the sides of the slider (and not the whole rectangle)
	ASRect eraseBounds = l_bounds;
	
	// erase left side
	eraseBounds.right = eraseBounds.left + INDICATOR_WIDTH;
	ADM_Access::drawing_suite()->ClearRect( environment, &eraseBounds );
	
	// erase right side
	eraseBounds = l_bounds;
	eraseBounds.left = eraseBounds.right - INDICATOR_WIDTH;
	ADM_Access::drawing_suite()->ClearRect( environment, &eraseBounds );

	ASRect	draw_bounds = { l_bounds.left, l_bounds.top + int_value() };
#else
	// access the item bounds in the dialog's client coordinates
	ASRect	l_bounds = bounds();

	// on the Mac everything is double buffered.
	ADM_Access::drawing_suite()->Clear(environment);

	ASPoint	image_origin = { l_bounds.top + IMAGE_HEIGHT_INSET, l_bounds.left + IMAGE_WIDTH_INSET };

	copy_bit_map( image_origin, NULL );		// Draw the flag image

	ASRect	draw_bounds = { l_bounds.top + int_value(), l_bounds.left };
#endif

	// setup position on vertical for arrows
	draw_bounds.bottom = draw_bounds.top + INDICATOR_HEIGHT - 1;
//	draw_bounds.bottom = draw_bounds.top + INDICATOR_HEIGHT;
	
	// Draw the left indicator arrow
	draw_bounds.right = draw_bounds.left + INDICATOR_WIDTH;
	ADM_Access::drawing_suite()->DrawRightArrow( environment, &draw_bounds );

	// Draw the right indicator arrow
	draw_bounds.right = l_bounds.right;
	draw_bounds.left  = draw_bounds.right - INDICATOR_WIDTH;
	ADM_Access::drawing_suite()->DrawLeftArrow( environment, &draw_bounds );

	// Frame the colors
	draw_bounds.top    = l_bounds.top    + IMAGE_HEIGHT_INSET - 1;
	draw_bounds.left   = l_bounds.left   + IMAGE_WIDTH_INSET - 1;
	draw_bounds.bottom = l_bounds.bottom - IMAGE_HEIGHT_INSET;
	draw_bounds.right  = l_bounds.right  - IMAGE_WIDTH_INSET + 1;
	frame_draw( environment, draw_bounds );
}


ASBoolean Hue_Slider_Handler::HandleTracking( ADMTrackerRef tracker )
{
	ASPoint	mouse_point;

	ADM_Access::tracking_suite()->GetPoint( tracker, &mouse_point );

	ComposerUI*			ui				= GetComposerUI();
	const ASRect		control_bounds	= bounds( true );
	ASRect				adjustedBounds	= control_bounds;
	
	// don't ask me why the top is a bit smaller than the bottom, that's how it's done
	// in the framing code above so I had to duplicate it here...
	adjustedBounds.top    += IMAGE_HEIGHT_INSET;
	adjustedBounds.bottom -= IMAGE_HEIGHT_INSET + 1;

	// pin mouse to make UI easier to use...
	if( mouse_point.v < adjustedBounds.top )
		mouse_point.v = adjustedBounds.top;
	
	if( mouse_point.v > adjustedBounds.bottom )
		mouse_point.v = adjustedBounds.bottom;

	// set the control value
	set_int_value( mouse_point.v - IMAGE_HEIGHT_INSET );

//	XDEBUG_PRINT1( "mouse coord w/ inset: %d\n", (short)(mouse_point.v - IMAGE_HEIGHT_INSET) );
	
	// now tell the color selector...
	if( ui )
	{
		ui->UpdateXYDisplay( RAW_MAX - int_value() );	

		// force the whole dialog to update, this seems to have the best results while dragging, etc...
		ADM_Access::dialog_suite()->Update( ui->GetNative() );
	}
	
	return true;
}

// EOF

