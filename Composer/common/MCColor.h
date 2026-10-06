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

#ifndef _H_MCColor
#define _H_MCColor

#include "PIGeneral.h"
#include "MCLib_Wrapper.h"
#include "ADMTypes.h"
#include "SpDebug.h"


// we don't want these values to exceed the size of a byte
typedef enum
{
	kRGBSpace  = plugIncolorServicesRGBSpace,
	kHSBSpace  = plugIncolorServicesHSBSpace,
	kHSLSpace  = plugIncolorServicesHSLSpace,
	kLabSpace  = plugIncolorServicesLabSpace,
	kCMYKSpace = plugIncolorServicesCMYKSpace,
	kHVCSpace  = 10,
	kWebSpace		// treated as RGB on input
} MCSpace;


// note: this structure matches that of PhotoShop's swatch book color element on disk.
typedef struct
{
	short colorSpace;
	short components[4];
} MCPackedColor;


// this class is used as the sole color container-  don't use anything else unless you want errors in the calculations. 
class MCColor
{
	public:
		// call this only once at startup!
		static void Startup( ColorServicesProc, MCLib_Wrapper* );
	
		// Constructors:
		MCColor();				// default constructor
		~MCColor();
		
		// more constructors for ease of use
		MCColor( ASRGBColor );
		MCColor( RGBTrip );
		MCColor( HVCTrip );
		MCColor( MCSpace colorSpace, const short componentData[4] );
		
		// these are to be used for displaying the color on the screen!
		ASRGBColor		GetColorForDisplay();
		
		void			SetWebSafe( bool v ) { mWebSafe = v; }
		bool			GetWebSafe() { return mWebSafe; }
	
		// draw the color
		void			Draw( ADMDrawerRef environment, ASRect* bounds, bool drawFrame = true );
		
		// sample a color off the screen from the given coordinates...
		bool			SampleColor( ASPoint );
	
		// low level accessors
		void			SetColor( MCColor& newColor );
		void			SetColor( MCSpace colorSpace, const short componentData[4] );
		void			SetColorRGB( const short componentData[4] ) { SetColor( kRGBSpace, componentData ); }

		bool			GetColor( MCSpace desiredColorSpace, short destination_data[4], bool* printable = NULL );
		bool			GetColorIgnoreWeb( MCSpace desiredColorSpace, short destination_data[4], bool* printable = NULL );
		
		// routine used to convert our colors for photoshop
		void			GetColorForPS( MCSpace desiredColorSpace, short destination_data[4] );
		void			ConvertToPS( MCSpace desiredColorSpace, short destination_data[4] );
		
		// shortcut routines...
		MCColor			GetColorInRGB();
		MCColor			GetColorInCMYK();
		MCColor			GetColorInWeb();

		MCPackedColor	GetPackedColor();
		MCPackedColor	GetPackedColorForPS();

		// this returns the colorspace and the data untouched!
		MCSpace			GetColorNoConvert( short componentData[4] );

		// this compares two colors, if convertToRGB is true, both colors are converted to RGB and then compared
		bool			Compare( MCColor& rhs, bool convertToRGB );
		unsigned long	ColorDistance( MCColor& rhs );
		
		// misc. utilities
		MCLib_Wrapper&	GetMCLibRef() const { return *sMCLib; }

		// Color space conversions utilities -- used by the color space selector when calculating warning colors
		bool					ColorConvert( MCSpace source_space, MCSpace destination_space, const short source_data[4], short destination_data[4], bool* printable = NULL ) const;
		
		// this works for any color space
		static MCPackedColor	MakePackedColor( short r, short g, short b, short h, MCSpace colorSpace = kRGBSpace );
		
			
		// operator overloads (these are all cast operators)
						operator ASRGBColor();
						operator RGBTrip();
						operator HVCTrip();
		const MCColor&	operator=( const MCColor& rhs );		
		bool			operator==( const MCColor& rhs );		
	
	
	protected:
		// Color space conversions using photoshop interface
		void			HostColorConvert( ColorServicesInfo& convert_colors, const short source_data[4], short destination_data[4] ) const;
		
		// these conversions happen in place
		void			ConvertHVCToRGB( short srcData[4] ) const;
		void			ConvertRGBToHVC( short srcData[4] ) const;
		
		// data
		MCSpace			mSpace;
		short			mComponents[4];
		bool			mWebSafe;			// limit the colors to only web safe colors when true

	private:
		static ColorServicesProc	sConvertProc;
		static MCLib_Wrapper*		sMCLib;
};
typedef MCColor& MCColorRef;

#endif	// !_H_MCColor

// EOF
