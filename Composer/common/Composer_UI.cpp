/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	Source code rewritten by: Alex Lelievre [PII Redacted], 2006.
 *
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Composer_UI.h"

#include "Check_Box_Handler.h"
#include "Color_Flag_Handler.h"
#include "Hue_Slider_Handler.h"
#include "Quick_Colors_Handler.h"

#include "Warning_Color_Display.h"
#include "Split_Color_Display_Handler.h"
#include "Palette_Display_Handler.h"
#include "Disclosure_Triangle.h"
#include "Scripting.h"

#include "MCLicense.h"
#include "MCGotoURL.h"
#include "MCEditText.h"

#include "admItem.h"
#include "admBasic.h"



enum 
{	
	MIN_STEP_COUNT = 20, 
	MAX_STEP_COUNT = 40
};

enum 
{	
	BUFFER_LENGTH = 32
};


const ASInt32 kSortLabels   = 16300;
const ASInt32 kSortFormat   = 16400;


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
//////////////////////////////////////////////////////////////////////////////////////////////

ComposerUI::ComposerUI( SelectedColorRef selected_color, const char* p_name, ASInt32 p_dialog_id ) :
	mCPSelector( NULL ),
	mSplit( NULL ),
	mPalette( NULL ),
	mTri1( NULL ),
	mTri2( NULL ),
	mSelectedColorRef( selected_color ),
	mPaletteExists( false ),
	mPaletteShowing( false ),	// !!@ this is an assumption that must be met!
	mPaletteControlsShowing( false ),
	mControlsHeight( 0 ),
	mState( NULL ),
	mLicenseData( NULL )
{
	// Create the dialog... or do nothing if we failed...
	if( !Create( mSelectedColorRef.GetPluginRef(), p_name, p_dialog_id ) )
		return;

	// both of these buttons finish our picker dialog and close it
	SetDialogFinishedCallback( Picker_CustomLibraries );
	SetDialogFinishedCallback( REGISTER_BUTTON );
	
	// this object handles ALL the color displays except for the palette controls and palette
	mCPSelector = new ColorPlaneSelector( *this, mSelectedColorRef, COLOR_COMPONENT_RADIO_START, COLOR_COMPONENT_RADIO_END );

	// palette controls
#if !PROVERSION	
	mSplit   = new Split_Color_Display_Handler( *this, GENERATE_SPLIT_SWATCH, m_palette_source, m_palette_selection );
#endif

	mPalette = new Palette_Display_Handler( *this, PALETTE_PALETTE, mSelectedColorRef.GetMCLibRef() );
	
	// default the first seperator to always ??? show the palette controls
	mTri1 = new Disclosure_Triangle( *this, FIRST_SEPARATOR_TOGGLE, &ComposerUI::HandleSeparator1 );
	if( mTri1 )
		mTri1->set_bool_value( true );
	
	// we initially don't show the palette and disable that toggle
	mTri2 = new Disclosure_Triangle( *this, SECOND_SEPARATOR_TOGGLE, &ComposerUI::HandleSeparator2 );
	if( mTri2 )		
		mTri2->Enable( false );

	// hook up the notify callback to our palette generator...
	GetControl( GENERATE_CREATE_PALETTE_BUTTON ).set_proc( ButtonCallback );
	GetControl( MASTER_COLORS_LOGO ).set_proc( ButtonCallback );
	GetControl( HELP_BUTTON ).set_proc( ButtonCallback );
	
	// get tracking events from the scrollbar
	GetControl( PALETTE_SCROLL_BAR ).set_proc( ScrollNotifyCB );
	
	// set the contrast text to bold
	GetControl( GENERATE_SPLIT_CONTRAST ).SetFont( kADMBoldDialogFont );

#if PROVERSION
	// create the necessary controls...
	mRangeSelector = new MCRangeSelector( *this, GENERATE_RANGE_RADIO, GENERATE_AUTO_RANGE_RADIO );
	
	// hook up the notify callback to save palette button...
	GetControl( PALETTE_SAVE_BUTTON ).set_proc( ButtonCallback );

	GetControl( PALETTE_ORDER_0 ).set_proc( ButtonCallback );
	GetControl( PALETTE_ORDER_1 ).set_proc( ButtonCallback );
	GetControl( PALETTE_ORDER_2 ).set_proc( ButtonCallback );
	GetControl( PALETTE_ORDER_3 ).set_proc( ButtonCallback );
	GetControl( PALETTE_ORDER_4 ).set_proc( ButtonCallback );
	GetControl( PALETTE_ORDER_5 ).set_proc( ButtonCallback );

	GetControl( PALETTE_LABEL ).SetFont( kADMBoldDialogFont );
	GetControl( PALETTE_CONTRAST ).SetFont( kADMBoldDialogFont );
	GetControl( PALETTE_CONTRAST1 ).SetFont( kADMBoldDialogFont );
	GetControl( PALETTE_CONTRAST2 ).SetFont( kADMBoldDialogFont );
	GetControl( PALETTE_RESULT ).SetFont( kADMBoldDialogFont );
#endif
	
	// the area from the bottom of the top arrow to the top of the bottom arrow	will
	// be considered the palette controls area.  This is the amount that the dialog is adjusted by.
	ASRect topRect = GetControl( FIRST_SEPARATOR_TOGGLE ).bounds();
	ASRect botRect = GetControl( SECOND_SEPARATOR_TOGGLE ).bounds();
	
	// don't mess with this value, it's used for restore...
	mControlsHeight = botRect.top - topRect.bottom;
	
	// the state gets restored right before we run the dialog...
	// NOTE: this is also where the okay button will get disabled if we run out of clicks...
}


// destructor...
ComposerUI::~ComposerUI()
{
	delete mState;
	
	// now destroy all the objects
//	delete mCPSelector;
	delete mSplit;
	delete mPalette;
	delete mTri1;
	delete mTri2;
	
#if PROVERSION
	delete mRangeSelector;
#endif	
}


ASInt32 ComposerUI::run()
{
	// go ahead and load the state data...
	mState = new MCPrefs( *this );
	if( mState )
		mState->RestoreState();

	// this enables or disables certain UI	
	// CheckLicense();

	// select the original color to start
	SelectNewColor( mSelectedColorRef );

	return base::run();
}


// should  be called right before this object is destroyed...
void ComposerUI::SaveState( bool decrementClicksLeft )
{
	if( mState )
		mState->SaveState( decrementClicksLeft );
}


void ComposerUI::ResetClicksLeft()
{
	if( mState )
		mState->ResetClicksLeft();
}


void ComposerUI::SetClicksLeftUI( short numClicksLeft )
{
	char buffer[16];
	
	::sprintf( buffer, "%d", numClicksLeft );
	GetControl( CLICK_COUNT ).set_text( buffer );
}

/*
void ComposerUI::SetLicenseDataRef( void* licenseData )
{
	mLicenseData = licenseData;
}
*/

void ComposerUI::CheckLicense()
{
	// see if we are purchased or not
	bool purchased = true;
	
	/*if( mLicenseData )
		purchased = ::LicenseValid( mLicenseData );*/
		
	// now deal with the UI
	if( purchased )
	{
		// hide clicks left
		GetControl( CLICK_COUNT ).set_visible( false );
		GetControl( CLICK_COUNT_LABEL ).set_visible( false );
		
		// we swap the purchase button with a help button
		GetControl( REGISTER_BUTTON ).set_visible( false );
		GetControl( HELP_BUTTON ).set_visible( true );
		
		// ensure that the okay button is enabled!!  
		// it could be disabled from running out of clicks before this, doh!
		GetControl( PICKER_OK ).Enable( true );
	}
	else
	{
		// show the clicks left
		GetControl( CLICK_COUNT ).set_visible( true );
		GetControl( CLICK_COUNT_LABEL ).set_visible( true );
		
		// we swap the help button with the purchase button
		GetControl( REGISTER_BUTTON ).set_visible( true );
		GetControl( HELP_BUTTON ).set_visible( false );
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////
//
// Palette stuff -- move these to the MCRangeSelector object... !!@
//
//////////////////////////////////////////////////////////////////////////////////////////////


// Update the palette contrast controls
void ComposerUI::UpdatePaletteContrast( SelectionFlags flags )
{
	// in the Pro Version this is handled by the Range selector object (which I know is kinda weird)
#if PROVERSION
	if( mRangeSelector )
		mRangeSelector->UpdatePaletteContrast( flags );
#else	
	if( flags & kUpdateFromPaletteFlag )
		UpdateSplitSwatch( GENERATE_SPLIT_SWATCH, m_palette_source, m_palette_selection );
#endif
}


// Update the specified split swatch controls -- this is only used in the Standard version, look inside MCRangeSelector
void ComposerUI::UpdateSplitSwatch( short swatch_item, const HVCTrip& first_color, const HVCTrip& second_color )
{
	// Update the control
	if( get_item( swatch_item ) )
		get_item( swatch_item )->invalidate();						

	// Calculate the split swatch distance - truncate floating point data
	ASInt32	contrast = (ASInt32)Palette_Display_Base::CalculateDistance( first_color, second_color );	

	// Limit it to steps of 5
	contrast += 2;												
	contrast /= 5;
	contrast *= 5;
	
	// Stupid assumption here that the contrast item is ONE plus the swatch!!  geez !!@
	Control_Interface	text_display( get_native_item( swatch_item + 1 ) );
	char				buffer[BUFFER_LENGTH] = {0};
	
	// now we go back to floating point...
	ADM_Access::basic_suite()->ValueToString( (float)contrast, buffer, BUFFER_LENGTH, kADMNoUnits, 0, false );

	// Update the text control
	text_display.set_text( buffer );								
}




// in the pro version most of this is handled by the range selector...
void ComposerUI::CreatePalette( bool setColors )
{
	Palette_Display_Handler* thePalette = dynamic_cast<Palette_Display_Handler*>( mPalette );
	if( !thePalette )
		return;

#if PROVERSION
	// let the range selector handle the color specifications...
	if( mRangeSelector )
		mRangeSelector->CreatePalette( thePalette, setColors );
#else
	// set the source and selection to the currently selected color...
	if( setColors )
		m_palette_source = m_palette_selection = mSelectedColorRef;
	
	int	step_delta = depth() * (MAX_STEP_COUNT - MIN_STEP_COUNT);
	
	// Create the palette generator
	thePalette->generate_palette( step_delta / PERCENT + MIN_STEP_COUNT, m_palette_source );
#endif
	
	// make sure to get the contrast numbers updated
	UpdatePaletteContrast( kUpdateFromPaletteFlag );

	mPaletteExists = true;

	// enable the second toggle and make sure the palette is now visible
	Control_Runner*	paletteToggle = get_item( SECOND_SEPARATOR_TOGGLE );
	if( !paletteToggle )
		return;		
	
	paletteToggle->Enable( true );
	
	// setup the scrollbar
	InitializeScrollbar();
	
	// make the palette redraw...
	thePalette->invalidate();
}



//////////////////////////////////////////////////////////////////////////////////////////////
//
// cover routines to the color selector and other stuff
//
//////////////////////////////////////////////////////////////////////////////////////////////

void ComposerUI::SelectNewColor( MCColorRef newColor, SelectionFlags flags )
{
	if( mCPSelector )
		mCPSelector->SelectNewColor( newColor, flags );
	
#if PROVERSION
	if( mRangeSelector )
		mRangeSelector->SelectNewColor( newColor, flags );
#endif
	
	// did this color come from the palette? standard version only...
	if( flags & kUpdateFromPaletteFlag )
		m_palette_selection = newColor;

	UpdatePaletteContrast( flags );
}


void ComposerUI::UpdateXYDisplay( ASInt32 z )
{
	if( mCPSelector )
		mCPSelector->UpdateXYDisplay( z );
}


void ComposerUI::UpdateZDisplay( ASInt32 x, ASInt32 y )
{
	if( mCPSelector )
		mCPSelector->UpdateZDisplay( x, y );
}
		

void ComposerUI::SetQuickColor( MCColorRef colorRef, short index )
{
	if( mCPSelector )
		mCPSelector->SetQuickColor( colorRef, index );
}


MCColor* ComposerUI::GetQuickColor( short index )
{
	if( !mCPSelector )
		return NULL;
		
	return mCPSelector->GetQuickColor( index );
}


void ComposerUI::SetQuickColorSelection( short index )
{
	if( mCPSelector )
		mCPSelector->SetQuickColorSelection( index );
}


short ComposerUI::GetQuickColorSelection()
{
	if( !mCPSelector )
		return NULL;
		
	return mCPSelector->GetQuickColorSelection();
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Mostly pro version stuff...
//
//////////////////////////////////////////////////////////////////////////////////////////////


void ComposerUI::SetPaletteColor( MCColorRef colorRef, short index )
{
	// only index 0 and 1 are valid
	if( index == 0 )
		m_palette_source = colorRef;
	else
		m_palette_selection = colorRef;
		
#if PROVERSION
	if( mRangeSelector )
		mRangeSelector->SetPaletteColor( colorRef, index );
#endif
}

MCColor* ComposerUI::GetPaletteColor( short index )
{
#if PROVERSION
	if( mRangeSelector )
		return mRangeSelector->GetPaletteColor( index );

	return NULL;	
#else
	// only index 0 and 1 are valid
	if( index == 0 )
		return &m_palette_source;
	
	
	return &m_palette_selection;
#endif
}


void ComposerUI::SetPaletteSelection( MCColorRef colorRef )
{
	// only index 0 and 1 are valid
#if PROVERSION
	if( mRangeSelector )
		mRangeSelector->SetPaletteSelection( colorRef );
#endif
}


MCColor* ComposerUI::GetPaletteSelection()
{
	// only index 0 and 1 are valid
#if PROVERSION
	if( mRangeSelector )
		return mRangeSelector->GetPaletteSelection();
#endif
	return NULL;
}


short ComposerUI::GetSelectedRange()
{
#if PROVERSION
	if( mRangeSelector )
		return mRangeSelector->GetSelectedRange();
#endif

	return 0;
}


void ComposerUI::SetSelectedRange( short selectedRadio )
{
#if PROVERSION
	if( mRangeSelector )
		mRangeSelector->SetSelectedRange( selectedRadio );
#endif
}


unsigned char ComposerUI::GetSelectedSplit()
{
#if PROVERSION
	if( mRangeSelector )
		return mRangeSelector->GetSelectedSplit();
#endif

	return 0;
}


void ComposerUI::SetSelectedSplit( unsigned char selectedSplit )
{
#if PROVERSION
	if( mRangeSelector )
		mRangeSelector->SetSelectedSplit( selectedSplit );
#endif
}


void ComposerUI::SetSplitColor( MCColorRef colorRef, short index )
{
#if PROVERSION
	if( mRangeSelector )
		mRangeSelector->SetSplitColor( colorRef, index );
#endif
}


MCColor* ComposerUI::GetSplitColor( short index )
{
#if PROVERSION
	if( mRangeSelector )
		return mRangeSelector->GetSplitColor( index );
#endif
	return NULL;
}


void ComposerUI::SetSortOrder( ASInt8 whichSortIcon )
{
#if PROVERSION
	if( mRangeSelector )
		mRangeSelector->SetPaletteSortOrder( whichSortIcon );
	
	// do the sort order text update here
	char sortOrder[BUFFER_LENGTH]       = {0};
	char sortFormat[BUFFER_LENGTH]      = {0};
	char finalString[BUFFER_LENGTH * 2] = {0};
	
	ASErr error = ADM_Access::basic_suite()->GetIndexString( mSelectedColorRef.GetPluginRef(), kSortLabels, whichSortIcon, sortOrder, BUFFER_LENGTH );
	if( !error )
	{
		error = ADM_Access::basic_suite()->GetIndexString( mSelectedColorRef.GetPluginRef(), kSortFormat, 0, sortFormat, BUFFER_LENGTH );
		if( !error )
		{
			// stuff the sort order into the string and then update the UI
			::sprintf( finalString, sortFormat, sortOrder );
			
			GetControl( PALETTE_ORDER_LABEL ).set_text( finalString );
		}
	}

	// re-setup the scrollbar
	InitializeScrollbar();
	
	InvalidateControl( PALETTE_SORT_FRAME );
#endif
}


ASInt8 ComposerUI::GetSortOrder()
{
#if PROVERSION
	if( mRangeSelector )
		return mRangeSelector->GetPaletteSortOrder();
#endif
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// palette parameters
//
//////////////////////////////////////////////////////////////////////////////////////////////


short ComposerUI::GetRange()
{
#if PROVERSION
	if( mRangeSelector )
		return mRangeSelector->GetRange();
#endif

	return kPaletteDefault_Range;
}

void ComposerUI::SetRange( short n )
{
#if PROVERSION
	if( mRangeSelector )
		mRangeSelector->SetRange( n );
#endif
}

short ComposerUI::GetDepth()
{
#if PROVERSION
	if( mRangeSelector )
		return mRangeSelector->GetDepth();
#endif

	return kPaletteDefault_Depth;
}

void ComposerUI::SetDepth( short n )
{
#if PROVERSION
	if( mRangeSelector )
		mRangeSelector->SetDepth( n );
#endif
}

float ComposerUI::GetAccuracy()
{
#if PROVERSION
	if( mRangeSelector )
		return mRangeSelector->GetAccuracy();
#endif

	return kPaletteDefault_Accuracy;
}

void ComposerUI::SetAccuracy( float n )
{
#if PROVERSION
	if( mRangeSelector )
		mRangeSelector->SetAccuracy( n );
#endif
}

float ComposerUI::GetProportion()
{
#if PROVERSION
	if( mRangeSelector )
		return mRangeSelector->GetProportion();
#endif

	return kPaletteDefault_Proportion;
}

void ComposerUI::SetProportion( float n )
{
#if PROVERSION
	if( mRangeSelector )
		mRangeSelector->SetProportion( n );
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// checkboxes and toggles
//
//////////////////////////////////////////////////////////////////////////////////////////////

bool ComposerUI::GetToggleState( ToggleArrow whichOne )
{
	// enable the second toggle and make sure the palette is now visible
	Control_Runner*	paletteToggle = get_item( whichOne == kSeparator1 ? FIRST_SEPARATOR_TOGGLE : SECOND_SEPARATOR_TOGGLE );
	if( !paletteToggle )
		return false;		

	return paletteToggle->bool_value();
}


void ComposerUI::SetToggleState( ToggleArrow whichOne, bool state )
{
	// check to see that the current state doesn't match the passed in state...
	if( state != GetToggleState( whichOne ) )
	{
		// turn the toggle if it isn't already turned...
		whichOne == kSeparator1 ? HandleSeparator1() : HandleSeparator2();
	}
}



bool ComposerUI::GetWebCheckboxState()
{
	if( mCPSelector )
		return mCPSelector->GetWebCheckboxState();
	
	return false;
}

void ComposerUI::SetWebCheckboxState( bool c )
{
	if( mCPSelector )
		mCPSelector->SetWebCheckboxState( c );
}

	
bool ComposerUI::GetMunsellCheckboxState()
{
	if( mCPSelector )
		return mCPSelector->GetMunsellCheckboxState();
	
	return false;
}

void ComposerUI::SetMunsellCheckboxState( bool c )
{
	if( mCPSelector )
		mCPSelector->SetMunsellCheckboxState( c );
}



short ComposerUI::GetSelectedColorPlane()
{
	if( mCPSelector )
		return mCPSelector->GetSelectedColorPlane();
		
	return HSB_HUE_RADIO;
}


void ComposerUI::SetSelectedColorPlane( short button )
{
	if( mCPSelector )
		mCPSelector->SetSelectedColorPlane( button );
}


void ComposerUI::DisableOkayButton()
{
	// disable the okay button
	GetControl( PICKER_OK ).Enable( false );
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// internal stuff
//
//////////////////////////////////////////////////////////////////////////////////////////////

void ComposerUI::HandleSeparator1()
{
	Control_Interface ci = GetControl( FIRST_SEPARATOR_TOGGLE );
	
	// update control -- toggle it
	ci.set_bool_value( !ci.bool_value() );
	ci.invalidate();
	
	// now show or hide the palette controls
	ShowHidePaletteControls( ci.bool_value() );
	
#if PROVERSION
	// we need to make sure the right controls are showing
	if( mRangeSelector && ci.bool_value() )
	{
		mRangeSelector->UpdateControls();
	}
#endif	
}

void ComposerUI::HandleSeparator2()
{
	Control_Interface ci = GetControl( SECOND_SEPARATOR_TOGGLE );
	
	// update control -- toggle it
	ci.set_bool_value( !ci.bool_value() );
	ci.invalidate();

	ShowHidePalette( ci.bool_value() );
}


// call this to setup the scroll bar after creating a new palette
void ComposerUI::InitializeScrollbar()
{
	Palette_Display_Handler* thePalette = dynamic_cast<Palette_Display_Handler*>( mPalette );
	if( !thePalette )
		return;

	// How many rows are in the new palette? -- this is the total height...
	ASInt32	numElements = thePalette->GetSize();
	ASInt32	rows		= numElements / PALETTE_COLUMNS;	
	
	// this checks to see if there are any straglng items and if so adds a row for them to be displayed in...
	if( rows * PALETTE_COLUMNS < numElements )
		++rows;
	
	// initialize the scroll bar
	Control_Interface scrollBar = GetControl( PALETTE_SCROLL_BAR );

	// we should at least have constants for these !!@
	scrollBar.SetSmallIncrement( 1 );	
	scrollBar.SetLargeIncrement( PALETTE_ROWS - 1 );
	scrollBar.set_min_int_value( 0 );
	
	// we need to compensate if there are more rows than the number of rows we can show
	if( rows > PALETTE_ROWS )
	{
		scrollBar.Enable( true );
		scrollBar.set_max_int_value( rows - PALETTE_ROWS );
	}
	else
	{
		// there is nothing to scroll here, everything fits - so disable the scroller
		scrollBar.Enable( false );
		scrollBar.set_max_int_value( 0 );		
	}
		
	// set initial position
	scrollBar.set_int_value( 0 );
}



// reveal/hide the palette -- 
// !!@ NOTE: this assumes the dialog IS NOT showing the palette initially
void ComposerUI::ShowHidePalette( bool shide )
{
	// this one just shrinks or expands the bottom of the dialog to 
	// "reveal or hide" the palette by simply clipping it out of view
	ASRect dialogRect = {0};
	GetBoundsRect( &dialogRect, true );
	
#if PROVERSION
	// get the bottom of the save frame as this is what we adjust the dialog size with
	ASRect paletteRect = GetControl( PALETTE_SAVE_FRAME ).bounds();
	
	// now we need a real top value...
	ASRect temp = GetControl( SECOND_SEPARATOR_TOGGLE ).bounds();
	paletteRect.top = temp.bottom;
#else
	// get height of the palette as this is what we adjust the dialog size with
	ASRect paletteRect = GetControl( PALETTE_PALETTE ).bounds( true );
#endif
	
	// if we should show the palette, grow the dialog (btw, true == show)
	if( shide )
		dialogRect.bottom += paletteRect.bottom - paletteRect.top;
	else
		dialogRect.bottom -= paletteRect.bottom - paletteRect.top;
	
	Size( dialogRect.right - dialogRect.left, dialogRect.bottom - dialogRect.top );
	
	// keep the state -- we may not need it...
	mPaletteShowing = shide;
}


// reveal/hide the controls for the palette -- 
// NOTE: this assumes that the controls ARE showing initially

// this one is more complex-  we show or hide all the palette controls,
// then we move the palette, scrollbar and lower toggle items to account 
// for the hidden controls and also shrink or expand the dialog bottom a bit...
void ComposerUI::ShowHidePaletteControls( bool shide )
{
	ASRect dialogRect = {0};
	GetBoundsRect( &dialogRect, true );

	// the area from the bottom of the top arrow to the top of the bottom arrow	will
	// be considered the palette controls area.  This is the amount that the dialog is adjusted by.
	if( shide )
		dialogRect.bottom += mControlsHeight;
	else
		dialogRect.bottom -= mControlsHeight;
	
	// if we should show the controls, grow the dialog (btw, true == show)
	Size( dialogRect.right - dialogRect.left, dialogRect.bottom - dialogRect.top );
	
	// show/hide all the items -- NOTE: this assumes the items are contiguous !!@
	for( int x = GENERATE_START; x <= GENERATE_END; x++ )
		GetControl( x ).set_visible( shide );

	// move all the palette controls up or down
	for( int x = PALETTE_START; x <= PALETTE_END; x++ )
	{
		Control_Interface tempControl = GetControl( x );
		
		ASRect theRect = tempControl.bounds();
#if WIN32		
		::OffsetRect( (LPRECT)&theRect, 0, shide ? mControlsHeight : -mControlsHeight );
#else
		::OffsetRect( (Rect*)&theRect, 0, shide ? mControlsHeight : -mControlsHeight );
#endif		
		tempControl.set_bounds( theRect );
	}

	// keep the state -- we may not need it...
	mPaletteControlsShowing = shide;
}


// this is a seperate routine because the button callback doesn't respond to intermediate tracking
ASAPI void ComposerUI::ScrollNotifyCB( ADMItemRef controlRef, ADMNotifierRef )
{
	Control_Interface ct( controlRef );
	ComposerUI* ui = (ComposerUI*)ct.Dialog();
	if( !ui )
		return;

	// all we have to do is invalidate the palette so it redraws
	ui->InvalidateControl( PALETTE_PALETTE );
}


ASAPI void ComposerUI::ButtonCallback( ADMItemRef controlRef, ADMNotifierRef notifier )
{
	// only execute this code when the notify is that the user changed...
	if( !ADM_Access::notifier_suite()->IsNotifierType( notifier, kADMUserChangedNotifier ) )
		return;

	// get the item number...
	Control_Interface ct( controlRef );
	ComposerUI* ui = (ComposerUI*)ct.Dialog();
	if( !ui )
		return;
	
	switch( ct.id() )
	{
		case MASTER_COLORS_LOGO:
			MCURL::Go( ui->mSelectedColorRef.GetPluginRef(), COMPANY_URL );
			break;
			
		case HELP_BUTTON:
			MCURL::Go( ui->mSelectedColorRef.GetPluginRef(), HELP_URL );
			break;
		
		case GENERATE_CREATE_PALETTE_BUTTON:
		{
			// now create the palette
			ui->CreatePalette();

			// index 0 and 1 represent 
			ui->SetToggleState( kSeparator2, true );
			
			// TEST CODE!!@ !!@  to see if we can sample a color off the screen
//			ASPoint pt = { 500, 500 };
//			ui->mSelectedColorRef.SampleColor( pt );

			break;
		}

#if PROVERSION		
		case PALETTE_SAVE_BUTTON:
		{
			Palette_Display_Handler* thePalette = dynamic_cast<Palette_Display_Handler*>( ui->mPalette );
			if( thePalette )
				thePalette->SavePalette();
			break;
		}
			
		case PALETTE_ORDER_0: 
		case PALETTE_ORDER_1:
		case PALETTE_ORDER_2:
		case PALETTE_ORDER_3: 
		case PALETTE_ORDER_4: 
		case PALETTE_ORDER_5:
			ui->SetSortOrder( (ASInt8)(ct.id() - PALETTE_ORDER_0) );
			break;
#endif
	}
}



// EOF
