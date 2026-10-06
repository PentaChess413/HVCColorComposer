/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	Source code rewritten by: Alex Lelievre ([PII Redacted]), 2006.
 *
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Quick_Colors_Handler.h"
#include "Composer_UI.h"

#include "admDrawer.h"
#include "admTracker.h"

// Palette display sizes
enum 
{	
	SQUARES_WIDTH = 4, 
	SQUARES_HEIGHT = 3, 
	TRACKING_INSET = 2 
};



Quick_Colors_Handler::Quick_Colors_Handler( ComposerUI& dialog_runner, const ASInt32 control_id ) :
						base( dialog_runner, control_id ), 
						mIndex( 0 )
{
	m_tracking_bounds.top = m_tracking_bounds.left = m_tracking_bounds.right = m_tracking_bounds.bottom = 0;
}



void Quick_Colors_Handler::SetCurrentColor( MCColorRef colorRef )
{
	mColors[mIndex] = colorRef;
}


MCColor* Quick_Colors_Handler::GetCurrentColor()
{
	return &mColors[mIndex];
}

void Quick_Colors_Handler::SetColor( MCColorRef colorRef, short index )
{
	// range check
	if( index < 0 || index >= kNumQuickColors )
		return;

	mColors[index] = colorRef;
}

MCColor* Quick_Colors_Handler::GetColor( short index )
{
	// range check
	if( index < 0 || index >= kNumQuickColors )
		return NULL;

	return &mColors[index];
}



void Quick_Colors_Handler::SetSelection( short index )
{
	// range check
	if( index < 0 || index >= kNumQuickColors )
		return;
	
	mIndex = index;
}

short Quick_Colors_Handler::GetSelection()
{
	return mIndex;
}



void Quick_Colors_Handler::Draw(ADMDrawerRef environment)
{
	ASRect	target_bounds = { 0 };

	ADM_Access::drawing_suite()->GetBoundsRect(environment, &target_bounds);

	ASInt32		height			= target_bounds.bottom - target_bounds.top;
	ASInt32		width			= target_bounds.right - target_bounds.left;
	ASInt32		original_left	= target_bounds.left;
	short		index			= 0;

#if WIN32
	// first create a sliver and then loop thru erasing
	ASInt32  offset = width / SQUARES_WIDTH;
	ASRect eraseRect = target_bounds;
	eraseRect.right = eraseRect.left + TRACKING_INSET;

	// erase in between the blocks...
	for( int i = 0; i < SQUARES_WIDTH; i++ )
	{		
		ASRect otherSide = eraseRect;
		
		otherSide.left  += offset - TRACKING_INSET;
		otherSide.right += offset - TRACKING_INSET;
		
		// erase one side and then the other
		ADM_Access::drawing_suite()->ClearRect( environment, &eraseRect );
		ADM_Access::drawing_suite()->ClearRect( environment, &otherSide );
		
		// move rectangle to next position...
		eraseRect.left += offset;
		eraseRect.right += offset;
	}	
	
	// now do the other direction...  if I was Orson, I would have this so tightly rolled no one could read it	
	offset = height / SQUARES_HEIGHT;
	eraseRect = target_bounds;
	eraseRect.bottom = eraseRect.top + TRACKING_INSET;

	// erase in between the blocks...
	for( int i = 0; i < SQUARES_HEIGHT; i++ )
	{		
		ASRect otherSide = eraseRect;
		
		otherSide.top    += offset - TRACKING_INSET;
		otherSide.bottom += offset - TRACKING_INSET;
		
		// erase one side and then the other
		ADM_Access::drawing_suite()->ClearRect( environment, &eraseRect );
		ADM_Access::drawing_suite()->ClearRect( environment, &otherSide );
		
		// move rectangle to next position...
		eraseRect.top    += offset;
		eraseRect.bottom += offset;
	}	

#else
	// the mac is double buffered so this doesn't apply -- just erase all
	ADM_Access::drawing_suite()->Clear( environment );
#endif
	
	// Calculate the color block size
	height /= SQUARES_HEIGHT;						
	width /= SQUARES_WIDTH;
	for( int i = 0; i < SQUARES_HEIGHT; ++i, target_bounds.top = target_bounds.bottom )
	{														
		// Draw each row of colors -- Calculate the color block bounds
		target_bounds.bottom = target_bounds.top + height;		
		target_bounds.left = original_left;
		
		for( int j = 0; j < SQUARES_WIDTH; ++j, target_bounds.left = target_bounds.right, ++index )
		{											
			// Draw each color -- Calculate the color block bounds
			target_bounds.right = target_bounds.left + width;
			
			// Is this color selected ?
			if( mIndex == index )		
				frame_draw( environment, target_bounds );	// Draw the selection rect
#if WIN32
			ASRect	color_bounds = { target_bounds.left + 2, target_bounds.top + 2,
									 target_bounds.right - 2, target_bounds.bottom - 2 };
#else
			ASRect	color_bounds = { target_bounds.top + 2, target_bounds.left + 2,
										target_bounds.bottom - 2, target_bounds.right - 2 };
#endif
			// Draw the color block
			mColors[index].Draw( environment, &color_bounds );
		}
	}
}



ASBoolean Quick_Colors_Handler::HandleTracking( ADMTrackerRef tracker )
{
	ASPoint	mouse_point;

	if( ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonDownAction ) )
	{
		ADM_Access::tracking_suite()->GetPoint(tracker, &mouse_point);

		const ASRect	control_bounds = bounds();
		ASInt32			height = control_bounds.bottom - control_bounds.top;
		ASInt32			width  = control_bounds.right - control_bounds.left;

		height /= SQUARES_HEIGHT;
		width /= SQUARES_WIDTH;
		mouse_point.v /= height;
		mouse_point.h /= width;

		// Calculate the color block bounds
		m_tracking_bounds.top = height * mouse_point.v + TRACKING_INSET;
		m_tracking_bounds.left = width * mouse_point.h + TRACKING_INSET;
		m_tracking_bounds.bottom = m_tracking_bounds.top + height - (TRACKING_INSET << 1);
		m_tracking_bounds.right = m_tracking_bounds.left + width - (TRACKING_INSET << 1);
		mIndex = (short)(mouse_point.v * SQUARES_WIDTH + mouse_point.h);
	}
	else if (ADM_Access::tracking_suite()->TestAction(tracker, kADMButtonUpAction))
	{
		ADM_Access::tracking_suite()->GetPoint(tracker, &mouse_point);

		if (m_tracking_bounds.top <= mouse_point.v && m_tracking_bounds.left <= mouse_point.h &&
			m_tracking_bounds.bottom >= mouse_point.v && m_tracking_bounds.right >= mouse_point.h)
		{
			invalidate();
			
			GetComposerUI()->SelectNewColor( mColors[mIndex], kUpdateFromQuickColorFlag );
		}
	}

	return true;
}
