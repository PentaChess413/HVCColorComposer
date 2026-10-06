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

#include "HSB_Selector.h"
#include "MCEditText.h"

#include "Composer_Dialog_Indexes.h"



uint8	Hue_2_RGB(const int32 v1, const int32 v2, int32 hue);
double	var_1(const double brightness, const double saturation);
double	var_2(const double brightness, const double saturation, const double delta_hue);
double	var_3(const double brightness, const double saturation, const double delta_hue);

//enum {	DEFAULT_BRIGHTNESS = RAW_MAX / 2 };

#define CALC_MAX	1.0
#define HALF		0.5
#define TWO			2.0
#define FOUR		4.0
#define SIX			6.0
#define ONE_THIRD	(1.0 / 3.0)

/*
// Calculate an RGB plane value from the v1, v2, and hue values
inline uint8 Hue_2_RGB(const double v1, const double v2, double hue)
{
	if (0 > hue)					// Check for hue underflow
		hue += CALC_MAX;

	if (CALC_MAX < hue)				// Check for hue overflow
		hue -= CALC_MAX;

	double	temp = SIX * hue;		// Intermediate result

	// Calculate the result
	return static_cast<uint8> ((CALC_MAX > temp ? v1 + (v2 - v1) * temp : HALF > hue ? v2 :
								FOUR > temp ? v1 + (v2 - v1) * (FOUR - temp) : v1) * RAW_MAX);
}
*/

// Calculate the simple relation
inline double var_1(const double brightness, const double saturation)
{
	return brightness * (1.0 - saturation);
}

// Calculate the simple delta hue relation
inline double var_2(const double brightness, const double saturation, const double delta_hue)
{
	return brightness * (1.0 - saturation * delta_hue);
}

// Calculate the complex relation
inline double var_3(const double brightness, const double saturation, const double delta_hue)
{
	return brightness * (1.0 - saturation * (1.0 - delta_hue));
}


// Photoshop uses the HSV colorspace but calls it HSB.
void HSB_Selector_Base::hsb_2_rgb(short rgb_color[3], const short hsb_color[3])
{
	if (0 == hsb_color[SATURATION])				// Gray
		rgb_color[0] = rgb_color[1] = rgb_color[2] = hsb_color[BRIGHTNESS];
	else
	{
		// Prepare for calculations
		double	hsb[3] = { hsb_color[HUE], hsb_color[SATURATION], hsb_color[BRIGHTNESS] };

		// Normalize to 0..1
		hsb[HUE] /= HUE_RAW_RANGE;
		hsb[SATURATION] /= RAW_MAX;
		hsb[BRIGHTNESS] /= RAW_MAX;

		// Intermediate values
		const double	hue = SIX * hsb[HUE];
		const int		hue_i = (int)hue;		// truncate ???
		const double	delta_hue = hue - hue_i;
		double			var_red, var_green, var_blue;

		// Calculate the normalized planes based on the hue sextant
		switch (hue_i)
		{
			case 0:
				var_red = hsb[BRIGHTNESS];
				var_green = var_3(hsb[BRIGHTNESS], hsb[SATURATION], delta_hue);
				var_blue = var_1(hsb[BRIGHTNESS], hsb[SATURATION]);
				break;
			case 1:
				var_red = var_2(hsb[BRIGHTNESS], hsb[SATURATION], delta_hue);
				var_green = hsb[BRIGHTNESS];
				var_blue = var_1(hsb[BRIGHTNESS], hsb[SATURATION]);
				break;
			case 2:
				var_red = var_1(hsb[BRIGHTNESS], hsb[SATURATION]);
				var_green = hsb[BRIGHTNESS];
				var_blue = var_3(hsb[BRIGHTNESS], hsb[SATURATION], delta_hue);
				break;
			case 3:
				var_red = var_1(hsb[BRIGHTNESS], hsb[SATURATION]);
				var_green = var_2(hsb[BRIGHTNESS], hsb[SATURATION], delta_hue);
				var_blue = hsb[BRIGHTNESS];
				break;
			case 4:
				var_red = var_3(hsb[BRIGHTNESS], hsb[SATURATION], delta_hue);
				var_green = var_1(hsb[BRIGHTNESS], hsb[SATURATION]);
				var_blue = hsb[BRIGHTNESS];
				break;
			default:
				var_red = hsb[BRIGHTNESS];
				var_green = var_1(hsb[BRIGHTNESS], hsb[SATURATION]);
				var_blue = var_2(hsb[BRIGHTNESS], hsb[SATURATION], delta_hue);
				break;
		}

		// Denormalize the planes
		rgb_color[0] = (short)(var_red * RAW_MAX);
		rgb_color[1] = (short)(var_green * RAW_MAX);
		rgb_color[2] = (short)(var_blue * RAW_MAX);
	} 
}



// Class to check for changes to the HSB color
class	HSB_Selector_Base::HSB_Update_Check : public Color_Selector::Check_For_Update
{
	typedef Color_Selector::Check_For_Update	base;

	public:
		// Constructors:
		HSB_Update_Check( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner, HSB_Selector_Base& p_selector, int p_z_index ) :
			base( p_Z_map, p_XY_map, p_runner, p_selector, p_selector.m_HSB, p_z_index ) {}
	
		virtual	~HSB_Update_Check()
		{
			// Always update the color before calling check changed with the HSB color
			HSB_Selector_Base&	l_selector = static_cast<HSB_Selector_Base&>( selector() );
			CheckForChange( l_selector.m_HSB );
		}
};


// Return the RGB mode
ASInt32 HSB_Selector_Base::GetCurrentMode() const
{
	return plugInModeRGBColor;
}

// Return the RGB mode
SkipBlock HSB_Selector_Base::GetSkipBlock() const
{
	return SKIP_HSB;
}


// Convert the colorspace values to both the displayable color and the text values before updating the display
void HSB_Selector_Base::SetNewColor( ComposerUI& runner ) const
{
	Color_Selector::SetNewColor( runner, HSB_HUE_TEXT, HSB_BRIGHTNESS_TEXT, plugIncolorServicesHSBSpace, m_HSB );
}


// Implement the Z buffer fill algorithm
void HSB_Selector_Base::UpdateZBuffer( const PSPixelMap& map )
{
	short		hsb_color[3], hsb_static_color[3], rgb_color[3];
	UInt8*		red = reinterpret_cast<UInt8*> (map.baseAddr);		// RGB plane addresses
	UInt8*		green = red + map.planeBytes, *blue = green + map.planeBytes;
	UInt8*		last_line = green;									// Find the end of the Red plane
	const int	half_row = map.rowBytes >> 1;

	initialize_z_buffer_color(hsb_color, hsb_static_color);
	while (last_line > red)											// Iterate over each line
	{
		UInt8*	row_end = red + map.rowBytes;

		// Fill the first half of the line
		hsb_2_rgb(rgb_color, hsb_static_color);
		for (UInt8* half_end = red + half_row; half_end != red; ++red, ++green, ++blue)
		{
			if( mWebSafe )
			{
				*red   = (UInt8)CalculateClosestWebColorComponent( rgb_color[0] );
				*green = (UInt8)CalculateClosestWebColorComponent( rgb_color[1] );
				*blue  = (UInt8)CalculateClosestWebColorComponent( rgb_color[2] );
			}
			else
				UnpackRGB(red, green, blue, rgb_color);
		}
		
		// Fill the second half of the line
		hsb_2_rgb(rgb_color, hsb_color);
		for (; row_end != red; ++red, ++green, ++blue)
		{
			if( mWebSafe )
			{
				*red   = (UInt8)CalculateClosestWebColorComponent( rgb_color[0] );
				*green = (UInt8)CalculateClosestWebColorComponent( rgb_color[1] );
				*blue  = (UInt8)CalculateClosestWebColorComponent( rgb_color[2] );
			}
			else
				UnpackRGB(red, green, blue, rgb_color);
		}
		
		increment_z_buffer_color(hsb_color, hsb_static_color);
	}
}

// Implement the XY buffer fill algorithm
void HSB_Selector_Base::UpdateXYBuffer( const PSPixelMap& map )
{
	short	hsb_color[3] = { 0 }, rgb_color[3];
	UInt8*	red = reinterpret_cast<UInt8*> (map.baseAddr);		// RGB plane addresses
	UInt8*	green = red + map.planeBytes, *blue = green + map.planeBytes;
	UInt8*	last_line = green;									// Find the end of the Red plane

	initialize_x_buffer_color(hsb_color);
	while (last_line > red)										// Iterate over each line
	{
		initialize_y_buffer_color(hsb_color);
		for (UInt8* row_end = red + map.rowBytes;				// Iterate over each pixel
			row_end != red;
			++red, ++green, ++blue, increment_y_buffer_color(hsb_color))
		{
			hsb_2_rgb(rgb_color, hsb_color);

			if( mWebSafe )
			{
				*red   = (UInt8)CalculateClosestWebColorComponent( rgb_color[0] );
				*green = (UInt8)CalculateClosestWebColorComponent( rgb_color[1] );
				*blue  = (UInt8)CalculateClosestWebColorComponent( rgb_color[2] );
			}
			else
				UnpackRGB( red, green, blue, rgb_color );
		}

		increment_x_buffer_color(hsb_color);
	}
}



/**********************************************************************************************/

// Initiate an HSB update check for the Hue Z plane
Color_Selector::Update_Check HSB_Hue_Selector::BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner )
{
	return Update_Check( new HSB_Update_Check( p_Z_map, p_XY_map, p_runner, *this, HUE ) );
}

// Get the Brightness parameter
short HSB_Hue_Selector::GetX()
{
	return GetColor()[BRIGHTNESS];
}

// Get the Saturation parameter
short HSB_Hue_Selector::GetY()
{
	return GetColor()[SATURATION];
}

// Get the Hue parameter
short HSB_Hue_Selector::GetZ()
{
	// we need the result in 0 - 255 form...
	return expand_hue( GetColor()[HUE] );
}


void HSB_Hue_Selector::StoreX( short x )
{
	GetColor()[BRIGHTNESS] = x;
}

void HSB_Hue_Selector::StoreY( short y )
{
	GetColor()[SATURATION] = y;
}

void HSB_Hue_Selector::StoreZ( short z )
{
	// the value is from 0 - 255 not from 0 - 360...
	GetColor()[HUE] = limit_hue( z );
}

/**********************************************************************************************/

// Initiate an HSB update check for the Saturation Z plane
Color_Selector::Update_Check Saturation_Selector::BeginUpdateCheck( const PSPixelMap& p_Z_map,	const PSPixelMap& p_XY_map,	ComposerUI& p_runner )
{
	return Update_Check( new HSB_Update_Check( p_Z_map, p_XY_map, p_runner, *this, SATURATION ) );
}

// Get the Brightness parameter
short Saturation_Selector::GetX()
{
	return GetColor()[BRIGHTNESS];
}

// Get the Hue parameter
short Saturation_Selector::GetY()
{
	// we need the result in 0 - 255 form...
	return expand_hue( GetColor()[HUE] );
}

// Get the Saturation parameter
short Saturation_Selector::GetZ()
{
	return GetColor()[SATURATION];
}


void Saturation_Selector::StoreX( short x )
{
	GetColor()[BRIGHTNESS] = x;
}

void Saturation_Selector::StoreY( short y )
{
	// the value is from 0 - 255 not from 0 - 360...
	GetColor()[HUE] = limit_hue( y );
}

void Saturation_Selector::StoreZ( short z )
{
	GetColor()[SATURATION] = z;
}

/**********************************************************************************************/

// Initiate an HSB update check for the Brightness Z plane
Color_Selector::Update_Check Brightness_Selector::BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner )
{
	return Update_Check( new HSB_Update_Check( p_Z_map, p_XY_map, p_runner, *this, BRIGHTNESS ) );
}

// Get the Saturation parameter
short Brightness_Selector::GetX()
{
	return GetColor()[SATURATION];
}

// Get the Hue parameter
short Brightness_Selector::GetY()
{
	// we need the result in 0 - 255 form...
	return expand_hue( GetColor()[HUE] );
}

// Get the Brightness parameter
short Brightness_Selector::GetZ()
{
	return GetColor()[BRIGHTNESS];
}



void Brightness_Selector::StoreX( short x )
{
	GetColor()[SATURATION] = x;
}

void Brightness_Selector::StoreY( short y )
{
	// the value is from 0 - 255 not from 0 - 360...
	GetColor()[HUE] = limit_hue( y );
}

void Brightness_Selector::StoreZ( short z )
{
	GetColor()[BRIGHTNESS] = z;
}


// Initialize the color information for filling the Z buffer
void HSB_Hue_Selector::initialize_z_buffer_color( short hsb_color[3], short hsb_static_color[3] ) const
{
	hsb_static_color[HUE] = hsb_color[HUE] = RAW_MAX;	// Initial hue value
	hsb_color[SATURATION] = GetColor()[SATURATION];		// Real saturation value
	hsb_color[BRIGHTNESS] = GetColor()[BRIGHTNESS];		// Real brightness value
	hsb_static_color[SATURATION] = RAW_MAX;				// Base saturation value
	hsb_static_color[BRIGHTNESS] = RAW_MAX;				// Base brightness value
}

void HSB_Hue_Selector::initialize_x_buffer_color(short hsb_color[3]) const
{
	hsb_color[HUE] = expand_hue( GetColor()[HUE] );				// Real hue value
	hsb_color[BRIGHTNESS] = RAW_MAX;							// Initial brightness value
}

void HSB_Hue_Selector::initialize_y_buffer_color(short hsb_color[3]) const
{
	hsb_color[SATURATION] = 0;									// Initial saturation value
}

void HSB_Hue_Selector::increment_x_buffer_color(short hsb_color[3]) const
{
	--hsb_color[BRIGHTNESS];									// Adjust the brightness value
}

void HSB_Hue_Selector::increment_y_buffer_color(short hsb_color[3]) const
{
	++hsb_color[SATURATION];									// Adjust the saturation value
}

void HSB_Hue_Selector::increment_z_buffer_color(short hsb_color[3], short hsb_static_color[3]) const
{
	hsb_static_color[HUE] = --hsb_color[HUE];					// Adjust the hue values
}

// Initialize the color information for filling the Z buffer
void Saturation_Selector::initialize_z_buffer_color(short hsb_color[3], short hsb_static_color[3]) const
{
	hsb_static_color[HUE] = hsb_color[HUE] = expand_hue( GetColor()[HUE] );	// Real hue value
	hsb_static_color[SATURATION] = hsb_color[SATURATION] = RAW_MAX;			// Base saturation value
	hsb_color[BRIGHTNESS] = GetColor()[BRIGHTNESS];							// Real brightness value
	hsb_static_color[BRIGHTNESS] = RAW_MAX;									// Base brightness value
	m_half = false;
}

void Saturation_Selector::initialize_x_buffer_color(short hsb_color[3]) const
{
	hsb_color[SATURATION] = GetColor()[SATURATION];			// Real saturation value
	hsb_color[BRIGHTNESS] = RAW_MAX;								// Initial brightness value
}

void Saturation_Selector::initialize_y_buffer_color(short hsb_color[3]) const
{
	hsb_color[HUE] = 0;												// Initial hue value
}

void Saturation_Selector::increment_x_buffer_color(short hsb_color[3]) const
{
	--hsb_color[BRIGHTNESS];										// Adjust the brightness value
}

void Saturation_Selector::increment_y_buffer_color(short hsb_color[3]) const
{
	++hsb_color[HUE];												// Adjust the hue value
}

void Saturation_Selector::increment_z_buffer_color(short hsb_color[3],	short hsb_static_color[3]) const
{
	hsb_static_color[SATURATION] = --hsb_color[SATURATION];			// Adjust the saturation values
	if (m_half)
		--hsb_static_color[BRIGHTNESS];								// Adjust the static color

	m_half = !m_half;
}

// Initialize the color information for filling the Z buffer
void Brightness_Selector::initialize_z_buffer_color(short hsb_color[3], short hsb_static_color[3]) const
{
	hsb_static_color[HUE] = hsb_color[HUE] = expand_hue( GetColor()[HUE] );		// Real hue value
	hsb_color[SATURATION] = GetColor()[SATURATION];								// Real saturation value
	hsb_static_color[BRIGHTNESS] = hsb_color[BRIGHTNESS] = RAW_MAX;				// Base brightness value
	hsb_static_color[SATURATION] = RAW_MAX;										// Base saturation value
}

void Brightness_Selector::initialize_x_buffer_color(short hsb_color[3]) const
{
	hsb_color[BRIGHTNESS] = GetColor()[BRIGHTNESS];					// Real brightness value
	hsb_color[SATURATION] = RAW_MAX;								// Initial saturation value
}

void Brightness_Selector::initialize_y_buffer_color(short hsb_color[3]) const
{
	hsb_color[HUE] = 0;												// Initial hue value
}

void Brightness_Selector::increment_x_buffer_color(short hsb_color[3]) const
{
	--hsb_color[SATURATION];										// Adjust the saturation value
}

void Brightness_Selector::increment_y_buffer_color(short hsb_color[3]) const
{
	++hsb_color[HUE];												// Adjust the hue value
}

void Brightness_Selector::increment_z_buffer_color(short hsb_color[3],	short hsb_static_color[3]) const
{
	hsb_static_color[BRIGHTNESS] = --hsb_color[BRIGHTNESS];			// Adjust the brightness values
}


// EOF
