/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include "PhotoShopSDK.h"

#include "MCColorList.h"

#include "Custom_UI.h"
#include "Helpers.h"

#include "admDrawer.h"
#include "admTracker.h"

#define USE_WINDOWS_ROUTINES

// ??? this should be something like the double click or key repeat rate !!@
// this time is in milliseconds...
const ADMTime kColorListSearchTimeout = 1000;	// 1 second time out.	


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
//////////////////////////////////////////////////////////////////////////////////////////////


MCColorList::MCColorList( Dialog_Runner& dialog_runner, const ASInt32 control_id ) :
	base( dialog_runner, control_id ),
	mLastKeyTime( 0 )
{
}



//////////////////////////////////////////////////////////////////////////////////////////////
//
// Methods
//
//////////////////////////////////////////////////////////////////////////////////////////////


void MCColorList::Draw( ADMDrawerRef environment )
{
	// get access to UI
	CustomUI* ui = GetLibrariesUI();
	if( !ui )
		return;

	// get the total bounds
	ASRect colorRect = bounds( true );
		
	// get color book
	MCColorbook* book = ui->GetColorbook();
	if( book )
		book->DrawSwatches( environment, &colorRect );
	else
	{
		// we need to load this error string from the resources !!@
		
		// write that we couldn't find the color libraries...
		ADM_Access::drawing_suite()->DrawTextCentered( environment, "No Color Books found.", &colorRect );
	}
	
	// frame the box -- this really should be it's own function !!@
	{
		ASRGBColor frame_color = {0};	// black
		ADM_Access::drawing_suite()->SetRGBColor( environment, &frame_color );
		ADM_Access::drawing_suite()->DrawSunkenRect( environment, &colorRect );
		
		ASRect insetRect = colorRect;
		++insetRect.top;
		++insetRect.left;
		--insetRect.bottom;
		--insetRect.right;
		ADM_Access::drawing_suite()->DrawRect( environment, &insetRect );
	}

}


ASBoolean MCColorList::HandleTracking( ADMTrackerRef tracker )
{
	// !!@ I guess I could use the event mask for this... duh..
	ASBoolean mouseDown = ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonDownAction );
	ASBoolean mouseDrag = ADM_Access::tracking_suite()->TestAction( tracker, kADMMouseMovedDownAction );

	if( !mouseDown && !mouseDrag )
		return false;
		
	ASPoint	mouse_point;

	ADM_Access::tracking_suite()->GetPoint( tracker, &mouse_point );

	CustomUI*			ui				= GetLibrariesUI();
	ASRect				adjustedBounds	= bounds( true );

	if( !ui )
		return false;

	// get color book
	MCColorbook* book = ui->GetColorbook();
	if( !book )
		return false;
		
	// pin mouse to make UI easier to use...
	if( mouse_point.v < adjustedBounds.top )
		mouse_point.v = adjustedBounds.top;
	
	if( mouse_point.v > adjustedBounds.bottom )
		mouse_point.v = adjustedBounds.bottom;

	// ask the color book if we selected anything...
	MCColorbookIndex result = book->PickSelection( &adjustedBounds, &mouse_point );
	SelectNewColor( result );
	
	return true;
}


bool MCColorList::HandleKeyClick( ADMTrackerRef tracker )
{
	// get access to UI
	CustomUI* ui = GetLibrariesUI();
	if( !ui )
		return false;

	// get color book
	MCColorbook* book = ui->GetColorbook();
	if( !book )
		return false;

	MCColorbookIndex	oldSelection = book->GetSelection();
	bool				handled		 = false;
	ADMChar				key			 = ADM_Access::tracking_suite()->GetVirtualKey( tracker );
	
	switch( key )
	{
		case kADMUpKey:
			--oldSelection.colorIndex;
			
			// go to previous group here
			if( oldSelection.colorIndex < 0 )
			{
				--oldSelection.colorGroup;
				
				// set the color index to the last color in the list...
				oldSelection.colorIndex = book->GetNumColorsInGroup( oldSelection.colorGroup ) - 1;
			}
				
			SelectNewColor( oldSelection );
			handled = true;
			break;
			
		case kADMDownKey:
			++oldSelection.colorIndex;
			
			// go to previous group here
			if( oldSelection.colorIndex >= book->GetNumColorsInGroup( oldSelection.colorGroup ) )
			{
				++oldSelection.colorGroup;
				
				// set the color index to the first color in the list...
				oldSelection.colorIndex = 0;
			}
				
			SelectNewColor( oldSelection );
			handled = true;
			break;
		
		// regular key strokes...  add each character to a search string
		// We clear the search field if too much time has passed since the last keystroke...		
		default:
			handled = HandleSearch( book, tracker, key );
			break;
	}

	return handled;
}


void MCColorList::SelectNewColor( MCColorbookIndex index )
{
	// get access to UI
	CustomUI* ui = GetLibrariesUI();
	if( !ui )
		return;

	// get color book
	MCColorbook* book = ui->GetColorbook();
	if( !book )
		return;

	MCColorbookIndex oldSelection = book->GetSelection();
	
	// here we check to make sure it's a valid selection and it's different than the current selection...
	bool selectionChanged = oldSelection.colorGroup != index.colorGroup || oldSelection.colorIndex != index.colorIndex;
	
	if( book->IsValidIndex( index ) && selectionChanged )
	{
		// oh we did select something, so grab the color and spread the word.
		MCColor* rColor = book->GetColor( index );
		
		if( rColor )
		{
			// pick that color as the selection in the color book...
			book->SetSelection( index );

			// tell the main dialog we just picked a new color
			ui->SelectNewColor( *rColor, kUpdateFromPaletteFlag );
			
			// redraw the list... !!@ we could do better here by only redrawing the changed portions !!@
			ui->InvalidateControl( CustomUI::COLOR_LIST );
		}
	}
}


bool MCColorList::HandleSearch( MCColorbook* book, ADMTrackerRef tracker, ADMChar key )
{
	// !!@ note: this has already been checked in the regular flow, so this is redundant
	if( !book )
		return false;

	// check to see if this key is a number or a letter
	int alnum = ::isalnum( key );
	if( !alnum )
		return false;

#if WIN32 && defined( USE_WINDOWS_ROUTINES )
	ADMTime currentKeyTime = (ADMTime)::GetTickCount();
#else
	// get the time this key came in... -- does this work ??? !!@
	ADMTime currentKeyTime = ADM_Access::tracking_suite()->GetTime( tracker );
#endif
	
	// clear the search field if too much time passed
	if( currentKeyTime > mLastKeyTime + kColorListSearchTimeout )
		mSearch.Clear();
	
	// this seems weird because I'm building a mini string to pass to the name field class... lame, I know..	
	SpChar	miniBuffer[2] = { (SpChar)key, 0 };

	// okay now add the character typed in to the search criteria
	mSearch += miniBuffer;
	
//	XDEBUG_PRINT1( "search string: %s\n", (SpCharPtr)mSearch );
	
	// do the search!
	MCColorbookIndex result = book->FindByName( mSearch );
	SelectNewColor( result );
	
	// reset key time...
	mLastKeyTime = currentKeyTime;
		
	return true;
}



// EOF
