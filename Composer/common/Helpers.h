/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Helpers_h
#define Helpers_h

#include "MCDefs.h"
#include "PITypes.h"


// Common values
enum 
{	
	PERCENT			 = 100, 
	LAB_ADJUST		 = 128, 
	RAW_MAX			 = 255, 
	RAW_RANGE		 = 256,
	HUE_RAW_RANGE	 = 256,
	MAX_HUE_RANGE	 = 360,
	MAX_HUE			 = MAX_HUE_RANGE,
	RGB_SHIFT		 = 8, 
	HUE_SELECT_WIDTH = 16, 
	IMAGE_INSET		 = 3 
};


typedef enum 
{	
	SKIP_NONE = 0,
	SKIP_HSB, 
	SKIP_RGB, 
	SKIP_LAB, 
	SKIP_HVC, 
	SKIP_CMYK,
	SKIP_RAW
} SkipBlock;





// Limit the plane to [0 - 255]
inline short limit_raw( short component )
{
	return component < 0 ? 0 : (component > RAW_MAX ? RAW_MAX : component);
}

// Convert an RGB plane from 16 bits to 8 bits
inline short limit_rgb( short component )
{
	return (unsigned short)component >> RGB_SHIFT;
}

// Convert an RGB plane from 8 bits to 16 bits
inline short expand_rgb( short value )
{
	// truncate to 8 bits
	unsigned char limited = (unsigned char)limit_raw( value ) & 0xFF;
	
	// force "Mac'ish" doubled component value (AaBa) where Aa and Bb are the same number
	return (limited << RGB_SHIFT) | limited;
}

// Convert from [0 - 255] to [0 - 100]
inline short limit_percent( short component )
{
	// round the result, then truncate
	float result = (float)component * PERCENT / (float)RAW_MAX;
	return (short)(result + 0.5f);
}

// Convert from [0 - 100] to [0 - 255]
inline short expand_percent( short value )
{
	// round the result, then truncate
	float result = (float)value * RAW_MAX / (float)PERCENT;
	return limit_raw( (short)(result + 0.5f) );
}

// Convert from [0 - 255] to [-128 - 127]
inline short limit_lab( short component )
{
	return component - LAB_ADJUST;
}

// Convert from [-128 - 127] to [0 - 255]
inline short expand_lab( short value )
{
	return limit_raw( value + LAB_ADJUST );
}


// Convert from [0 - 255] to [0 - 360]
inline short limit_hue( short component )
{
	float result = (float)component * MAX_HUE / (float)HUE_RAW_RANGE;
	return (short)(result + 0.5f);	// round and truncate
}



// Convert from [0 - 360] to [0 - 255]
inline short expand_hue( short value )
{
	float result = (float)value * HUE_RAW_RANGE / (float)MAX_HUE;
	
	// round then trunate
	short raw = (short)(result + 0.5f);

	while( HUE_RAW_RANGE <= raw )
		raw -= HUE_RAW_RANGE;

	while( 0 > raw )
		raw += HUE_RAW_RANGE;

	return raw;
}



// Copy the color data in a short[4] array
inline void copy_color(short* destination, const short* source)
{
	// !!@ this loop only copies 3 out of four elements...  !!@ bug?
	for (int i = 0; i < 3; ++i)
		destination[i] = source[i];
}

// Convert the RGBColor to a short[4] array and copy the data
inline void copy_color(RGBColor& destination, const short* source)
{
	copy_color(reinterpret_cast<short*> (&destination), source);
}

// Convert the RGBColor to a short[4] array and copy the data
inline void copy_color(short* destination, const RGBColor& source)
{
	copy_color(destination, reinterpret_cast<const short*> (&source));
}

// Convert the RGBColors to short[4] arrays and copy the data
inline void copy_color(RGBColor& destination, const RGBColor& source)
{
	copy_color(reinterpret_cast<short*> (&destination), reinterpret_cast<const short*> (&source));
}



// Convert from the limited RGBTrip to the full RGBColor formats
inline void expand_color( ASRGBColor& destination, const RGBTrip& source )
{
	destination.red = expand_rgb(source.red);
	destination.green = expand_rgb(source.green);
	destination.blue = expand_rgb(source.blue);
}


// Extract the rgb_color information into separate RGB values -- this one is backwards !!@
inline void UnpackRGB( ASUInt8* red, ASUInt8* green, ASUInt8* blue, const RGBTrip& rgb_color )
{
	*red   = rgb_color.red;
	*green = rgb_color.green;
	*blue  = rgb_color.blue;
}


// Extract the rgb_color information into separate RGB values -- backwards !!@
inline void UnpackRGB( ASUInt8* red, ASUInt8* green, ASUInt8* blue, const short* rgb_color )
{
	// !!@ data type too large to fit destination -- so we mask it...
	*red   = (ASUInt8)(rgb_color[0] & 0xFF);
	*green = (ASUInt8)(rgb_color[1] & 0xFF);
	*blue  = (ASUInt8)(rgb_color[2] & 0xFF);
}


inline void UnpackRGB( RGBTrip rgbTrip, short rgb[4] )
{
	rgb[0] = rgbTrip.red;
	rgb[1] = rgbTrip.green;
	rgb[2] = rgbTrip.blue;
}



// pack shorts into an RGBTriplet
inline void PackRGB( const short rgb[4], RGBTrip& rgbTrip )
{
	rgbTrip.red   = (unsigned char)rgb[0];
	rgbTrip.green = (unsigned char)rgb[1];
	rgbTrip.blue  = (unsigned char)rgb[2];
}


// pack shorts into an HVCTrip
inline void PackHVC( const short hvc[4], HVCTrip& hvcTrip )
{
	hvcTrip.hue    = (unsigned char)hvc[0];
	hvcTrip.value  = (unsigned char)hvc[1];
	hvcTrip.chroma = (unsigned char)hvc[2];
}

inline void UnpackHVC( HVCTrip hvcTrip, short hvc[4] )
{
	hvc[0] = hvcTrip.hue;
	hvc[1] = hvcTrip.value;
	hvc[2] = hvcTrip.chroma;
}


// Calculate the closest web color
inline short CalculateClosestWebColorComponent( short component )
{
	const short kWebSpace = 0x33;
	const short kWebSpaceOver2 = kWebSpace / 2;
	
	// !!@ I don't know exactly what's going on here and don't really care, this "seems" to work.
	short offset = component % kWebSpace;
	short result = component - offset;			// Find the smallest color

	// Is the next color a better match?
	if( kWebSpaceOver2 < offset )					
		result += kWebSpace;

	return result;
}



#endif

// EOF
