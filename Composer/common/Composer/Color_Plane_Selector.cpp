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

#include "Color_Plane_Selector.h"
#include "RGB_Selector.h"
#include "Lab_Selector.h"
#include "HVC_Selector.h"
#include "HSB_Selector.h"

#include "Selected_Color_Display.h"
#include "Warning_Color_Display.h"

#include "MCEditText.h"
#include "Raw_Text_Display.h"

#include "Color_Flag_Handler.h"
#include "Hue_Slider_Handler.h"
#include "Check_Box_Handler.h"
#include "Quick_Colors_Handler.h"

#include "admBasic.h"


#define IMAGE_WIDTH_INSET  6



ColorPlaneSelector::ColorPlaneSelector( ComposerUIRef ui, SelectedColorRef colorRef, ASInt32 first_button, ASInt32 last_button ) :
	base( ui, first_button, last_button ),
	mUIRef( ui ),
	mMCLib( colorRef.GetMCLibRef() ),
	mIsHVCSafe( true ),
	mIsPrintSafe( true ),
	mIsWebSafe( true ),
	mQuickColors( NULL ),
	mSelected( NULL ),
	mSelectedColorRef( colorRef ),
	m_xy_buffer( NULL ),
	m_z_buffer( NULL )
{
	// clear the control array
	::memset( mControls, 0, kNumControls * sizeof( Control_Runner* ) );

	// create the display with the current color and the original color.
	mSelected    = new Selected_Color_Display( mUIRef, COLOR_DISPLAY, mSelectedColorRef );
	mQuickColors = new Quick_Colors_Handler( mUIRef, QUICK_SWATCHES );

	// TEXT displays -- start with HSB here
	mControls[0] = new MCEditText( mUIRef, kEditType_RawDegrees, HSB_HUE_TEXT,        &m_hsb[0], &HSBCallback, this );
	mControls[1] = new MCEditText( mUIRef, kEditType_Percent,    HSB_SATURATION_TEXT, &m_hsb[1], &HSBCallback, this );
	mControls[2] = new MCEditText( mUIRef, kEditType_Percent,    HSB_BRIGHTNESS_TEXT, &m_hsb[2], &HSBCallback, this );
	
	// now do RGB
	mControls[3] = new MCEditText( mUIRef, kEditType_Raw, RGB_RED_TEXT,   &m_rgb[0], &RGBCallback, this );
	mControls[4] = new MCEditText( mUIRef, kEditType_Raw, RGB_GREEN_TEXT, &m_rgb[1], &ColorPlaneSelector::RGBCallback, this );
	mControls[5] = new MCEditText( mUIRef, kEditType_Raw, RGB_BLUE_TEXT,  &m_rgb[2], &ColorPlaneSelector::RGBCallback, this );
	
	// Lab, L uses the percent display, ab use the -128 to 127 display...
	mControls[6] = new MCEditText( mUIRef, kEditType_Percent, LAB_L_TEXT, &m_lab[0], &ColorPlaneSelector::LabCallback, this );
	mControls[7] = new MCEditText( mUIRef, kEditType_Signed,  LAB_A_TEXT, &m_lab[1], &ColorPlaneSelector::LabCallback, this );
	mControls[8] = new MCEditText( mUIRef, kEditType_Signed,  LAB_B_TEXT, &m_lab[2], &ColorPlaneSelector::LabCallback, this );

	// the cmyk data is "inverted" so we use a special case of the percent edit text
	mControls[9]  = new MCEditText( mUIRef, kEditType_CMYK, CMYK_CYAN_TEXT,    &m_cmyk[0], &ColorPlaneSelector::CMYKCallback, this );
	mControls[10] = new MCEditText( mUIRef, kEditType_CMYK, CMYK_MAGENTA_TEXT, &m_cmyk[1], &ColorPlaneSelector::CMYKCallback, this );
	mControls[11] = new MCEditText( mUIRef, kEditType_CMYK, CMYK_YELLOW_TEXT,  &m_cmyk[2], &ColorPlaneSelector::CMYKCallback, this );
	mControls[12] = new MCEditText( mUIRef, kEditType_CMYK, CMYK_BLACK_TEXT,   &m_cmyk[3], &ColorPlaneSelector::CMYKCallback, this );
	
	mControls[13] = new MCEditText( mUIRef, kEditType_Munsell, HVC_MUNSELL_HUE_TEXT,    &m_hvc[0], &ColorPlaneSelector::HVCCallback, this );
	mControls[14] = new MCEditText( mUIRef, kEditType_Float,   HVC_MUNSELL_VALUE_TEXT,  &m_hvc[1], &ColorPlaneSelector::HVCCallback, this );
	mControls[15] = new MCEditText( mUIRef, kEditType_Float,   HVC_MUNSELL_CHROMA_TEXT, &m_hvc[2], &ColorPlaneSelector::HVCCallback, this );

	MCEditText* hvt = dynamic_cast<MCEditText*>( mControls[14] );
	MCEditText* hct = dynamic_cast<MCEditText*>( mControls[15] );
	
	// set the value min and max
	if( hvt )
		hvt->SetMinMax( kMin_MunsellValue, kMax_MunsellValue );
		
	// set the chroma min and max
	if( hct )
		hct->SetMinMax( kMin_MunsellChroma, kMax_MunsellChroma );
	
	
	mControls[16] = new MCEditText( mUIRef, kEditType_Degrees, HVC_HUE_TEXT,    &m_hvc[0], &HVCCallback, this );
	mControls[17] = new MCEditText( mUIRef, kEditType_Percent, HVC_VALUE_TEXT,  &m_hvc[1], &HVCCallback, this );
	mControls[18] = new MCEditText( mUIRef, kEditType_Percent, HVC_CHROMA_TEXT, &m_hvc[2], &HVCCallback, this );

	mControls[19] = new Raw_Text_Display( mUIRef, RAW_COLOR_TEXT, m_rgb );

	mControls[20] = new Check_Box_Handler( mUIRef, ONLY_WEB_COLORS,  &ColorPlaneSelector::WebCallback );
	mControls[21] = new Check_Box_Handler( mUIRef, MUNSELL_NOTATION, &ColorPlaneSelector::MunsellCallback );
	Control_Runner* mc = mControls[21];

	// initialize the munsell checkbox
	if( mc )
		MunsellCallback( *mc );


	mControls[22] = new Warning_Color_Display( mUIRef, COLOR_SPACE_WARNING_ICON, mHVCSafeColor );
	mControls[23] = new Warning_Color_Display( mUIRef, PRINTABLE_WARNING_ICON, mPrintSafeColor );
	mControls[24] = new Warning_Color_Display( mUIRef, WEB_COLOR_WARNING_ICON, mWebSafeColor );

	mControls[25] = new Color_Flag_Handler( mUIRef, COLOR_FLAG, mSelectedColorRef.GetDisplayProc() );
	mControls[26] = new Hue_Slider_Handler( mUIRef, HUE_SLIDER, mSelectedColorRef.GetDisplayProc() );

	
	ASRect				select_bounds = { 0, 0, RAW_RANGE, RAW_RANGE };
	Color_Flag_Handler*	color_flag_handler = dynamic_cast<Color_Flag_Handler*>( mControls[25] );
	Hue_Slider_Handler*	hue_slider_handler = dynamic_cast<Hue_Slider_Handler*>( mControls[26] );

	if( hue_slider_handler && color_flag_handler )
	{
		ASRect			hue_slider_bounds	= hue_slider_handler->bounds();
		G_World&		xy_world			= color_flag_handler->bit_map();
		G_World::Hold	XY_select			= xy_world.create( select_bounds );
		
		// adjust the bounds to reuse it for the hue slider...
		select_bounds.right = hue_slider_bounds.right - hue_slider_bounds.left - IMAGE_WIDTH_INSET * 2;

		G_World&		z_world   = hue_slider_handler->bit_map();
		G_World::Hold	Z_select  = z_world.create( select_bounds );
	
		// initialize ourselves...
		Initialize( HSB_HUE_RADIO );
	
		Z_select.release();
		XY_select.release();
		
		// store the xy and z buffer references for later disposal...
		m_xy_buffer = &xy_world;
		m_z_buffer = &z_world;
	}
	
	
	// get all the numeric displays updated
	HandleColorChange( SKIP_NONE, kUpdateUnspecifiedFlag );
}


ColorPlaneSelector::~ColorPlaneSelector()
{
	// delete all the controls that depend on us...
	for( int i = 0; i < kNumControls; i++ )
		delete mControls[i];
	
//	if( m_xy_buffer )
//		m_xy_buffer->dispose();
		
//	if( m_z_buffer )
//		m_z_buffer->dispose();
		
	delete mSelected;
	delete mQuickColors;
}



void ColorPlaneSelector::Initialize( ASInt32 new_button )
{
	if( ButtonChanging( new_button ) )
	{
		// call the base class...
		SetCurrentButton( new_button );
	}
}



bool ColorPlaneSelector::ButtonChanging( ASInt32 new_button )
{
	try
	{
		// Change how the color flag and color slider display is generated
		switch( new_button )						
		{
			case HSB_HUE_RADIO:
				mCurrentSelector = Auto_Selector( new HSB_Hue_Selector( m_hsb_spectrum ) );
				break;

			case HSB_SATURATION_RADIO:
				mCurrentSelector = Auto_Selector( new Saturation_Selector( m_hsb_spectrum ) );
				break;

			case HSB_BRIGHTNESS_RADIO:
				mCurrentSelector = Auto_Selector( new Brightness_Selector( m_hsb_spectrum ) );
				break;

			case RGB_RED_RADIO:
				mCurrentSelector = Auto_Selector( new Red_Selector( m_rgb_spectrum ) );
				break;

			case RGB_GREEN_RADIO:
				mCurrentSelector = Auto_Selector( new Green_Selector( m_rgb_spectrum ) );
				break;

			case RGB_BLUE_RADIO:
				mCurrentSelector = Auto_Selector( new Blue_Selector( m_rgb_spectrum ) );
				break;

			case LAB_L_RADIO:
				mCurrentSelector = Auto_Selector( new L_Selector( m_lab_spectrum ) );
				break; 

			case LAB_A_RADIO:
				mCurrentSelector = Auto_Selector( new A_Selector( m_lab_spectrum ) );
				break;

			case LAB_B_RADIO:
				mCurrentSelector = Auto_Selector( new B_Selector( m_lab_spectrum ) );
				break;

			case HVC_HUE_RADIO:
				mCurrentSelector = Auto_Selector( new HVC_Hue_Selector( m_hvc_spectrum, mSelectedColorRef.GetMCLibRef() ) );
				break;

			case HVC_VALUE_RADIO:
				mCurrentSelector = Auto_Selector( new Value_Selector( m_hvc_spectrum, mSelectedColorRef.GetMCLibRef() ) );
				break;

			case HVC_CHROMA_RADIO:
				mCurrentSelector = Auto_Selector( new Chroma_Selector( m_hvc_spectrum, mSelectedColorRef.GetMCLibRef() ) );
				break;

			default:
				XDEBUG_PRINT( "default case hit inside color plane selector. - what button did you hit?\n" );
				return false;
		}
		
		// set the state of the web safe display in the selector...
		mCurrentSelector->SetWebSafe( GetWebCheckboxState() );
		
		UpdateSpectrumDisplays();
	}
	catch(...)
	{
		return false;
	}

	return true;
}

void ColorPlaneSelector::UpdateSpectrumDisplays()
{
	// ensure that the spectrum displays are set to the right mode
	if( m_z_buffer )
	{
		m_z_buffer->set_mode( mCurrentSelector->GetCurrentMode() );
		mCurrentSelector->UpdateZBuffer( m_z_buffer->get() );
	}
	
	if( m_xy_buffer )
	{
		m_xy_buffer->set_mode( mCurrentSelector->GetCurrentMode() );
		mCurrentSelector->UpdateXYBuffer( m_xy_buffer->get() );
	}
	
	// now deal with the slider and spectrum directly...
	Control_Runner*	control = mUIRef.get_item( HUE_SLIDER );	
	if( !control )
		return;
	
	// Update the color slider
	control->set_int_value( RAW_MAX - mCurrentSelector->GetZ() );
	control->invalidate();
	
	Color_Flag_Handler* colorFlag = (Color_Flag_Handler*)mUIRef.get_item( COLOR_FLAG );					
	if( !colorFlag )
		return;
		
	// Update the color spectrum - this will move the little indicator dot to the correct location	
	colorFlag->SetPosition( (short)(RAW_MAX - mCurrentSelector->GetX()), (short)mCurrentSelector->GetY() );
	colorFlag->invalidate();
}



Color_Selector::Update_Check ColorPlaneSelector::CheckForUpdate() 
{ 
	return mCurrentSelector->BeginUpdateCheck( m_z_buffer->get(), m_xy_buffer->get(), mUIRef );
} 



void ColorPlaneSelector::UpdateXYDisplay( ASInt32 new_z )
{
	if( mCurrentSelector->SetZ( (short)new_z, mUIRef ) )
		HandleColorChange( mCurrentSelector->GetSkipBlock() );
}


void ColorPlaneSelector::UpdateZDisplay( ASInt32 new_x, ASInt32 new_y )
{
	bool xChanged = mCurrentSelector->SetX( (short)new_x, mUIRef );
	bool yChanged = mCurrentSelector->SetY( (short)new_y, mUIRef );

	if( !xChanged && !yChanged )
		return;
		
	HandleColorChange( mCurrentSelector->GetSkipBlock() );
}


void ColorPlaneSelector::SetQuickColor( MCColorRef colorRef, short index )
{
	if( !mQuickColors )
		return;
		
	return mQuickColors->SetColor( colorRef, index );
}


MCColor* ColorPlaneSelector::GetQuickColor( short index )
{
	if( !mQuickColors )
		return NULL;
		
	return mQuickColors->GetColor( index );
}

void ColorPlaneSelector::SetQuickColorSelection( short index )
{
	if( !mQuickColors )
		return;
		
	mQuickColors->SetSelection( index );
}

short ColorPlaneSelector::GetQuickColorSelection()
{
	if( !mQuickColors )
		return NULL;
		
	return mQuickColors->GetSelection();
}



bool ColorPlaneSelector::GetWebCheckboxState()
{
	// check to see if the web checkbox is selected
	Control_Runner*	checkbox = mUIRef.get_item( ONLY_WEB_COLORS );
	if( !checkbox )
		return false;
	
	// get state of control
	return checkbox->bool_value();
} 

void ColorPlaneSelector::SetWebCheckboxState( bool checked )
{
	// set the value of the checkbox and then call the callback
	Control_Runner*	checkbox = mUIRef.get_item( ONLY_WEB_COLORS );
	if( !checkbox )
		return;
	
	checkbox->set_bool_value( checked );
	WebCallback( *checkbox );
}


bool ColorPlaneSelector::GetMunsellCheckboxState()
{
	// check to see if the web checkbox is selected
	Control_Runner*	checkbox = mUIRef.get_item( MUNSELL_NOTATION );
	if( !checkbox )
		return false;
	
	// get state of control
	return checkbox->bool_value();
} 

void ColorPlaneSelector::SetMunsellCheckboxState( bool checked )
{
	// set the value of the checkbox and then call the callback
	Control_Runner*	checkbox = mUIRef.get_item( MUNSELL_NOTATION );
	if( !checkbox )
		return;
	
	checkbox->set_bool_value( checked );
	MunsellCallback( *checkbox );
}


short ColorPlaneSelector::GetSelectedColorPlane()
{
	return (short)GetCurrentButton();
}


void ColorPlaneSelector::SetSelectedColorPlane( short button )
{
	if( ButtonChanging( button ) )
		SetCurrentButton( button );
}



//////////////////////////////////////////////////////////////////////////////////////////////
//
// Handle request to update the color from the displays
//
//////////////////////////////////////////////////////////////////////////////////////////////

// This routine must be called in order for all the color values to be updated
void ColorPlaneSelector::HandleColorChange( SkipBlock skipBlock, SelectionFlags flags )
{
	// when a color changes we convert to all spaces expect for the block that needs to be skipped
	if( skipBlock != SKIP_HSB )
		SetHSB();
		
	if( skipBlock != SKIP_HVC )
		SetHVC();

	if( skipBlock != SKIP_RGB )
		SetRGB();

	if( skipBlock != SKIP_LAB )
		SetLab();

	if( skipBlock != SKIP_CMYK )
		SetCMYK();
	
	if( skipBlock != SKIP_RAW )
		SetRaw();

	// this updates all the swatches and warnings
	CalculateWarnings();
	
	// Update the selected quick color -- if this request didn't already come 
	// from a quick color, otherwise we ignore the request...
	if( mQuickColors && !(flags & kUpdateFromQuickColorFlag) )
		mQuickColors->SetCurrentColor( mSelectedColorRef );	

	// Update the selected color display and quick swatches
	mUIRef.InvalidateControl( QUICK_SWATCHES );					
	mUIRef.InvalidateControl( COLOR_DISPLAY );
	
	// make sure the spectrum updates too
	UpdateSpectrumDisplays();
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Setters
//
//////////////////////////////////////////////////////////////////////////////////////////////


void ColorPlaneSelector::SetRaw()
{
	// !!@ deal with this code...
	Raw_Text_Display* text = dynamic_cast<Raw_Text_Display*>( mUIRef.get_item( RAW_COLOR_TEXT ) );
	if( !text )
		return;
		
	short source[4];
	mSelectedColorRef.GetColor( kRGBSpace, source );
	text->update_color( source );
}

void ColorPlaneSelector::SetHVC()
{
	mSelectedColorRef.GetColor( kHVCSpace, m_hvc );
	mSelectedColorRef.GetColorIgnoreWeb( kHVCSpace, m_hvc_spectrum );

	mUIRef.UpdateControls( HVC_HUE_TEXT, HVC_MUNSELL_CHROMA_TEXT );
}

void ColorPlaneSelector::SetRGB()
{
	mSelectedColorRef.GetColor( kRGBSpace, m_rgb );
	mSelectedColorRef.GetColorIgnoreWeb( kRGBSpace, m_rgb_spectrum );

	mUIRef.UpdateControls( RGB_RED_TEXT, RGB_BLUE_TEXT );
}

void ColorPlaneSelector::SetHSB()
{
	// convert color from whatever space it's in to HSB
	mSelectedColorRef.GetColor( kHSBSpace, m_hsb );
	mSelectedColorRef.GetColorIgnoreWeb( kHSBSpace, m_hsb_spectrum );
	
	mUIRef.UpdateControls( HSB_HUE_TEXT, HSB_BRIGHTNESS_TEXT );
}

void ColorPlaneSelector::SetLab()
{
	mSelectedColorRef.GetColor( kLabSpace, m_lab );
	mSelectedColorRef.GetColorIgnoreWeb( kLabSpace, m_lab_spectrum );

	mUIRef.UpdateControls( LAB_L_TEXT, LAB_B_TEXT );
}


void ColorPlaneSelector::SetCMYK()
{
	mSelectedColorRef.GetColor( kCMYKSpace, m_cmyk );
	mUIRef.UpdateControls( CMYK_CYAN_TEXT, CMYK_BLACK_TEXT );
}


// all colors must come thru here for the displays to be correct.
void ColorPlaneSelector::SelectNewColor( MCColorRef newColor, SelectionFlags flags )
{
	mSelectedColorRef.SetColor( newColor );
	
	// get all the numeric displays updated
	HandleColorChange( SKIP_NONE, flags );
}



//////////////////////////////////////////////////////////////////////////////////////////////
//
// Display and update code
//
//////////////////////////////////////////////////////////////////////////////////////////////

void ColorPlaneSelector::CalculateWarnings()
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

	// Update the color space warning
	didChange = CalculateClosestHVCColor( not_safe );	
	UpdateWarning( COLOR_SPACE_WARNING_ICON, not_safe, didChange, mIsHVCSafe );
}



// Calculate the print safe color by converting the CMYK color to RGB
bool ColorPlaneSelector::CalculatePrintSafeColor()
{
	MCColor cmykColor = mSelectedColorRef.GetColorInCMYK();
	
	// check to see if this color is different than the current print safe color...
	bool didChange = !mPrintSafeColor.Compare( cmykColor, true );

	// well did the color change?
	if( didChange )
		mPrintSafeColor = cmykColor;

	return didChange;
}

// Calculate the web safe color by grabbing web RGB
bool ColorPlaneSelector::CalculateWebSafeColor( bool& notSafe )
{
	MCColor webColor = mSelectedColorRef.GetColorInWeb();
	
	// check to see if the colors are the same
	notSafe = !mSelectedColorRef.Compare( webColor, true );
	
	// check to see if this color is different than the current web safe color...
	bool didChange = !mWebSafeColor.Compare( webColor, true );

	// well did the color change?
	if( didChange )
		mWebSafeColor = webColor;

	return didChange;
}


// Calculate the HVC color to use for the color space warning
bool ColorPlaneSelector::CalculateClosestHVCColor( bool& not_safe )
{
	RGBTrip rgb			 = {0};
	HVCTrip hvc			 = {0};
	MCLib_Wrapper&	mc   = mSelectedColorRef.GetMCLibRef();

	// smush the shorts into a HCTrip for use with the MCLib
	// we use the MCLib for the max chroma function and the error condition when converting to RGB
	MCColor hvcNoWeb = mSelectedColorRef;
	
	// turn off the web color functionality for this comparison.
	// this fixes the bug where there is no closest color
	// when the web color checkbox is enabled...
	hvcNoWeb.SetWebSafe( false );
	
	// get an HVCTrip from the selected color...  
	hvc = hvcNoWeb;
		
	// convert the current HVC values to RGB and see if we get an error!
	bool inGamut = mc.hvc_2_rgb( hvc, rgb );
	
	if( !inGamut )										
	{
		// Not valid, find the nearest valid chroma -- note: we are truncating a float to an unsigned char here!!@
		hvc.chroma = (unsigned char)mc.max_chroma( hvc.hue, hvc.value );
		
		// Get the RGB equivalent for the display
		mc.hvc_2_rgb( hvc, rgb );	
		
		not_safe = true;
	}
	else
		not_safe = false;

	// store the new color for the HVC warning 
	mHVCSafeColor = MCColor( hvc );

	// Determine if the display needs updating
	bool didChange = false;						

	// compare the new color to the old
	unsigned char*  hvcPtr = &hvc.hue; 

	for( int i = 0; i < 3; ++i )
	{
		if( hvcPtr[i] != m_hvc[i] )
			didChange = true;			// !!@ we should break out here once found, hrm...
	}

	return didChange;
}


// Update the color warning controls
void ColorPlaneSelector::UpdateWarning( ASInt32 pictureID, bool not_safe, bool color_changed, bool& safe_color )
{
	// Change the visibility of the warning
	if( not_safe != safe_color )
	{													
		safe_color = not_safe;	// update state
		mUIRef.ShowHideControl( pictureID, not_safe );
		mUIRef.ShowHideControl( pictureID + 1, not_safe );
	}
	else if( not_safe && color_changed )
	{
		// Update the warning color
		mUIRef.InvalidateControl( pictureID + 1 );
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Callbacks from text objects
//
//////////////////////////////////////////////////////////////////////////////////////////////

void ColorPlaneSelector::WebCallback( const Control_Runner& )
{
	Color_Selector::Update_Check updater = CheckForUpdate();
	
	bool checked = GetWebCheckboxState();
	
	// set the display to show web safe RGB if possible
	mCurrentSelector->SetWebSafe( checked );

	// also tell the selected color that we only want web safe colors...
	mSelectedColorRef.SetWebSafe( checked );

	// !!@ we should also tell the quick colors 
	// !!@ HOWEVER: if we do this we will run into the situation where a user can get
	// a particular quick color to only display the web version of the real color...  

#if PROVERSION
	// !!@ in the pro-version we should tell all the color patches !!@ 
	MCRangeSelector* range = mUIRef.GetRangeSelector();
	if( range )
		range->UpdateWebSafe( checked );
#endif


 	// force a color update... this way if the web safe mode changed the display will reflect it
	HandleColorChange( SKIP_NONE );
}



void ColorPlaneSelector::MunsellCallback( const Control_Runner& )
{
	// if the checkbox is checked then we display the munsell values instead of the "normal values"
	bool checked = GetMunsellCheckboxState();
	
	// note: I don't like using loops for this stuff because if the item numbers
	// ever become discontiguous then this code would break...
	mUIRef.ShowHideControl( HVC_HUE_TEXT, !checked );
	mUIRef.ShowHideControl( HVC_VALUE_TEXT, !checked );
	mUIRef.ShowHideControl( HVC_CHROMA_TEXT, !checked );
	
	mUIRef.ShowHideControl( HVC_MUNSELL_HUE_TEXT, checked );
	mUIRef.ShowHideControl( HVC_MUNSELL_VALUE_TEXT, checked );
	mUIRef.ShowHideControl( HVC_MUNSELL_CHROMA_TEXT, checked );
	
	// don't forget about the little unit labels...
	mUIRef.ShowHideControl( HVC_HUE_LABEL, !checked );
	mUIRef.ShowHideControl( HVC_VALUE_LABEL, !checked );
	mUIRef.ShowHideControl( HVC_CHROMA_LABEL, !checked );
	
	// update "synchonize" the edit text in case one got edited -- 
	// otherwise one will be out of sync when first displayed!
	mUIRef.UpdateControls( HVC_HUE_TEXT, HVC_MUNSELL_CHROMA_TEXT );
}



void ColorPlaneSelector::HSBCallback( const MCEditText&, void* userData )
{
	ColorPlaneSelector* me = (ColorPlaneSelector*)userData;
	if( !me )
		return;

	Color_Selector::Update_Check updater = me->CheckForUpdate();
	
	me->mSelectedColorRef.SetColor( kHSBSpace, me->m_hsb );
	CSCopyColor( me->m_hsb_spectrum, me->m_hsb );

	me->HandleColorChange( SKIP_HSB );
}


void ColorPlaneSelector::RGBCallback( const MCEditText&, void* userData )
{
	ColorPlaneSelector* me = (ColorPlaneSelector*)userData;
	if( !me )
		return;

	Color_Selector::Update_Check updater = me->CheckForUpdate();

	me->mSelectedColorRef.SetColor( kRGBSpace, me->m_rgb );
	CSCopyColor( me->m_rgb_spectrum, me->m_rgb );

	me->HandleColorChange( SKIP_RGB );
}

void ColorPlaneSelector::LabCallback( const MCEditText&, void* userData )
{
	ColorPlaneSelector* me = (ColorPlaneSelector*)userData;
	if( !me )
		return;

	Color_Selector::Update_Check updater = me->CheckForUpdate();

	me->mSelectedColorRef.SetColor( kLabSpace, me->m_lab );
	CSCopyColor( me->m_lab_spectrum, me->m_lab );

	me->HandleColorChange( SKIP_LAB );
}


void ColorPlaneSelector::CMYKCallback( const MCEditText&, void* userData )
{
	ColorPlaneSelector* me = (ColorPlaneSelector*)userData;
	if( !me )
		return;

	Color_Selector::Update_Check updater = me->CheckForUpdate();

	me->mSelectedColorRef.SetColor( kCMYKSpace, me->m_cmyk );
	me->HandleColorChange( SKIP_CMYK );
}


void ColorPlaneSelector::HVCCallback( const MCEditText&, void* userData )
{
	ColorPlaneSelector* me = (ColorPlaneSelector*)userData;
	if( !me )
		return;

	Color_Selector::Update_Check updater = me->CheckForUpdate();

	me->mSelectedColorRef.SetColor( kHVCSpace, me->m_hvc );
	CSCopyColor( me->m_hvc_spectrum, me->m_hvc );

	me->HandleColorChange( SKIP_HVC );
}



void ColorPlaneSelector::RawCallback( const Raw_Text_Display& target )
{
	Color_Selector::Update_Check updater = CheckForUpdate();

	mSelectedColorRef.SetColor( kRGBSpace, target.GetColor() );
	HandleColorChange( SKIP_RAW );
}



// EOF
