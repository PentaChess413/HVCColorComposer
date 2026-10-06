/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Custom_UI.h"
#include "Helpers.h"

#include "ADM_Access.h"


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Defines
//
//////////////////////////////////////////////////////////////////////////////////////////////

enum 
{	
	INDICATOR_HEIGHT = 9, 
	INDICATOR_WIDTH = 7, 
	SCROLL_ARROW_HEIGHT = 16,
	IMAGE_WIDTH_INSET = INDICATOR_WIDTH + 1, 
	INDICATOR_OFFSET = INDICATOR_HEIGHT / 2 - 2,
	TEXT_HEIGHT = 12, 
	MAX_SEARCH_LENGTH = 100
};


ConstSpCharPtr	kComponentLabel_C	= "C:";
ConstSpCharPtr	kComponentLabel_M	= "M:";
ConstSpCharPtr	kComponentLabel_Y	= "Y:";
ConstSpCharPtr	kComponentLabel_K	= "K:";

ConstSpCharPtr	kComponentLabel_L	= "L:";
ConstSpCharPtr	kComponentLabel_a	= "a:";
ConstSpCharPtr	kComponentLabel_b	= "b:";


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
//////////////////////////////////////////////////////////////////////////////////////////////

CustomUI::CustomUI( SelectedColorRef p_selected_color, const char* p_name, ASInt32 p_dialog_id ) :
	mSelectedColorRef( p_selected_color ),
	mSelectedColor( NULL ),
	mIsPrintSafe( true ),
	mIsWebSafe( true ),
	mPrintWarning( NULL ), 
	mWebWarning( NULL ),
	mColorList( NULL ),
	mColorSlider( NULL ),
	mBookPopup( NULL ),
	mColorLibraries( NULL )
{
	if( !Create( mSelectedColorRef.GetPluginRef(), p_name, p_dialog_id ) )
		return;

	SetDialogFinishedCallback( OK );
	SetDialogFinishedCallback( PICKER );

	mSelectedColor = new Selected_Color_Display( *this, COLOR_DISPLAY, mSelectedColorRef );
	mPrintWarning  = new Warning_Color_Display(  *this, PRINTABLE_WARNING_ICON, mPrintColor );
	mWebWarning    = new Warning_Color_Display(  *this, WEB_COLOR_WARNING_ICON, mWebColor );
	
	// add a key handler to this dialog, these keys go to the color list object...
	ADM_Access::dialog_suite()->SetTrackProc( GetNative(), DialogTrackProc );

	ADMActionMask mask = ADM_Access::dialog_suite()->GetMask( GetNative() );
	ADM_Access::dialog_suite()->SetMask( GetNative(), mask | kADMKeyStrokeMask );

	mColorList     = new MCColorList( *this, COLOR_LIST );
	mColorSlider   = new MCColorSlider( *this, COLOR_SLIDER );
	
	// we need to get the current book from the MCColorLibraries object !!@
	mColorLibraries = new MCColorLibraries;
	if( mColorLibraries )
		mBookPopup = new MCBookPopup( *this, BOOK_MENU, *mColorLibraries );
}


CustomUI::~CustomUI()
{
	delete mSelectedColor;
	delete mPrintWarning;
	delete mWebWarning;
	delete mColorList;
	delete mColorSlider;
	delete mBookPopup;
	delete mColorLibraries;
	
	// do not delete mCurrentBook as we don't own it.
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Methods
//
//////////////////////////////////////////////////////////////////////////////////////////////

ASInt32 CustomUI::run()
{
	// !!@ get the last used color book... and try to load it...

	// reinitialize the warnings
//	mIsPrintSafe = true;
//	mIsWebSafe   = true;
	
	// make sure to initially select a color...
	SelectNewColor( mSelectedColorRef, kUpdateUnspecifiedFlag );
	
	return base::run();
}

//////////////////////////////////////////////////////////////////////////////////////////////

void CustomUI::SelectNewColor( MCColorRef newColor, SelectionFlags flags )
{
	if( flags & kUpdateFromPaletteFlag )
	{
		// if the color came from the swatch list (or palette) simply set that as the current color
		mSelectedColorRef.SetColor( newColor );
	}
	else
	{
		// when we select a new color we first search for the closest in the
		// selected color book
		MCColorbook* b = mColorLibraries->GetCurrentBook();
		
		if( b )
		{
			MCColorbookIndex result = b->FindClosestColor( newColor );
			
			if( b->IsValidIndex( result ) )
			{
				MCColor* cp = b->GetColor( result );
				if( cp )
					mSelectedColorRef.SetColor( *cp );
				
				// point list to this color
				b->SetSelection( result );
			}
		}
	}

	// get all the numeric displays updated
	HandleColorChange( SKIP_NONE, flags );
}



void CustomUI::HandleColorChange( SkipBlock skipBlock, SelectionFlags flags )
{
	// this updates all the swatches and warnings
	CalculateWarnings();
	
	// Update the selected color display
	InvalidateControl( COLOR_DISPLAY );
	InvalidateControl( COLOR_LIST );
	
//	if( !(flags & kUpdateFromPaletteFlag) )	// no- prevents arrow keys from getting the slider updated...
	InvalidateControl( COLOR_SLIDER );

	// update the text labels	
	UpdateLabels();
}


MCColorbook* CustomUI::GetColorbook()
{
	if( mColorLibraries )
		return mColorLibraries->GetCurrentBook();
	
	return NULL;
}

MCColorSlider* CustomUI::GetColorSlider()
{
	return mColorSlider;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Display and update code -- copied from ComposerUI!!
//
//////////////////////////////////////////////////////////////////////////////////////////////

void CustomUI::UpdateLabels()
{
	MCColorbook* b = mColorLibraries->GetCurrentBook();

	if( b )
	{
		// update text
		char buffer[16];
		short components[4];
		
		// we really should check that this color matches the book's color space... hehe !!@		
		mSelectedColorRef.GetColorNoConvert( components );

		// make sure the labels read properly
		if( b->GetBookSpace() == kCMYKSpace )
		{
			// do CMYK components here
			for( int i = 0; i < 4; i++ )
			{
				::sprintf( buffer, "%d", limit_percent( RAW_MAX - components[i] ) );
				GetControl( COMPONENT_1 + i ).set_text( buffer );
			}
			
			GetControl( COMPONENT_LABEL_1 ).set_text( kComponentLabel_C );
			GetControl( COMPONENT_LABEL_2 ).set_text( kComponentLabel_M );
			GetControl( COMPONENT_LABEL_3 ).set_text( kComponentLabel_Y );
			GetControl( COMPONENT_LABEL_4 ).set_text( kComponentLabel_K );
		}
		else
		{
			// do Lab
			::sprintf( buffer, "%d", limit_percent( components[0] ) );
			GetControl( COMPONENT_1 ).set_text( buffer );
			::sprintf( buffer, "%d", limit_lab( components[1] ) );
			GetControl( COMPONENT_2 ).set_text( buffer );
			::sprintf( buffer, "%d", limit_lab( components[2] ) );
			GetControl( COMPONENT_3 ).set_text( buffer );

			// clear the last component
			GetControl( COMPONENT_4 ).set_text( "" );

			GetControl( COMPONENT_LABEL_1 ).set_text( kComponentLabel_L );
			GetControl( COMPONENT_LABEL_2 ).set_text( kComponentLabel_a );
			GetControl( COMPONENT_LABEL_3 ).set_text( kComponentLabel_b );
			GetControl( COMPONENT_LABEL_4 ).set_text( "" );
		}
	}
}


void CustomUI::CalculateWarnings()
{
	// Should the any of the warnings be updated?
	bool not_safe	= false;
	bool didChange	= false;

	// update web color version of the same color.
	didChange = CalculateWebSafeColor( not_safe );
	UpdateWarning( WEB_COLOR_WARNING_ICON, not_safe, didChange, mIsWebSafe );

	// Update the printable (CMYK) warning
	bool  printable = false;
	short cmyk[4]   = {0};
	if( mSelectedColorRef.GetColor( kCMYKSpace, cmyk, &printable ) )									
	{
		// calculate the CMYK colors
		didChange = !printable && CalculatePrintSafeColor();
		UpdateWarning( PRINTABLE_WARNING_ICON, !printable, didChange, mIsPrintSafe );
	}
}


// Update the color warning controls
void CustomUI::UpdateWarning( ASInt32 pictureID, bool not_safe, bool color_changed, bool& safe_color )
{
	// Change the visibility of the warning
	if( not_safe != safe_color )
	{													
		safe_color = not_safe;	// update state
		ShowHideControl( pictureID, not_safe );
		ShowHideControl( pictureID + 1, not_safe );
	}
	else if( not_safe && color_changed )
	{
		// Update the warning color
		InvalidateControl( pictureID + 1 );
	}
}



// Calculate the print safe color by converting the CMYK color to RGB
bool CustomUI::CalculatePrintSafeColor()
{
	MCColor cmykColor = mSelectedColorRef.GetColorInCMYK();
	
	// check to see if this color is different than the current print safe color...
	bool didChange = !mPrintColor.Compare( cmykColor, true );

	// well did the color change?
	if( didChange )
		mPrintColor = cmykColor;

	return didChange;
}

// Calculate the web safe color by grabbing web RGB
bool CustomUI::CalculateWebSafeColor( bool& notSafe )
{
	MCColor webColor = mSelectedColorRef.GetColorInWeb();
	
	// check to see if the colors are the same
	notSafe = !mSelectedColorRef.Compare( webColor, true );
	
	// check to see if this color is different than the current web safe color...
	bool didChange = !mWebColor.Compare( webColor, true );

	// well did the color change?
	if( didChange )
		mWebColor = webColor;

	return didChange;
}


//////////////////////////////////////////////////////////////////////////////////////////////
ASAPI ASBoolean CustomUI::DialogTrackProc( ADMDialogRef inDialog, ADMTrackerRef inTracker )
{
	CustomUI* me = (CustomUI*)ADM_Access::dialog_suite()->GetUserData( inDialog );
	if( !me || !me->mColorList )
		return false;

	ASBoolean keyClick  = ADM_Access::tracking_suite()->TestAction( inTracker, kADMKeyStrokeAction );
	if( !keyClick )
		return false;
		
	// pass the keystroke to the color list
	if( me->mColorList->HandleKeyClick( inTracker ) )
		return true;
		
	return ADM_Access::dialog_suite()->DefaultTrack( inDialog, inTracker );
	
}


// EOF
