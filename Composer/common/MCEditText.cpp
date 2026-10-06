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

#include "MCEditText.h"
#include "Composer_UI.h"

#if !WIN32 
// automatically included on Windows...
#include "Composer-sym.h"
#endif


#include "admBasic.h"
#include <math.h>
#ifndef _H_SpDebug
#include "SpDebug.h"
#endif


const ASInt32 kMaxText        = 3;
const ASInt32 kBufferSize     = 36;		// 8 numbers max, room for a negative sign, and decimal point and null byte
const ASInt32 kNumMunsellHues = 10;
const ASInt32 kPopupControlWidth = 16;

const MCFloatValue kPopupSliderMin = 0;
const MCFloatValue kPopupSliderMax = 100;

const char* kMCEditText_MunsellHueFormat		 = "%s %0.2f";
const char* kMCEditText_FloatFormat				 = "%0.2f";
const char* kMCEditText_ProportionFormat		 = "1:%0.1f";
const char* kMCEditText_ProportionFormatNoPlaces = "1:%0.0f";


static const char* kMunsellNames[kNumMunsellHues] = 
{ 
	"R ",	// 0 
	"YR ",	// 1
	"Y ",	// 2
	"GY ",	// 3
	"G ",	// 4
	"BG ",	// 5
	"B ",	// 6
	"PB ",	// 7
	"P ",	// 8
	"RP "	// 9
};




MCEditText::MCEditText( ComposerUI& ui, MCEditType type, ASInt32 control_id, MCRawValue* valuePtr, ColorTextCallback updateCallback, void* userData, ColorTextCallback finishedCallback ) :
	base( ui, control_id ), 
	mValuePtr( valuePtr ), 
	mFloatPtr( NULL ),
	mCallback( updateCallback ),
	mFinishCB( finishedCallback ),
	mUserData( userData ),
	mType( type ),
	mMin( 0 ),
	mMax( 0 ),
	mUseFloatingValue( false ),
	mItemRef( NULL )
{
	// call SetType, it sets the min and max
	SetType( mType );

	switch( mType )
	{
		case kEditType_Accuracy:
		case kEditType_Proportion:
			XDEBUG_PRINT( "You are using the wrong constructor for this edit type\n" );
			break;
	}
	
	// start off with fairly correct looking values.
	Update();
}


MCEditText::MCEditText( ComposerUI& ui, MCEditType type, ASInt32 control_id, MCFloatValue* valuePtr, ColorTextCallback updateCallback, void* userData, ColorTextCallback finishedCallback ) :
	base( ui, control_id ), 
	mValuePtr( NULL ), 
	mFloatPtr( valuePtr ), 
	mCallback( updateCallback ),
	mFinishCB( finishedCallback ),
	mUserData( userData ),
	mType( type ),
	mMin( 0 ),
	mMax( 0 ),
	mUseFloatingValue( true ),
	mItemRef( NULL )
{
	// call SetType, it sets the min and max
	SetType( mType );

	switch( mType )
	{
		case kEditType_Text:
		case kEditType_Munsell:
		case kEditType_Degrees:
		case kEditType_RawDegrees:
		case kEditType_Raw:
		case kEditType_RawPercent:
		case kEditType_Percent:
		case kEditType_CMYK:
		case kEditType_Signed:
		case kEditType_Float:
		case kEditType_Range:
			XDEBUG_PRINT( "You are using the wrong constructor for this edit type\n" );
			break;
	}
	
	// start off with fairly correct looking values.
	Update();
}



MCEditText::~MCEditText() 
{
	// this will dispose the popup if we have one...
	AllowPopupSlider( false );
}




ASBoolean MCEditText::HandleTracking( ADMTrackerRef tracker )
{
	ASBoolean result = base::HandleTracking( tracker );
	
	// let's also take a look at the data...
	if( ADM_Access::tracking_suite()->TestAction( tracker, kADMKeyStrokeAction ) )
	{
		// convert value in edit box to raw and notify the callback
		if( mUseFloatingValue )
			SetFloatValue( GetFloatTextValue() );
		else
			SetRawValue( ValueToRaw( GetTextValue() ) );
		
		if( mCallback )
			mCallback( *this, mUserData );
	}
	return result;
}


void MCEditText::HandleNotify( ADMNotifierRef notifier )
{
	// here we are being told that the text changed.. so get the new value and update the external raw value
	if( ADM_Access::notifier_suite()->IsNotifierType( notifier, kADMUserChangedNotifier ) )
	{
		// convert value in edit box to raw and notify the callback
		if( mUseFloatingValue )
			SetFloatValue( GetFloatTextValue() );
		else
			SetRawValue( ValueToRaw( GetTextValue() ) );
		
		if( mFinishCB )
			mFinishCB( *this, mUserData );
	}
}


void MCEditText::Draw( ADMDrawerRef drawer )
{
	if( mItemRef )
	{
		// !!@  NOTE: this is Windows native code here because ADM sucks... 
#if WIN32
		// on Windows we need to get the control's window DC
		HDC dc = GetDCEx( (HWND)Control_Interface( mItemRef ).GetWindowRef(), NULL, DCX_PARENTCLIP );
		
		if( dc )
		{
			ASRect asRect = Control_Interface( mItemRef ).bounds( true );

			// !!@ this should use the ADM color for the brush color, this was determined empirically
			HBRUSH myBrush = CreateSolidBrush( RGB( 166, 167, 170 ) );
			
			if( myBrush )
			{
				// frame the control (but outside of its client rectangle)
				InflateRect( (RECT*)&asRect, 1, 1 );
				FrameRect( dc, (RECT*)&asRect, myBrush );
				DeleteObject( myBrush );
			}

			// let go of the DC
			ReleaseDC( NULL, dc );
		}
#endif
	}
	
	// do default drawing
	ADM_Access::item_suite()->DefaultDraw( native(), drawer );
}


void MCEditText::set_visible( const bool show )
{
	if( mItemRef )
	{
		// let's shide our popup too
		Control_Interface( mItemRef ).set_visible( show );
		
		// we need to erase the frame around the popup when hidden
		if( !show && GetComposerUI() )
		{
			ASRect asRect = Control_Interface( mItemRef ).bounds();
#if WIN32			
			InflateRect( (RECT*)&asRect, 1, 1 );
#endif
			GetComposerUI()->InvalidateRect( &asRect );
		}
	}
	
	base::set_visible( show );
}


void MCEditText::Update()
{
	// here we take our external raw value and set the display value from it
	if( mUseFloatingValue )
		SetFloatTextValue( GetFloatValue() );
	else
		SetTextValue( RawToValue( GetRawValue() ) );
	
	// if we have a popup slider we need to set the slider position	
	if( mItemRef )
	{
		if( mUseFloatingValue )
			SetSliderPosition( GetFloatValue() );
		else
			SetSliderPosition( (float)RawToValue( GetRawValue() ) );
	}
}


// special case floating point setter
void MCEditText::SetMinMax( float min, float max )
{
	mMin = min;
	mMax = max;
		
	set_min_float_value( mMin );
	set_max_float_value( mMax );
}


bool MCEditText::SetType( MCEditType newType )
{
	bool setMinMax = false;

	mType = newType;
	
	// now do something
	// set default units...
	set_max_text_length( kMaxText );

	switch( mType )
	{
		case kEditType_Text:
			// do not set anything like units or min and max, otherwise text will be rejected!
			break;

		case kEditType_Munsell:
			// munsell text needs alot more space -- see comment where bufferSize is defined for more info
			set_max_text_length( kBufferSize );	
			break;
			
		case kEditType_Degrees:
		case kEditType_RawDegrees:
			mMin = kMin_Degrees;
			mMax = kMax_Degrees;
			set_units( kADMDegreeUnits );
			setMinMax = true;
			break;
			
		case kEditType_Raw:
			set_units( kADMNoUnits );
			mMin = kMin_Raw;
			mMax = kMax_Raw;
			setMinMax = true;
			break;
			
		case kEditType_RawPercent:
		case kEditType_Percent:
		case kEditType_CMYK:
			mMin = kMin_Percent;
			mMax = kMax_Percent;
			set_units( kADMPercentUnits );
			setMinMax = true;
			break;
			
		case kEditType_Signed:
			// allow a negative sign
			set_max_text_length( kMaxText + 1 );	
			mMin = kMin_Signed;
			mMax = kMax_Signed;
			set_units( kADMNoUnits );
			setMinMax = true;
			break;

		case kEditType_Float:
			// don't set the min and max, you are expected to do that first!

			// floating text 0.00 to -00.00
			set_max_text_length( kMaxText + 3 );
			set_units( kADMNoUnits );
			break;
		
		// 0 - 86     <-> pass thru
		case kEditType_Range:
			mMin = kMin_Range;
			mMax = kMax_Range;
			set_units( kADMNoUnits );
			setMinMax = true;
			break;
			
		// 0.50 - 5.00 two significant digits (floating point) [0 - 255 raw]
		case kEditType_Accuracy:
			SetMinMax( kMin_Accuracy, kMax_Accuracy );	

			// floating text 0.00 to -00.00
			set_max_text_length( kMaxText + 3 );
			set_units( kADMNoUnits );
			break;
			
		// 1.0 - 5.0 one significant digit (floating point)  [0 - 255 raw]
		case kEditType_Proportion:
			SetMinMax( kMin_Proportion, kMax_Proportion );	
			
			// floating text 1:0.0 to 1:0.0
			set_max_text_length( kMaxText + 4 );
			
			// we need this proc in order to do the proper conversions...
			SetFloatToTextProc( FloatToTextProc );
			SetTextToFloatProc( TextToFloatProc );
			set_units( kADMNoUnits );
			break;
			
		default:
			return false;
	}
	
	if( setMinMax )
	{
		// truncate to integer
		set_min_int_value( (ASInt32)mMin );
		set_max_int_value( (ASInt32)mMax );	
	}
	
	return true;
}


MCEditType MCEditText::GetType()
{
	return mType;
}


void MCEditText::AllowPopupSlider( bool allowed )
{
	// if we are turning off the slider we should dispose and clear out the stuff we are using
	if( !allowed )
	{
		if( GetComposerUI() && GetComposerUI()->GetNative() && mItemRef )
		{
			// handle changing the rectangle back to its original size
			// !!@
			
			// !!@ what's up with this?  Calling destroy causes a crash... argh.		
//			ADM_Access::dialog_suite()->DestroyItem( GetComposerUI()->GetNative(), mItemRef );
			mItemRef = NULL;
		}
	
		return;
	}
	
	// funky I know...
	if( !GetComposerUI() )
		return;
		
	// attempt to create the popup item if we don't already have one...	
	if( !mItemRef )
	{
		// adjust the bounds so we are sitting on the right side of the control...
		ASRect boundsRect = bounds();
		
		boundsRect.left = boundsRect.right - kPopupControlWidth;
		++boundsRect.top;
		--boundsRect.right;
		--boundsRect.bottom;
		
		// get the item and position it properly...		
		mItemRef = ADM_Access::dialog_suite()->GetItem( GetComposerUI()->GetNative(), id() + kPopupIDOffset );
		
		// set item's min and max (and rectangle)
		if( mItemRef )
		{
			Control_Interface pop( mItemRef );

			// set the callback to point to us...
			pop.set_user_data( this );
			pop.set_proc( SliderNotify );
			
			// adjust the popup bounds...
			pop.set_bounds( boundsRect );
			
			// now adjust our bounds...
			boundsRect = bounds();
			boundsRect.right -= kPopupControlWidth;
			set_bounds( boundsRect );
			
			pop.SetItemStyle( kADMBottomPopupMenuStyle );
			pop.Enable( true );
			
			pop.set_min_float_value( kPopupSliderMin );
			pop.set_max_float_value( kPopupSliderMax );
		}		
	}	
}


void MCEditText::SetSliderPosition( float newValue )
{
	if( !mItemRef )
		return;
		
	// newValue is assumed to be in the range of mMin >= newValue >= mMax
	// it needs to be mapped to the slider range of 0 - 100
	float range       = mMax - mMin;
	float sliderValue = (newValue - mMin) * (kPopupSliderMax - kPopupSliderMin) / range;

	Control_Interface( mItemRef ).set_float_value( sliderValue );
}


float MCEditText::GetSliderPosition()
{
	if( !mItemRef )
		return 0.0f;

	// the return value is assumed to be in the range of mMin >= newValue >= mMax
	float range       = mMax - mMin;
	float sliderValue = mMin + (Control_Interface( mItemRef ).float_value() * range / kPopupSliderMax);
		
	return sliderValue;
}


void ASAPI MCEditText::SliderNotify( ADMItemRef itemRef, ADMNotifierRef notifier )
{
	// gain access to ourselves
	MCEditText* me = (MCEditText*)ADM_Access::item_suite()->GetUserData( itemRef );
	if( !me )
		return;
		
	if( ADM_Access::notifier_suite()->IsNotifierType( notifier, kADMIntermediateChangedNotifier ) )
	{
		if( me->mItemRef )
		{
			if( me->mUseFloatingValue )
				me->SetFloatValue( me->GetSliderPosition() );
			else
				me->SetRawValue( me->ValueToRaw( (ASInt32)me->GetSliderPosition() ) );
			
			// we must call the notifier otherwise nothing happens on the UI				
			if( me->mCallback )
				me->mCallback( *me, me->mUserData );
			
			// this allows for live update
			if( me->mFinishCB )
				me->mFinishCB( *me, me->mUserData );
		}
	}
}



ASBoolean ASAPI MCEditText::FloatToTextProc( ADMItemRef inItem, float inValue, char* outText, ASInt32 )
{
	if( !outText )
		return false;
		
	// start with a null string
	outText[0] = 0;

	// gain access to ourselves
	MCEditText* me = (MCEditText*)ADM_Access::item_suite()->GetUserData( inItem );
	if( !me )
		return false;

	if( me->mType == kEditType_Proportion )
	{
		// look at this floating point number, if it has a fractional part then use a format that shows some of it
		float integer = 0;
		float fraction = ::modff( inValue, &integer );
		
		// form a string in this format "1:0.0"
		if( fraction )
			::sprintf( outText, kMCEditText_ProportionFormat, inValue );
		else
			::sprintf( outText, kMCEditText_ProportionFormatNoPlaces, inValue );
	}
	else
	{
		// default case -- not used...
		::sprintf( outText, kMCEditText_FloatFormat, inValue );
	}
	
	return true;
}


ASBoolean ASAPI MCEditText::TextToFloatProc( ADMItemRef inItem, const char* inText, float* outValue )
{
	if( !outValue )
		return false;

	// gain access to ourselves
	MCEditText* me = (MCEditText*)ADM_Access::item_suite()->GetUserData( inItem );
	if( !me )
		return false;
	
	if( me->mType == kEditType_Proportion )
	{
		// default with our mininum value...
		*outValue = me->mMin;

		// parse text for the colon, if there is one grab text after it
		const char* foundStr = ::strchr( inText, ':' );
		
		// if we didn't find the colon just reset the string, we'll convert the whole thing.
		if( !foundStr )
			foundStr = inText;
		else
			++foundStr;	// point to after the colon
		
		return ADM_Access::basic_suite()->StringToValue( foundStr, outValue, kADMNoUnits );
	}
	
	return false;
}


bool MCEditText::IsBoldStyle()
{ 
	return GetFont() == kADMBoldDialogFont; 
}

void MCEditText::SetBoldStyle( bool b )
{ 
	if( b )
		SetFont( kADMBoldDialogFont );
	else
		SetFont( kADMDialogFont ); //kADMDefaultFont
}



///////////////////////////////////////////////////////////////////////////////////////


// the raw value comes from the passed in pointer at construction and is in the range 0 - 255
MCRawValue MCEditText::GetRawValue()
{
	// just check to make sure we have a valid pointer
	if( mValuePtr )
		return *mValuePtr;
	
	return 0;
}

// raw values are always in the range of 0-255
void MCEditText::SetRawValue( MCRawValue newValue )
{
	if( mValuePtr )
		*mValuePtr = newValue;
	else
		XDEBUG_PRINT( "missing raw value pointer!!\n" );
}


ASInt32 MCEditText::GetTextValue()
{
	// grab the text out of the edit text control
	char buffer[kBufferSize] = {0};
	text( buffer, kBufferSize );
	
	// for munsell text we just use the text buffer directly and parse it
	if( mType == kEditType_Munsell )
		return MunsellTextToValue( buffer );

	// othewise we convert the value to floating point and the default case truncates it to integer	
	float value = 0.0f;
	
	ASBoolean success = ADM_Access::basic_suite()->StringToValue( buffer, &value, kADMNoUnits );
	if( !success )
		return 0;
	
	// convert to Raw from float...		
	if( mType == kEditType_Float )
		return MappedFloatToValue( value );
		
	// truncate float !!@ (we may want to use rounding or make it an option)
	return (ASInt32)value;
}


void MCEditText::SetTextValue( ASInt32 newValue )
{
	switch( mType )
	{
		case kEditType_Munsell:
			// here we take the value (which is in degrees 0 - 359)
			// and map it into a textual representation of the hue.
			ValueToMunsellText( newValue );
			break;
		
		case kEditType_Float:
			// in this case we take the value (which is raw 0 - 255)
			// and map it into mMin to mMax range (in floating point)
			ValueToMappedFloat( newValue );
			break;
			
		default:
			set_int_value( newValue );			
	}
}



float MCEditText::GetFloatTextValue()
{
	// grab the text out of the edit text control
	char buffer[kBufferSize] = {0};
	text( buffer, kBufferSize );
	
	if( mType == kEditType_Proportion )
	{
		// this gets automatically converted by the text to float proc...
		return float_value();
	}

	// othewise we convert the value to floating point and the default case truncates it to integer	
	float value = 0.0f;
	
	ASBoolean success = ADM_Access::basic_suite()->StringToValue( buffer, &value, kADMNoUnits );
	if( !success )
		return 0;
		
	return value;
}


void MCEditText::SetFloatTextValue( float newValue )
{
	switch( mType )
	{
		case kEditType_Proportion:
			ValueToProportionText( newValue );
			break;
				
		default:
			// form a string in this format "0.00"
			char buffer[kBufferSize] = {0};
			::sprintf( buffer, kMCEditText_FloatFormat, newValue );
	
			// shove that into the edit control
			set_text( buffer );
			
//			// this doesn't keep the format of the float properly...
//			set_float_value( newValue );			
	}
}


float MCEditText::GetFloatValue()
{
	// just check to make sure we have a valid pointer
	if( mFloatPtr )
		return *mFloatPtr;
	
	return 0.0f;
}

void MCEditText::SetFloatValue( float newValue )
{

	if( mFloatPtr )
		*mFloatPtr = newValue;
	else
		XDEBUG_PRINT( "missing floating point value pointer!!\n" );
}




///////////////////////////////////////////////////////////////////////////////////////////////

MCRawValue MCEditText::ValueToRaw( ASInt32 newValue )
{
	MCRawValue rawValue = 0;

	switch( mType )
	{
		// pass thru 0 - 255 value
		case kEditType_Raw:
		case kEditType_Float:
		case kEditType_RawPercent:
		case kEditType_Range:
		case kEditType_Proportion:
			rawValue = (MCRawValue)newValue & 0xFF;
			break;
		
		case kEditType_Percent:
			rawValue = (MCRawValue)expand_percent( (short)newValue );
			break;
		
		// cmyk values are inverted...
		case kEditType_CMYK:
			rawValue = (MCRawValue)(RAW_MAX - expand_percent( (short)newValue ));
			break;

		case kEditType_Degrees:
		case kEditType_Munsell:
			// make sure that a hue of 360 gets mapped to 0
			if( newValue == kMax_Degrees )
			{
				// update the text display...
				newValue = 0;
				SetTextValue( newValue );	
			}
			rawValue = (MCRawValue)expand_hue( (short)newValue );
			break;


		case kEditType_RawDegrees:
			// make sure that a hue of 360 gets mapped to 0
			if( newValue == kMax_Degrees )
			{
				// update the text display...
				newValue = 0;
				SetTextValue( newValue );	
			}
			rawValue = (MCRawValue)newValue;
			break;
			
		case kEditType_Signed:
			rawValue = (MCRawValue)expand_lab( (short)newValue );
			break;
	}	

	return rawValue;
}



ASInt32 MCEditText::RawToValue( MCRawValue newValue )
{
	ASInt32 resultValue = 0;

	switch( mType )
	{
		// we just pass thru the values untouched for raw
		case kEditType_Raw:
		case kEditType_Float:
		case kEditType_RawPercent:
		case kEditType_Range:
		case kEditType_Proportion:
			resultValue = newValue;
			break;
			
		case kEditType_Percent:
			resultValue = limit_percent( newValue );
			break;

		case kEditType_CMYK:
			resultValue = limit_percent( RAW_MAX - newValue );
			break;

		case kEditType_Degrees:
		case kEditType_Munsell:
		{
			// in the case of degrees we need to map 0 and 360 to the same number, they are the same angle
			resultValue = limit_hue( newValue );
			if( resultValue == kMax_Degrees )
				resultValue = 0;
		}
		break;
		
		case kEditType_RawDegrees:
		{
			// raw = pass thru
			resultValue = newValue;				
			
			// in the case of degrees we need to map 0 and 360 to the same number, they are the same angle
			if( resultValue == kMax_Degrees )
				resultValue = 0;
		}
		break;

		case kEditType_Signed:
			resultValue = limit_lab( newValue );
			break;
	}

	return resultValue;
}


// This sets the HVC munsell text
void MCEditText::ValueToMunsellText( ASInt32 hueValue )
{
	float hue = (float)hueValue * kNumMunsellHues / (float)kMax_Degrees;
	
	// calculate index by rounding then truncating...
	ASInt32 hueIndex = (ASInt32)hue;	
	
	// !!@ error case ???
	if( hueIndex >= kNumMunsellHues || hueIndex < 0 )
		return; 
	
	// Copy the Munsell base color name
	float hue_remainder = hue - hueIndex;

	// form a string in this format "color name 00.00"
	char buffer[kBufferSize] = {0};
	::sprintf( buffer, kMCEditText_MunsellHueFormat, kMunsellNames[hueIndex], hue_remainder * kNumMunsellHues );
	
	// shove that into the edit control
	set_text( buffer );
}

// !!@ crudely implemented (better than the 1.3 version which was not implemented)
ASInt32	MCEditText::MunsellTextToValue( const char* buffer )
{
	// first find the hue name by finding the first space
	const char* space = ::strchr( buffer, ' ' );
	if( !space )
		return 0;	// bad format
	
	// grab the hue name -- copy from the beginning to after the space
	char hueName[64] = {0};
	
	::memcpy( hueName, buffer, space - buffer + 1 );
	const char* hueRemainder = space + 2;

	// find hue in the list... this gives us an index number
	int index = 0; 
	
	for( int i = 0; i < kNumMunsellHues; i++ )
	{
		if( ::strcmp( kMunsellNames[i], hueName ) == 0 )
		{
			index = i;
			break;
		}
	}
	
	// reconstruct full hue angle
	float hueAngle = (float)index / kNumMunsellHues * (float)kMax_Degrees;
	float remain   = 0.0f; 
	ADM_Access::basic_suite()->StringToValue( hueRemainder, &remain, kADMNoUnits );
	
	float final = hueAngle + ((remain / kNumMunsellHues) * (kMax_Degrees / kNumMunsellHues));
	XDEBUG_PRINT3( "munsell hue angle: %f, remain: %f, final: %f\n", hueAngle, remain, final );
	
	// round result up
	return (ASInt32)(final + 0.5f);
}



void MCEditText::ValueToProportionText( float value )
{
	char buffer[kBufferSize] = {0};
	
	// look at this floating point number, if it has a fractional part then use a format that shows some of it
	float integer = 0;
	float fraction = ::modff( value, &integer );
	
	// form a string in this format "1:0.0"
	if( fraction )
		::sprintf( buffer, kMCEditText_ProportionFormat, value );
	else
		::sprintf( buffer, kMCEditText_ProportionFormatNoPlaces, value );
	
	// shove that into the edit control
	set_text( buffer );
}



// This sets the HVC value and chroma displays
void MCEditText::ValueToMappedFloat( ASInt32 value )
{
	// calculate range
	float range = mMax - mMin;

	float floatingValue = (float)value * range / kMax_Raw;
	
	// form a string in this format "-00.00"
	char buffer[kBufferSize] = {0};
	::sprintf( buffer, kMCEditText_FloatFormat, floatingValue );
	
	// shove that into the edit control
	set_text( buffer );
}


ASInt32 MCEditText::MappedFloatToValue( float value )
{
	// convert from min to max to 0 - 255
	float range = mMax - mMin;

	float rawValue = value * kMax_Raw / range;
	
	// round to 16 bits
	return (short)(rawValue + 0.5f);
}


// EOF
