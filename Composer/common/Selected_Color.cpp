/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "PhotoShopSDK.h"

#include "Selected_Color.h"
#include "Helpers.h"

#include "PIUtilities.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Initialize the data using the original color
SelectedColor::SelectedColor( PIPickerParams& p_picker_record ) :
	mHostRecord( p_picker_record )
{
	ResetColor();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Get the original color data
void SelectedColor::ResetColor()
{
	// modify the base class directly...
	mSpace = (MCSpace)mHostRecord.pickParms.sourceSpace;

	GetOriginalColor( mSpace, mComponents );
}

/*
// Save the new color data in the class color data, converting to the existing color space
void SelectedColor::convert_color(const short source_space, const short source_data[4])
{
	convert_color(source_space, mSpace, source_data, mComponents);
}

// Convert the class color data to the new color space
void SelectedColor::convert_to(const short destination_space)
{
	if( destination_space == mSpace )
		return;

	convert_color(mHostRecord, destination_space, mComponents, mComponents);
	mSpace = destination_space;
}
*/


MCColor	SelectedColor::GetOriginalColor()
{
	short components[4] = {0};
	MCSpace space = (MCSpace)mHostRecord.pickParms.sourceSpace; // !!@ check to make sure this is the right color space !!@
	
	GetOriginalColor( space, components );
	
	return MCColor( space, components );
}


void SelectedColor::GetOriginalColor( short desiredColorSpace, short destData[4] )
{
	MCSpace space = (MCSpace)mHostRecord.pickParms.sourceSpace;

	// first grab the data from photoshop and convert it to our format...
	CSCopyColor( destData, (const short*)mHostRecord.pickParms.colorComponents );

	// we need to convert the photoshop color into our color format
	switch( space )
	{
		case kRGBSpace:
		case kCMYKSpace:
			// 16 bit to 8 bit
			for( int i = 0; i < 4; i++ )
				destData[i] = limit_rgb( destData[i] );
			break;
			
		case kHSBSpace:
			// shrink hue from 0 - 65535 to 0 - 360
			destData[0] = (short)((((unsigned short)destData[0]) * 360.0f / 65535.0f) + 0.5f);
			destData[1] = limit_rgb( destData[1] );
			destData[2] = limit_rgb( destData[2] );
			break;

		case kLabSpace:
			// expand funky Lab values...
			destData[0] = expand_percent( destData[0] / 100 );
			destData[1] = expand_lab( destData[1] / 100 );
			destData[2] = expand_lab( destData[2] / 100 );
			break;
		
		// these don't belong!!@
		case kHVCSpace:
		case kHSLSpace:
			XDEBUG_PRINT( "bad color space request!!\n" );
			break;
		
	}
	
	// now convert to color if necessary...
	ColorConvert( space, (MCSpace)desiredColorSpace, destData, destData );
}


ASRGBColor SelectedColor::GetOriginalColorForDisplay()
{
	// form an RGB for display
	short temp[4];
	ASRGBColor rgb;
	
	// grab color in RGB
	GetOriginalColor( plugIncolorServicesRGBSpace, temp );
	rgb.red   = expand_rgb( temp[0] );
	rgb.green = expand_rgb( temp[1] );
	rgb.blue  = expand_rgb( temp[2] );
	
	return rgb;
}


// EOF
