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

#include "PhotoShopSDK.h"
#include "PIUtilities.h"

#include "MCColor.h"
#include "MCEditText.h"

#include "Helpers.h"

#include "ADM_Access.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ColorServicesProc	MCColor::sConvertProc	= NULL;
MCLib_Wrapper*		MCColor::sMCLib			= NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




MCColor::MCColor() : 
	mSpace( kRGBSpace ), 
	mWebSafe( false )
{
	mComponents[0] = mComponents[1] = mComponents[2] = mComponents[3] = 0;

#if XDEBUG
	// assert that the statics are initialized!!!
	if( !sMCLib )
		XDEBUG_PRINT( "MCLib reference is not initialized\n" );
		

	if( !sConvertProc )
		XDEBUG_PRINT( "There is no color conversion proc!\n" );
#endif
}


MCColor::~MCColor()
{
}
	

MCColor::MCColor( ASRGBColor asRGB ) : 
	mSpace( kRGBSpace ), 
	mWebSafe( false )
{
	// limit the RGB to 8 bits and then set internal color
	short rgb[4] = {0};
	
	rgb[0] = limit_rgb( asRGB.red ); 
	rgb[1] = limit_rgb( asRGB.green ); 
	rgb[2] = limit_rgb( asRGB.blue ); 
	SetColorRGB( rgb );
}


MCColor::MCColor( RGBTrip rgbTrip ) : 
	mSpace( kRGBSpace ), 
	mWebSafe( false )
{
	short rgb[4] = {0};
	
	UnpackRGB( rgbTrip, rgb );
	SetColorRGB( rgb );
}


MCColor::MCColor( HVCTrip hvcTrip ) : 
	mSpace( kHVCSpace ), 
	mWebSafe( false )
{
	short hvc[4] = {0};
	
	UnpackHVC( hvcTrip, hvc );
	SetColor( kHVCSpace, hvc );
}


MCColor::MCColor( MCSpace colorSpace, const short componentData[4] ) : 
	mSpace( colorSpace ), 
	mWebSafe( false )
{
	SetColor( colorSpace, componentData );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MCColor::Startup( ColorServicesProc proc, MCLib_Wrapper* mcLib )
{
	sConvertProc = proc;
	sMCLib = mcLib;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



ASRGBColor MCColor::GetColorForDisplay()
{
	// form an RGB for display
	short temp[4] = {0};
	ASRGBColor rgb = {0};
	
	// grab color in RGB
	GetColor( kRGBSpace, temp );
	rgb.red   = expand_rgb( temp[0] );
	rgb.green = expand_rgb( temp[1] );
	rgb.blue  = expand_rgb( temp[2] );
	
	return rgb;
}



void MCColor::Draw( ADMDrawerRef environment, ASRect* bounds, bool drawFrame )
{
	ASRect displayBounds = {0};

	if( !bounds )
		ADM_Access::drawing_suite()->GetBoundsRect( environment, &displayBounds );
	else
		displayBounds = *bounds;
	
	if( drawFrame )
	{
		ASRGBColor frame_color = {0};	// black
		ADM_Access::drawing_suite()->SetRGBColor( environment, &frame_color );
		ADM_Access::drawing_suite()->DrawRect( environment, &displayBounds );
		
		// now inset the fill rectangle
		displayBounds.top    += 1;
		displayBounds.left   += 1;
		displayBounds.bottom -= 1;
		displayBounds.right  -= 1;
	}
		
	// now fill in the color
	ASRGBColor rgbColor = GetColorForDisplay();
	ADM_Access::drawing_suite()->SetRGBColor( environment, &rgbColor );
	ADM_Access::drawing_suite()->FillRect( environment, &displayBounds );
}


bool MCColor::SampleColor( ASPoint samplePoint )
{
	Point				sampleLoc	= {0};
	ColorServicesInfo	info		= {0};

	info.infoSize = sizeof( ColorServicesInfo );
	info.selector = plugIncolorServicesSamplePoint;

	sampleLoc.v = (short)samplePoint.v;
	sampleLoc.h = (short)samplePoint.h;
	info.selectorParameter.globalSamplePoint = &sampleLoc;
	
	// umm call the function...
	OSErr err = (*sConvertProc)( &info );				
	
	if( err == errInvalidSamplePoint )
		return false;
	
	// Save the results
	SetColor( MCSpace( info.resultSpace ), info.colorComponents );
	
	return true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Convert the source color to the destination colorspace
bool MCColor::ColorConvert( MCSpace source_space, MCSpace destination_space, const short source_data[4], short destination_data[4], bool* printable ) const
{
//	XDEBUG_PRINT2( "source: %d, dest: %d\n", source_space, destination_space );
//	XDEBUG_PRINT4( "source: %d %d %d %d\n", source_data[0], source_data[1], source_data[2], source_data[3] );
	
	// Just copy the color if the colorspaces are the same and then bail.
	if( source_space == destination_space )
	{
		CSCopyColor( destination_data, source_data );
		
		if( printable )
			*printable = true;
			
		return true;	// everything is valid...
	}
	
	// check all special cases -- if the input is RGB and we want HVC then do it the easy way
	if( source_space == kRGBSpace && destination_space == kHVCSpace )
	{
		// conversion is in place, so...
		destination_data[0] = source_data[0];
		destination_data[1] = source_data[1];
		destination_data[2] = source_data[2];
		
		ConvertRGBToHVC( destination_data );

		if( printable )
			*printable = true;
			
		return true;	// everything is valid...
	}


	ColorServicesInfo convertInfo = {0};
	convertInfo.infoSize = sizeof( ColorServicesInfo );
	convertInfo.selector = plugIncolorServicesConvertColor;
	
	// check to see if the destination should be in HVC, in this case we 
	// need to make our destination be RGB and then go from there...
	if( destination_space == kHVCSpace )
	{
		// Use the Photoshop colorServices to go to RGB then go to HVC
		convertInfo.sourceSpace = source_space;
		convertInfo.resultSpace = plugIncolorServicesRGBSpace;

		HostColorConvert( convertInfo, source_data, destination_data );
		
		// okay to go to HVC
		ConvertRGBToHVC( destination_data );
		goto checkGamut;
	}
	
	// check to see if we are converting an HVC color because if we are,
	// we need to first convert the HVC to RGB then go from there...  in the case of destination being RGB the convert is just a copy...
	if( source_space == kHVCSpace )
	{
		short hvcData[4] = {0};
		
		CSCopyColor( hvcData, source_data );
		
		// NOTE: this converts data in place
		ConvertHVCToRGB( hvcData );

		// set to RGB, do conversion and then bail
		convertInfo.sourceSpace = plugIncolorServicesRGBSpace;
		convertInfo.resultSpace = destination_space;
		
		// if the destination is RGB we are done here...
		if( destination_space != kRGBSpace )
			HostColorConvert( convertInfo, hvcData, destination_data );
		else
		{
			// copy the color to the destination and then bail
			CSCopyColor( destination_data, hvcData );
			
			// mark valid result
			convertInfo.resultInGamut = convertInfo.resultGamutInfoValid = true;	
		}
		goto checkGamut;
	}

	// default case that doesn't involve HVC colors -- use the Photoshop colorServices for conversion
	convertInfo.sourceSpace = source_space;
	convertInfo.resultSpace = destination_space;
	HostColorConvert( convertInfo, source_data, destination_data );

checkGamut:
	// Return additional color data
	if( printable )
		*printable = convertInfo.resultInGamut == true;		
	
	// return whether or not result gamma data is valid
	return convertInfo.resultGamutInfoValid == true;
}




void MCColor::ConvertHVCToRGB( short srcData[4] ) const
{
	// I'm not sure if the MCLib routine can do the conversion in place, so here you go:
	RGBTrip rgb = {0};
	HVCTrip hvc = {0};
	
	PackHVC( srcData, hvc );
	
	GetMCLibRef().hvc_2_rgb( hvc, rgb );

	srcData[0] = rgb.red;
	srcData[1] = rgb.green;
	srcData[2] = rgb.blue;
	
	// we don't touch the last value...
}



void MCColor::ConvertRGBToHVC( short srcData[4] ) const
{
	// I'm not sure if the MCLib routine can do the conversion in place, so here you go:
	HVCTrip hvc = {0};
	RGBTrip rgb = { (unsigned char)srcData[0], (unsigned char)srcData[1], (unsigned char)srcData[2] };
	
	GetMCLibRef().rgb_2_hvc( rgb, hvc );

	srcData[0] = hvc.hue;
	srcData[1] = hvc.value;
	srcData[2] = hvc.chroma;
	
	// we don't touch the last value...
}



// Call the Photoshop colorServices to convert the color data
void MCColor::HostColorConvert( ColorServicesInfo& convert_colors, const short source_data[4], short destination_data[4] ) const
{
	// Initialize the conversion
	CSCopyColor( convert_colors.colorComponents, source_data );	
	
	// Convert the color by using the host's conversion function pointer
	(*sConvertProc)( &convert_colors );				
	
	// Save the results
	CSCopyColor( destination_data, convert_colors.colorComponents );	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void MCColor::SetColor( MCColorRef newColor )
{
	// copy data...
	*this = newColor;
	
	// check the color space...
	if( mSpace == kWebSpace )
		mSpace = kRGBSpace;		// web space is RGB...
}


void MCColor::SetColor( MCSpace colorSpace, const short componentData[4] )
{
	// always copy the color data...  don't convert it!  this happens on retrieval...
	mSpace = colorSpace;
	CSCopyColor( mComponents, componentData );
	
	// web space is RGB...
	if( mSpace == kWebSpace )
		mSpace = kRGBSpace;		
}


// returns the color in the desired color space by possibly converting it first
bool MCColor::GetColor( MCSpace desiredColorSpace, short destination_data[4], bool* printable )
{
	// if we are set to web safe colors only then we convert the color to rgb, then make it safe
	// then return the color in whatever space it was asked for...
	if( mWebSafe || desiredColorSpace == kWebSpace )
	{
		// reset the conversion space if necessary
		if( desiredColorSpace == kWebSpace )
			desiredColorSpace = kRGBSpace;
			
		short   webRGB[4] = {0};
		MCSpace currentSpace = GetColorNoConvert( webRGB );
		
		// go to RGB with the color...
		ColorConvert( currentSpace, kRGBSpace, mComponents, webRGB );
				
		// perform conversion to web safe
		for( int i = 0; i < 4; i++ )
			webRGB[i] = CalculateClosestWebColorComponent( webRGB[i] );
		
		// now perform final conversion (or copy) to destination space...
		return ColorConvert( kRGBSpace, desiredColorSpace, webRGB, destination_data, printable );
	} 
	else
	{
		// copy or convert color
		return ColorConvert( mSpace, desiredColorSpace, mComponents, destination_data, printable );
	}
}


bool MCColor::GetColorIgnoreWeb( MCSpace desiredColorSpace, short dst[4], bool* printable )
{
	// store web color state
	bool oldState = mWebSafe;
	
	// turn off web safe
	mWebSafe = false;

	bool result = GetColor( desiredColorSpace, dst, printable );
	
	// restore state
	mWebSafe = oldState;
	
	return  result;
}


// this returns the colorspace and the data untouched!
MCSpace MCColor::GetColorNoConvert( short componentData[4] )
{
	// always copy the color data...  don't convert it!  this happens on retrieval...
	if( componentData )
		CSCopyColor( componentData, mComponents );
	return mSpace;
}


// returns a color in Photoshop's various formats...
void MCColor::GetColorForPS( MCSpace desiredColorSpace, short destination_data[4] )
{
	// get color and then convert to photoshop...
	GetColor( desiredColorSpace, destination_data );
	
	// now convert to PhotoShop
	ConvertToPS( desiredColorSpace, destination_data );	
}


MCPackedColor MCColor::GetPackedColorForPS()
{
	MCPackedColor packedColor = GetPackedColor();
	
	ConvertToPS( mSpace, packedColor.components );
	
	return packedColor;
}


void MCColor::ConvertToPS( MCSpace desiredColorSpace, short destination_data[4] )
{
	switch( desiredColorSpace )
	{
		case kRGBSpace:
		case kCMYKSpace:
			// 8 bit to 16 bit, apple/adobe style
			for( int i = 0; i < 4; i++ )
				destination_data[i] = expand_rgb( destination_data[i] );
			break;
			
		case kHSBSpace:
			// expand hue from 0 - 360 to 0 - 65535
			destination_data[0] = destination_data[0] * 65535 / 360;
			destination_data[1] = expand_rgb( destination_data[1] );
			destination_data[2] = expand_rgb( destination_data[2] );
			break;

		case kLabSpace:
			// convert Lab values...back to funky photoshop values
			destination_data[0] = limit_percent( destination_data[0] ) * 100;
			destination_data[1] = limit_lab( destination_data[1] ) * 100;
			destination_data[2] = limit_lab( destination_data[2] ) * 100;
			break;

		
		// these don't belong!!@
		case kHVCSpace:
		case kHSLSpace:
			break;
		
	}
}


// returns the color in RGB (possibly converting it)
MCColor MCColor::GetColorInRGB()
{
	// copy or convert color
	short rgb[4] = {0};
	GetColor( kRGBSpace, rgb );
	
	return MCColor( kRGBSpace, rgb );
}

MCColor MCColor::GetColorInCMYK()
{
	// copy or convert color
	short cmyk[4] = {0};
	GetColor( kCMYKSpace, cmyk );
	
	return MCColor( kCMYKSpace, cmyk );
}


MCColor MCColor::GetColorInWeb()
{
	// copy or convert color
	short web[4] = {0};
	GetColor( kWebSpace, web );
	
	return MCColor( kWebSpace, web );	// webspace just gets mapped to rgb
}


MCPackedColor MCColor::GetPackedColor()
{
	return MakePackedColor( mComponents[0], mComponents[1], mComponents[2], mComponents[3], mSpace );
}


MCPackedColor MCColor::MakePackedColor( short r, short g, short b, short h, MCSpace colorSpace )
{
	MCPackedColor newColor;
	newColor.components[0] = r;
	newColor.components[1] = g;
	newColor.components[2] = b;
	newColor.components[3] = h;
	newColor.colorSpace = (short)colorSpace;
	
	return newColor;
}


bool MCColor::Compare( MCColor& rhs, bool convertToRGB )
{
	// if convert to RGB is true we do a different comparison.
	if( convertToRGB )
	{
		MCColor usRGB = GetColorInRGB();
		MCColor rhsRGB = rhs.GetColorInRGB();
		
		return usRGB == rhsRGB;	
	}


	// test the color space first, if this doesn't match bail
	if( mSpace != rhs.mSpace )
		return false;
	
	// NOTE: it is expected than any UNUSED components are set to zero, 
	// if you don't do this- your comparisons will always fail.
	for( int i = 0; i < 4; i++ )
	{
		if( mComponents[i] != rhs.mComponents[i] )
			return false;
	}
	
	return true;
}


unsigned long MCColor::ColorDistance( MCColor& rhs )
{
	// test the color space first, if this doesn't match bail
	if( mSpace != rhs.mSpace )
		return LONG_MAX;			// return the maximum distance here...
	
	// Calculate the distance between the colors
	long result = rhs.mComponents[0];	

	result -= mComponents[0];
	result *= result;

	// Intermediate result for the 2nd plane
	long temp = rhs.mComponents[1];		

	// Add it to the total distance
	temp -= mComponents[1];
	result += temp * temp;					

	// Intermediate result for the 3rd plane
	temp = rhs.mComponents[2];				
	temp -= mComponents[2];
	
	// Add it to the total distance
	result += temp * temp;					

	// Intermediate result for the 4th plane -- note: last plane can sometimes be invalid, when this is the case the value is always zero
	temp = rhs.mComponents[3];				
	temp -= mComponents[3];
	
	// Add it to the total distance
	result += temp * temp;
	
	return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MCColor::operator ASRGBColor()
{
	return GetColorForDisplay();
}


MCColor::operator RGBTrip()
{
	short	rgb[4]  = {0};
	RGBTrip rgbTrip = {0};
	
	// get the color as an RGB and then pack it
	GetColor( kRGBSpace, rgb );
	PackRGB( rgb, rgbTrip );
	
	return rgbTrip;
}


MCColor::operator HVCTrip()
{
	short	hvc[4]  = {0};
	HVCTrip hvcTrip = {0};
	
	// get the color as HVC and then pack it
	GetColor( kHVCSpace, hvc );
	PackHVC( hvc, hvcTrip );
	
	return hvcTrip;
}


const MCColor& MCColor::operator=( const MCColor& rhs )
{	
	mSpace = rhs.mSpace;
	
	// unrolled
	mComponents[0] = rhs.mComponents[0];
	mComponents[1] = rhs.mComponents[1];
	mComponents[2] = rhs.mComponents[2];
	mComponents[3] = rhs.mComponents[3];

	// should we copy the web safe flag??? 
	// !!@ it seems the answer is no because colors can get 
	// stuck displaying only web safe versions of their color!!!
//	mWebSafe = rhs.mWebSafe; // don't enable !!@

	return *this;
}


bool MCColor::operator==( const MCColor& rhs )
{
	return Compare( const_cast<MCColor&>( rhs ), false );
}


// EOF
