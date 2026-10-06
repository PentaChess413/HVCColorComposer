/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	Source code rewritten by: Alex Lelievre ([PII Redacted]), 2006.
 *
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "MCRangeSelector.h"
#include "MCEditText.h"
#include "MCIcon.h"
#include "MCSortFrame.h"

#include "Composer_UI.h"
#include "admBasic.h"
#include "admResource.h"

// this code is only appropriate in the Pro Version
#if PROVERSION

const char* kProportionFormat = "1.0:%0.1f";
const char* kRangeFormat = "%d";

const ASInt32 kMaxBufferLen	= 1024;
const ASInt32 kSwatchLabels	= 16200;

enum 
{	
	kSwatchLabel_Between = 0, 
	kSwatchLabel_From,
	kSwatchLabel_ContrastRange,
	kSwatchLabel_Proportional,
	kSwatchLabel_Range,
	kSwatchLabel_Proportion,
	kPaletteType_Auto
};


enum
{
	kEdit_Depth = 0,
	kEdit_Range,
	kEdit_Accuracy,
	kEdit_Proportion
};


enum 
{	
	MIN_STEP_COUNT = 20, 
	MAX_STEP_COUNT = 40
};


MCRangeSelector::MCRangeSelector( ComposerUIRef ui, ASInt32 first_button, ASInt32 last_button ) :
	base( ui, first_button, last_button ),
	mUIRef( ui ),
	mMainSplit( NULL ),
	mSwatch1( NULL ),
	mSwatch2( NULL ),
	mSplit1( NULL ),
	mSplit2( NULL ),
	mSortFrame( NULL ),
	mPaletteSelectionMade( false ),
	mRadio( kMCRange_Range ),
	mPaletteType( kMCRange_Range ),
	mCurrentSortOrder( 0 ),
	mPaletteRef( NULL )
{
	::memset( &mLiveParams, 0, sizeof( mLiveParams ) );
	
	// the prefs set the values of the palette parameters
	mLiveParams.depth      = kPaletteDefault_Depth;
	mLiveParams.accuracy   = kPaletteDefault_Accuracy;
	mLiveParams.range      = kPaletteDefault_Range;
	mLiveParams.proportion = kPaletteDefault_Proportion;
	
	// copy that to the palette params (these are what is used when generating a palette and restoring from prefs)
	mPaletteParams = mLiveParams;
	
	// create the main color split
	mMainSplit = new MCDualColorSwatch( mUIRef, GENERATE_SPLIT_SWATCH, mSplitColor1, mSplitColor2 );

	// create the two color swatches
	mSwatch1 = new MCColorSwatch( mUIRef, PALETTE_COLOR_1, mColor1 );
	mSwatch2 = new MCColorSwatch( mUIRef, PALETTE_COLOR_2, mColor2 );

	mSplit1 = new Split_Color_Display_Handler( mUIRef, PALETTE_SPLIT1, mPaletteColor1, mPaletteSelection );
	mSplit2 = new Split_Color_Display_Handler( mUIRef, PALETTE_SPLIT2, mPaletteColor2, mPaletteSelection );

	mSortFrame = new MCSortFrame( mUIRef, PALETTE_SORT_FRAME );
	
	// clear the control array
	::memset( mSortIcons, 0, kNumSortIcons * sizeof( MCIcon* ) );
	::memset( mEditTexts, 0, kNumEditTexts * sizeof( MCEditText* ) );

	for( int i = 0; i < kNumSortIcons; i++ )
		mSortIcons[i] = new MCIcon( mUIRef, PALETTE_ORDER_0 + i );

	// create range, accuracy, depth and proportion edit text elements.	
	mEditTexts[kEdit_Depth] = new MCEditText( mUIRef, kEditType_RawPercent, GENERATE_DEPTH_TEXT,      &mLiveParams.depth,      NULL, this, &TextCallback );
	mEditTexts[kEdit_Range] = new MCEditText( mUIRef, kEditType_Range,      GENERATE_RANGE_TEXT,      &mLiveParams.range,      NULL, this, &TextCallback );
	mEditTexts[kEdit_Accuracy]   = new MCEditText( mUIRef, kEditType_Accuracy,   GENERATE_ACCURACY_TEXT,   &mLiveParams.accuracy,   NULL, this, &TextCallback );
	mEditTexts[kEdit_Proportion] = new MCEditText( mUIRef, kEditType_Proportion, GENERATE_PROPORTION_TEXT, &mLiveParams.proportion, NULL, this, &TextCallback );

	// depth	
	if( mEditTexts[kEdit_Depth] )
		mEditTexts[kEdit_Depth]->AllowPopupSlider( true );
	// accuracy
	if( mEditTexts[kEdit_Accuracy] )
		mEditTexts[kEdit_Accuracy]->AllowPopupSlider( true );
	// proportion
	if( mEditTexts[kEdit_Proportion] )
		mEditTexts[kEdit_Proportion]->AllowPopupSlider( true );
	
	// initially hide the splits
	ShowHideSplitSwatches( false );

	// initialize ourselves...
	Initialize( GENERATE_RANGE_RADIO );
}


MCRangeSelector::~MCRangeSelector()
{
	delete mMainSplit;
	
	delete mSwatch1;
	delete mSwatch2;
	
	delete mSplit1;
	delete mSplit2;
	
	delete mSortFrame;
	
	// delete all the controls that depend on us...
	for( int i = 0; i < kNumSortIcons; i++ )
		delete mSortIcons[i];

	for( int i = 0; i < kNumEditTexts; i++ )
		delete mEditTexts[i];
}



void MCRangeSelector::Initialize( ASInt32 new_button )
{
	if( ButtonChanging( new_button ) )
	{
		// call the base class...
		SetCurrentButton( new_button );
	}
}


void MCRangeSelector::SelectNewColor( MCColorRef newColor, SelectionFlags flags )
{
	if( !mMainSplit )
		return;
	
	bool rightSide = mMainSplit->RightSideSelected();

	// set whichever swatch is active...
	if( rightSide )
		mSplitColor2 = newColor;
	else
		mSplitColor1 = newColor;
	
	if( mRadio == kMCRange_Proportional )
	{
		// set whichever swatch is active...
		if( rightSide )
			mColor2 = newColor;
		else
			mColor1 = newColor;
	}
	else
	{
		// in this case we are doing range or auto-range
		mColor1 = mColor2 = newColor;
	}
	
	// do some accounting work
	if( flags & kUpdateFromPaletteFlag )
	{
		mPaletteSelection = newColor;
		mPaletteSelectionMade = true;
	
		// make sure to show the proper swatches here	
		ShowHideSplitSwatches( true );
	}
}



void MCRangeSelector::CreatePalette( Palette_Display_Handler* thePalette, bool setColors )
{
	if( !thePalette )
		return;
	
	// hold on to the palette for now...
	mPaletteRef = thePalette;
	
	// set the source and selection to the currently selected colors...
	if( setColors )
	{
		mPaletteColor1    = mColor1;
		mPaletteColor2    = mColor2;
		mPaletteSelection = mColor1;
	}
	
	// common stuff
	mPaletteSelectionMade = false;
	mPaletteType		  = mRadio;
	mPaletteParams		  = mLiveParams;
	
	int	step_delta = mPaletteParams.depth * (MAX_STEP_COUNT - MIN_STEP_COUNT);
	
	UpdatePaletteResults();
	
	// palette sort order is detected directly from UI

	double param         = mPaletteType != kMCRange_Proportional ? mPaletteParams.range : mPaletteParams.proportion;
	double gen_parameter = mPaletteType == kMCRange_Auto ?  -1 : param;
	double gen_accuracy  = mPaletteType == kMCRange_Auto ? 1.0 : mPaletteParams.accuracy;

	// we need to copy the current colors because the palette takes a direct (stupid) reference to the color which isn't
	// guarrantied to stick around because MCColors can fabric things that don't exist for long (like an HVCTrip) :)
	mHVC1 = mPaletteColor1;
	mHVC2 = mPaletteColor2;
	
	// Create the palette generator
	mPaletteRef->generate_palette( step_delta / PERCENT + MIN_STEP_COUNT, gen_parameter, gen_accuracy, mHVC1, mHVC2, mPaletteType != kMCRange_Proportional );


	// make sure to show the proper swatches here	
	ShowHideSplitSwatches( true );
}


void MCRangeSelector::UpdatePaletteResults()
{
	// set palette results...
	char buffer[kMaxBufferLen] = {0};
	
	if( mPaletteType == kMCRange_Proportional )
	{
		// form a string in this format "1.0:0.0"
		::sprintf( buffer, kProportionFormat, mPaletteParams.proportion );
		mUIRef.GetControl( PALETTE_RESULT ).set_text( buffer );
	}
	else if( mPaletteType == kMCRange_Auto )
	{
		if( !ADM_Access::basic_suite()->GetIndexString( mUIRef.mSelectedColorRef.GetPluginRef(), kSwatchLabels, kPaletteType_Auto, buffer, kMaxBufferLen ) )
			mUIRef.GetControl( PALETTE_RESULT ).set_text( buffer );
	}
	else
	{	
		::sprintf( buffer, kRangeFormat, mPaletteParams.range );
		mUIRef.GetControl( PALETTE_RESULT ).set_text( buffer );
	}
}


// Update the palette contrast controls
void MCRangeSelector::UpdatePaletteContrast( SelectionFlags flags )
{
	UpdateSplitContrast( GENERATE_SPLIT_CONTRAST, mSplitColor1, mSplitColor2 );
	mUIRef.InvalidateControl( GENERATE_SPLIT_SWATCH );
	mUIRef.InvalidateControl( PALETTE_COLOR_1 );
	mUIRef.InvalidateControl( PALETTE_COLOR_2 );
	
 	if( flags & kUpdateFromPaletteFlag )
	{
		UpdateSplitContrast( PALETTE_CONTRAST1, mPaletteColor1, mPaletteSelection );
		UpdateSplitContrast( PALETTE_CONTRAST2, mPaletteColor2, mPaletteSelection );

		// invalidate our controls...
		mUIRef.InvalidateControl( PALETTE_SPLIT1 );
		mUIRef.InvalidateControl( PALETTE_SPLIT2 );
	}
}


void MCRangeSelector::UpdateWebSafe( bool safe )
{
	// tell all the colors that we only want web safe colors...
	mColor1.SetWebSafe( safe );
	mColor2.SetWebSafe( safe );
	mSplitColor1.SetWebSafe( safe );
	mSplitColor2.SetWebSafe( safe );
	
	UpdatePaletteContrast();
}


bool MCRangeSelector::ButtonChanging( ASInt32 new_button )
{
	if( !mMainSplit )
		return false;
	
	bool rightSide = mMainSplit->RightSideSelected();

	try
	{
		// Change the user interface layout depending on what radio button is selected...
		switch( new_button )						
		{
			case GENERATE_RANGE_RADIO:
				mRadio = kMCRange_Range;
				mColor1 = mColor2 = rightSide ? mSplitColor2 : mSplitColor1;
				break;

			case GENERATE_AUTO_RANGE_RADIO:
				mRadio = kMCRange_Auto;
				mColor1 = mColor2 = rightSide ? mSplitColor2 : mSplitColor1;
				break;

			case GENERATE_PROPORTION_RADIO:
				mRadio = kMCRange_Proportional;
				mColor1 = mSplitColor1;
				mColor2 = mSplitColor2;
				break;


			default:
				XDEBUG_PRINT( "default case hit inside range selector. - what button did you hit?\n" );
				return false;
		}
		
		// make sure the little color patches are updated

		
		// update the controls, make sure the right ones are showing...
		UpdateControls();
	}
	catch(...)
	{
		return false;
	}

	return true;
}


// Update the specified split swatch controls
void MCRangeSelector::UpdateSplitContrast( short text_item, const HVCTrip& first_color, const HVCTrip& second_color )
{
	// Calculate the split swatch distance
	double contrast = Palette_Display_Base::CalculateDistance( first_color, second_color );	

	Control_Interface	text_display = mUIRef.GetControl( text_item );
	char				buffer[kMaxBufferLen] = {0};

	// Auto mode
	if( mPaletteType != kMCRange_Proportional && (text_item == PALETTE_CONTRAST1 || text_item == PALETTE_CONTRAST2) )
	{
		// Limit it to steps of 5
		ASInt32 intContrast = (ASInt32)contrast;
		intContrast += 2;											
		intContrast /= 5;
		intContrast *= 5;
		
		// set back to floating point
		contrast = intContrast;
	}
	
	// stuff that into the UI
	ADM_Access::basic_suite()->ValueToString( (float)contrast, buffer, kMaxBufferLen, kADMNoUnits, 0, false );

	// Update the text control
	text_display.set_text( buffer );								
}


// this determines which splits to show depending on the range selection
void MCRangeSelector::ShowHideSplitSwatches( bool show )
{
	// if we are hiding, hide everything and bail
	if( !show )
	{
		mUIRef.ShowHideControl( PALETTE_SPLIT1, false );
		mUIRef.ShowHideControl( PALETTE_SPLIT1_STATIC, false );
		mUIRef.ShowHideControl( PALETTE_SPLIT2, false );
//		mUIRef.ShowHideControl( PALETTE_SPLIT2_STATIC, false );
		mUIRef.ShowHideControl( PALETTE_CONTRAST1, false );
		mUIRef.ShowHideControl( PALETTE_CONTRAST2, false );
	}
	else
	{
		// load the right label
		char  label[kMaxBufferLen] = { 0 };
		ASErr error = ADM_Access::basic_suite()->GetIndexString( mUIRef.mSelectedColorRef.GetPluginRef(), kSwatchLabels, mPaletteType == kMCRange_Proportional ? kSwatchLabel_Between : kSwatchLabel_From, label, kMaxBufferLen );
		if( !error )
			mUIRef.GetControl( PALETTE_SPLIT1_STATIC ).set_text( label );

		error = ADM_Access::basic_suite()->GetIndexString( mUIRef.mSelectedColorRef.GetPluginRef(), kSwatchLabels, mPaletteType == kMCRange_Proportional ? kSwatchLabel_Proportional : kSwatchLabel_ContrastRange, label, kMaxBufferLen );
		if( !error )
			mUIRef.GetControl( PALETTE_TYPE_STATIC ).set_text( label );

		// show the first split swatch and contrast
		mUIRef.ShowHideControl( PALETTE_SPLIT1, true );
		mUIRef.ShowHideControl( PALETTE_SPLIT1_STATIC, true );
		
		mUIRef.ShowHideControl( PALETTE_CONTRAST1, mPaletteSelectionMade );
		
		// if we are in proportional than we should show both...
		mUIRef.ShowHideControl( PALETTE_SPLIT2, mPaletteType == kMCRange_Proportional );
		mUIRef.ShowHideControl( PALETTE_CONTRAST2, mPaletteSelectionMade && (mPaletteType == kMCRange_Proportional) );
//		mUIRef.ShowHideControl( PALETTE_SPLIT2_STATIC, mPaletteType == kMCRange_Proportional );

	}
}


// this determines which controls to show depending on the range selection
void MCRangeSelector::UpdateControls()
{
	// load the right label
	char  label[kMaxBufferLen] = { 0 };
	ASErr error = ADM_Access::basic_suite()->GetIndexString( mUIRef.mSelectedColorRef.GetPluginRef(), kSwatchLabels, mRadio == kMCRange_Proportional ? kSwatchLabel_Between : kSwatchLabel_From, label, kMaxBufferLen );

	// set the text of the split label and the first color label
	if( !error )
		mUIRef.GetControl( GENERATE_PROPORTION_FIRST_COLOR_LABEL ).set_text( label );

//	error = ADM_Access::basic_suite()->GetIndexString( mUIRef.mSelectedColorRef.GetPluginRef(), kSwatchLabels, mRadio == kMCRange_Proportional ? kSwatchLabel_Proportion : kSwatchLabel_Range, label, kMaxBufferLen );
	error = ADM_Access::basic_suite()->GetIndexString( mUIRef.mSelectedColorRef.GetPluginRef(), kSwatchLabels + (mRadio == kMCRange_Proportional ? kSwatchLabel_Proportion : kSwatchLabel_Range), 0, label, kMaxBufferLen );
	if( !error )
		mUIRef.GetControl( GENERATE_PROPORTION_LABEL ).set_text( label );

	// if we are in proportional than we should show both patches...
	mUIRef.ShowHideControl( PALETTE_COLOR_2, mRadio == kMCRange_Proportional );
	mUIRef.ShowHideControl( GENERATE_PROPORTION_SECOND_COLOR_LABEL, mRadio == kMCRange_Proportional );
	
	switch( mRadio )
	{
		case kMCRange_Proportional:
			mUIRef.ShowHideControl( GENERATE_PROPORTION_LABEL, true );
			mUIRef.ShowHideControl( GENERATE_ACCURACY_LABEL, true );
			mUIRef.ShowHideControl( GENERATE_RANGE_TEXT, false );

			if( mEditTexts[kEdit_Accuracy] )
				mEditTexts[kEdit_Accuracy]->set_visible( true );

			if( mEditTexts[kEdit_Proportion] )
				mEditTexts[kEdit_Proportion]->set_visible( true );
			break;
			
		case kMCRange_Range:
			mUIRef.ShowHideControl( GENERATE_PROPORTION_LABEL, true );
			mUIRef.ShowHideControl( GENERATE_ACCURACY_LABEL, true );
			mUIRef.ShowHideControl( GENERATE_RANGE_TEXT, true );

			if( mEditTexts[kEdit_Accuracy] )
				mEditTexts[kEdit_Accuracy]->set_visible( true );

			if( mEditTexts[kEdit_Proportion] )
				mEditTexts[kEdit_Proportion]->set_visible( false );
			break;

		case kMCRange_Auto:
			mUIRef.ShowHideControl( GENERATE_PROPORTION_LABEL, false );
			mUIRef.ShowHideControl( GENERATE_ACCURACY_LABEL, false );
			mUIRef.ShowHideControl( GENERATE_RANGE_TEXT, false );

			if( mEditTexts[kEdit_Accuracy] )
				mEditTexts[kEdit_Accuracy]->set_visible( false );

			if( mEditTexts[kEdit_Proportion] )
				mEditTexts[kEdit_Proportion]->set_visible( false );
			break;
	}
}


void MCRangeSelector::SetPaletteColor( MCColorRef colorRef, short index )
{
	// only index 0 and 1 are valid
	if( index == 0 )
		mPaletteColor1 = colorRef;
	else
		mPaletteColor2 = colorRef;
}

MCColor* MCRangeSelector::GetPaletteColor( short index )
{
	// only index 0 and 1 are valid
	if( index == 0 )
		return &mPaletteColor1;
	
	return &mPaletteColor2;
}


void MCRangeSelector::SetPaletteSelection( MCColorRef colorRef )
{
	mPaletteSelection = colorRef;
}

MCColor* MCRangeSelector::GetPaletteSelection()
{
	return &mPaletteSelection;
}


short MCRangeSelector::GetSelectedRange()
{
	return mRadio;
}


void MCRangeSelector::SetSelectedRange( short selectedRadio )
{
	switch( selectedRadio )						
	{
		case kMCRange_Range:
			Initialize( GENERATE_RANGE_RADIO );
			break;

		case kMCRange_Auto:
			Initialize( GENERATE_AUTO_RANGE_RADIO );
			break;

		case kMCRange_Proportional:
			Initialize( GENERATE_PROPORTION_RADIO );
			break;
	}
}



void MCRangeSelector::SetSplitColor( MCColorRef colorRef, short index )
{
	// only index 0 and 1 are valid
	if( index == 0 )
		mSplitColor1 = mColor1 = colorRef;
	else
		mSplitColor2 = mColor2 = colorRef;
}

MCColor* MCRangeSelector::GetSplitColor( short index )
{
	// only index 0 and 1 are valid
	if( index == 0 )
		return &mSplitColor1;
	
	return &mSplitColor2;
}


void MCRangeSelector::SetSelectedSplit( unsigned char index )
{
	if( mMainSplit )
		mMainSplit->SetRightSideSelected( index == 1 );
}

unsigned char MCRangeSelector::GetSelectedSplit()
{
	if( mMainSplit )
		return mMainSplit->RightSideSelected();

	return 0;
}


void MCRangeSelector::SetPaletteSortOrder( ASInt8 newOrder )
{
	mCurrentSortOrder = newOrder; 

	// Generate a new palette
	if( mPaletteRef )
		mPaletteRef->generate_palette();			
}

ASInt8 MCRangeSelector::GetPaletteSortOrder()
{ 
	return mCurrentSortOrder; 
}


short MCRangeSelector::GetRange()
{
	return mPaletteParams.range;
}

void MCRangeSelector::SetRange( short n )
{
	mPaletteParams.range = mLiveParams.range = n;
	mUIRef.UpdateControls( GENERATE_RANGE_TEXT, GENERATE_RANGE_TEXT );
}

short MCRangeSelector::GetDepth()
{
	return mPaletteParams.depth;
}

void MCRangeSelector::SetDepth( short n )
{
	mPaletteParams.depth = mLiveParams.depth = n;
	mUIRef.UpdateControls( GENERATE_DEPTH_TEXT, GENERATE_DEPTH_TEXT );
}

float MCRangeSelector::GetAccuracy()
{
	return mPaletteParams.accuracy;
}

void MCRangeSelector::SetAccuracy( float n )
{
	mPaletteParams.accuracy = mLiveParams.accuracy = n;
	mUIRef.UpdateControls( GENERATE_ACCURACY_TEXT, GENERATE_ACCURACY_TEXT );
}

float MCRangeSelector::GetProportion()
{
	return mPaletteParams.proportion;
}

void MCRangeSelector::SetProportion( float n )
{
	mPaletteParams.proportion = mLiveParams.proportion = n;
	mUIRef.UpdateControls( GENERATE_PROPORTION_TEXT, GENERATE_PROPORTION_TEXT );
}


void MCRangeSelector::TextCallback( const MCEditText& target, void* userData )
{
	// get us...
	MCRangeSelector* me = (MCRangeSelector*)userData;
	if( !me )
		return;
		
	// check what's in the edit box and make sure it's correct...
	// !!@
	
	me->mUIRef.UpdateControls( target.id(), target.id() );
}
		

#endif // PROVERSION

// EOF
