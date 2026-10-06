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

#ifndef _H_MCEditText
#define _H_MCEditText

#include "Custom_Draw_Control.h"

typedef enum
{
	kEditType_Text = 0,		// any old text you want in there
	kEditType_RawDegrees,	// 0 - 359    <-> pass thru
	kEditType_Degrees,		// 0 - 359    [0 - 255 raw] 
	kEditType_Raw,			// 0 - 255    <-> pass thru
	kEditType_Percent,		// 0 - 100    [0 - 255 raw]
	kEditType_Signed,		// -128 - 127 [0 - 255 raw]

	// special cases herre...
	kEditType_CMYK,			// 0 - 100    [100 - 0 raw] -- special case!!
	kEditType_Munsell,		// text/number description, also a special case
	kEditType_Float,		// two significant digits (floating point) special case of munsell chroma and value

	kEditType_Accuracy,		// 0.5 - 5.0 two significant digits (floating point) [0 - 255 raw]
	kEditType_Proportion,	// 1.0 - 5.0 one significant digit (floating point)  [0 - 255 raw]
	kEditType_RawPercent,	// 0 - 100    <-> pass thru
	kEditType_Range			// 0 - 86     <-> pass thru
} MCEditType;


const short kMin_Raw = 0;
const short kMax_Raw = 255;

const short kMin_Degrees = 0;
const short kMax_Degrees = 360;

const short kMin_Signed = -128;
const short kMax_Signed = 127;

const short kMin_Percent = 0;
const short kMax_Percent = 100;

const float kMin_MunsellValue = 0.0f;
const float kMax_MunsellValue = 10.75f;

const float kMin_MunsellChroma = 0.0f;
const float kMax_MunsellChroma = 22.0f;

const float kMin_Accuracy = 0.5f;
const float kMax_Accuracy = 5.0f;

const float kMin_Proportion = 1.0f;
const float kMax_Proportion = 5.0f;

const short kMin_Range = 0;
const short kMax_Range = 86;

const short kPaletteDefault_Range      = 20;
const float kPaletteDefault_Accuracy   = kMin_Accuracy;
const short kPaletteDefault_Depth      = kMax_Percent;
const float kPaletteDefault_Proportion = kMin_Proportion;


// this is the external storage type...
typedef short MCRawValue;
typedef float MCFloatValue;

class ComposerUI;
class MCEditText;

typedef void (*ColorTextCallback)( const MCEditText& target, void* userData );


class MCEditText : public Custom_Draw_Control
{
	typedef Custom_Draw_Control base;

	public:
		// construction
		MCEditText( ComposerUI& ui, MCEditType type, ASInt32 control_id, MCRawValue* rawValuePtr, ColorTextCallback updateCallback, void* userData, ColorTextCallback finishedCallback = NULL );
		MCEditText( ComposerUI& ui, MCEditType type, ASInt32 control_id, MCFloatValue* rawValuePtr, ColorTextCallback updateCallback, void* userData, ColorTextCallback finishedCallback = NULL );
		virtual	~MCEditText();

		// overrides
		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		HandleNotify( ADMNotifierRef notifier );
		virtual void		Draw( ADMDrawerRef drawer );
		virtual void		set_visible( const bool show );

		// in our case this method causes this control to look at it's external value and set it in the control display
		virtual void		Update();
		
		// used for the floating point special case only... bcause they have different min/max's
		virtual void		SetMinMax( float min, float max );
		
		// change the item type
		virtual bool		SetType( MCEditType );
		virtual MCEditType	GetType();
		
		// call this to turn on the popup slider feature...
		virtual void		AllowPopupSlider( bool );
		
		virtual bool		IsBoldStyle();
		virtual void		SetBoldStyle( bool b );
		
				
	protected:
		// convert text to a value - min <= value <= max
		ASInt32				GetTextValue();
		void				SetTextValue( ASInt32 newValue );
		
		// the raw value comes from the passed in pointer at construction and is in the range 0 - 255
		MCRawValue			GetRawValue();
		void				SetRawValue( MCRawValue newValue );
		
		// convert a display value to the raw form
		MCRawValue			ValueToRaw( ASInt32 newValue );
		ASInt32				RawToValue( MCRawValue newValue );
		
		////////////////////////////////////////////////////////////////////////////////////
		// convert to a munsell text name...
		void				ValueToMunsellText( ASInt32 hueValue );
		ASInt32				MunsellTextToValue( const char* buffer );
	
		void				ValueToProportionText( float value );

		void				ValueToMappedFloat( ASInt32 value );
		ASInt32				MappedFloatToValue( float value );

		// floating point versions...
		float				GetFloatTextValue();
		void				SetFloatTextValue( float newValue );
		
		////////////////////////////////////////////////////////////////////////////////////
		
		// these set the referenced value
		float				GetFloatValue();
		void				SetFloatValue( float newValue );
		
		void				SetSliderPosition( float newValue );
		float				GetSliderPosition();
		
		////////////////////////////////////////////////////////////////////////////////////
		// callbacks - I think that ASAPI syntax is in the wrong place... !!@
		static void			ASAPI	SliderNotify( ADMItemRef, ADMNotifierRef notifier );
		static ASBoolean	ASAPI	FloatToTextProc( ADMItemRef inItem, float inValue, char* outText, ASInt32 inMaxLength );
		static ASBoolean	ASAPI	TextToFloatProc( ADMItemRef inItem, const char* inText, float* outValue );

	private:
		MCRawValue*			mValuePtr;
		MCFloatValue*		mFloatPtr;
		ColorTextCallback	mCallback;
		ColorTextCallback	mFinishCB;
		void*				mUserData;
		MCEditType			mType;
		float				mMin;
		float				mMax;
		bool				mUseFloatingValue;
		ADMItemRef			mItemRef;		
};


#endif // !_H_MCEditText

// EOF