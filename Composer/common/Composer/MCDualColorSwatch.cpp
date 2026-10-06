/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "MCDualColorSwatch.h"
#include "Composer_UI.h"


MCDualColorSwatch::MCDualColorSwatch( Dialog_Runner& dialog_runner, const ASInt32 control_id, MCColorRef p_left_color, MCColorRef p_right_color ) :
	base( dialog_runner, control_id, p_left_color, p_right_color )
{
	// start off with the left side selected...
//	m_tracking_right = true;
}


// all we have to do here is frame whichever color is selected and then have the base class draw
void MCDualColorSwatch::Draw( ADMDrawerRef environment )
{
	// draw the selection frame...
	ASRect frameBounds = bounds( true );
	ASRect insetBounds = bounds( true );
	insetBounds.top    += 2;
	insetBounds.left   += 2;
	insetBounds.bottom -= 2;
	insetBounds.right  -= 2;
	
	// actually erase the frame first incase it changed sides...
	ASRect oldClip = {0};
	ADM_Access::drawing_suite()->GetClipRect( environment, &oldClip );
	ADM_Access::drawing_suite()->SubtractClipRect( environment, &insetBounds );
	ADM_Access::drawing_suite()->Clear( environment );
	
	// restore clip
	ADM_Access::drawing_suite()->SetClipRect( environment, &oldClip );
	
	// modify frame
	if( m_tracking_right )
		frameBounds.left = (frameBounds.right - frameBounds.left) / 2 + frameBounds.left - 1;
	else
		frameBounds.right = (frameBounds.right - frameBounds.left) / 2 + 1;
	
	frame_draw( environment, frameBounds );
	

	// now draw the split swatch
	draw_frame( environment, insetBounds );

	ASInt32 old_right = insetBounds.right;
	
	// draw left side
	insetBounds.right = (insetBounds.right - insetBounds.left) / 2 + insetBounds.left;
	m_left_color.Draw( environment, &insetBounds, false );
	
	// draw the right side
	insetBounds.left = insetBounds.right;
	insetBounds.right = old_right;
	m_right_color.Draw( environment, &insetBounds, false );
}



ASBoolean MCDualColorSwatch::HandleTracking( ADMTrackerRef tracker )
{
	ASPoint	mouse_point;

	if( ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonDownAction ) )
	{
		ADM_Access::tracking_suite()->GetPoint( tracker, &mouse_point );

		const ASRect control_bounds = bounds();

		// select whichever side we are on...
		bool rightSide = (control_bounds.right - control_bounds.left) >> 1 < mouse_point.h;
		
		// invalidate the item if the selection changed 
		if( rightSide != m_tracking_right )
			invalidate();
		
		// make sure we stay within our correct half...  the code below checks this again upon reentry...
		m_tracking_right = rightSide;
	}
	else if( ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonUpAction ) )
	{
		ADM_Access::tracking_suite()->GetPoint( tracker, &mouse_point );

		const ASRect	control_bounds = bounds();
		const ASInt32	width = control_bounds.right - control_bounds.left;
		const ASInt32	half_width = width >> 1;

		if( 0 <= mouse_point.v && (m_tracking_right ? half_width : 0) <= mouse_point.h 
		&& control_bounds.bottom - control_bounds.top >= mouse_point.v 
		&& (m_tracking_right ? width : half_width) >= mouse_point.h )
		{
			// if we get a double click select that color...
			if( ADM_Access::tracking_suite()->TestModifier( tracker, kADMDoubleClickModifier ) )
			{
				// tell everyone that we are selecting a new color...
				GetComposerUI()->SelectNewColor( m_tracking_right ? m_right_color : m_left_color );
			}
		}
	}

	return true;
}

// EOF
