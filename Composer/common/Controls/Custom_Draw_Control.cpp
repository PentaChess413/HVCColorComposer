/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Custom_Draw_Control.h"

#include "admDrawer.h"

void ASAPI Custom_Draw_Control::DrawCallback( ADMItemRef p_control, ADMDrawerRef environment )
{
	try
	{
		Custom_Draw_Control* l_control = GetDrawControlFromNative( p_control );

		if( l_control->visible() )
			l_control->Draw( environment );
	}
	catch(...)
	{
#if WIN32
		::OutputDebugString( "runaway exception caught!\n");
#else
		::DebugStr( "\prunaway exception caught!" );
#endif		
	}
}


void Custom_Draw_Control::Update()
{
	invalidate();
}


void Custom_Draw_Control::frame_draw( ADMDrawerRef environment, ASRect& bounds )
{
	ASRGBColor	frame_color = { 0 };

	ADM_Access::drawing_suite()->SetRGBColor(environment, &frame_color);
	ADM_Access::drawing_suite()->DrawRect(environment, &bounds);
}


void Custom_Draw_Control::draw_frame( ADMDrawerRef environment, ASRect& bounds )
{
	frame_draw(environment, bounds);
	bounds.top += 1;
	bounds.left += 1;
	bounds.bottom -= 1;
	bounds.right -= 1;
}


void Custom_Draw_Control::fill_with_color( ADMDrawerRef environment, const ASRect& bounds, const ASRGBColor& color )
{
	ADM_Access::drawing_suite()->SetRGBColor( environment, &color );
	ADM_Access::drawing_suite()->FillRect( environment, &bounds );
}


// EOF
