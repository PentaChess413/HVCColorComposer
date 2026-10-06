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
#include "HVC_Selector.h"
#include "MCLib_Wrapper.h"
#include "Composer_Dialog_Indexes.h"

enum 
{	
	HUE, 
	VALUE, 
	CHROMA 
};

// ???
enum 
{	
	DEFAULT_VALUE = 69 * RAW_MAX / PERCENT, 
	DEFAULT_CHROMA = 30 * RAW_MAX / PERCENT 
};



// Class to check for changes to the HVC color
class HVC_Selector::HVC_Update_Check : public Color_Selector::Check_For_Update
{
	typedef Color_Selector::Check_For_Update base;

	public:
		// Constructors:
		HVC_Update_Check( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner, HVC_Selector& p_selector, int p_z_index ) :
			base( p_Z_map, p_XY_map, p_runner, p_selector, p_selector.m_hvc, p_z_index ) {}

		virtual	~HVC_Update_Check()
		{
			// Always update the color before calling check changed with the HVC color
			HVC_Selector&	l_selector = static_cast<HVC_Selector&>( selector() );

//			l_selector.update_color();
			CheckForChange( l_selector.m_hvc );
	
		}
};



// Return the RGB mode -- we do this because we can only convert HVC to RGB
ASInt32 HVC_Selector::GetCurrentMode() const
{
	return plugInModeRGBColor;
}

// Return the RGB mode
SkipBlock HVC_Selector::GetSkipBlock() const
{
	return SKIP_HVC;
}


// Convert the colorspace values to both the displayable color and the text values before updating the display
void HVC_Selector::SetNewColor( ComposerUI& runner ) const
{
//	runner.hvc_convert();
	Color_Selector::SetNewColor( runner, HVC_HUE_TEXT, HVC_MUNSELL_CHROMA_TEXT, kHVCSpace, m_hvc );
}


// Implement the Z buffer fill algorithm
void HVC_Selector::UpdateZBuffer( const PSPixelMap& map )
{
	const RGBTrip	back = get_background_color();
	
	ASUInt8*		red = reinterpret_cast<ASUInt8*> (map.baseAddr);		// RGB plane addresses
	ASUInt8*		green = red + map.planeBytes, *blue = green + map.planeBytes;
	ASUInt8*		last_line = red + map.planeBytes;					// Find the end of the Red plane
	
	HVCTrip			hvc_color, hvc_static_color;
	RGBTrip			rgb_color = { 0 }, rgb_static_color = { 0 };
	
	const int		half_row = map.rowBytes >> 1;

	initialize_z_buffer_color( hvc_color, hvc_static_color );
	
	// Iterate over each line
	while (last_line > red)												
	{
		const RGBTrip&	the_color = mMCLib.hvc_2_rgb( hvc_color, rgb_color ) ? rgb_color : back;
		
		ASUInt8* row_end = red + map.rowBytes;

		// Fill the first half of the line
		mMCLib.hvc_2_rgb( hvc_static_color, rgb_static_color );
		
		for (ASUInt8* half_end = red + half_row; half_end != red; ++red, ++green, ++blue)
		{
			if( mWebSafe )
			{
				*red   = (ASUInt8)CalculateClosestWebColorComponent( rgb_static_color.red );
				*green = (ASUInt8)CalculateClosestWebColorComponent( rgb_static_color.green );
				*blue  = (ASUInt8)CalculateClosestWebColorComponent( rgb_static_color.blue );
			}
			else
				UnpackRGB( red, green, blue, rgb_static_color );
		}
		
		// Fill the second half of the line
		for (; row_end != red; ++red, ++green, ++blue)
		{
			if( mWebSafe )
			{
				*red   = (ASUInt8)CalculateClosestWebColorComponent( the_color.red );
				*green = (ASUInt8)CalculateClosestWebColorComponent( the_color.green );
				*blue  = (ASUInt8)CalculateClosestWebColorComponent( the_color.blue );
			}
			else
				UnpackRGB( red, green, blue, the_color );
		}
		
		increment_z_buffer_color( hvc_color, hvc_static_color );
	}
}

// Implement the XY buffer fill algorithm
void HVC_Selector::UpdateXYBuffer( const PSPixelMap& map )
{
	RGBTrip	back_color = { 128, 128, 128 };
	
	ASUInt8*	red = reinterpret_cast<ASUInt8*> (map.baseAddr);		// RGB plane addresses
	ASUInt8*	green = red + map.planeBytes, *blue = green + map.planeBytes;
	ASUInt8*	last_line = red + map.planeBytes;					// Find the end of the Red plane
	
	HVCTrip	hvc_color;
	RGBTrip	rgb_color = { 0 };

	initialize_x_buffer_color( hvc_color );
	
	while (last_line > red)										// Iterate over each line
	{
		initialize_y_buffer_color(hvc_color);
		for (ASUInt8* row_end = red + map.rowBytes;				// Iterate over each pixel
			row_end != red;
			++red, ++green, ++blue, increment_y_buffer_color( hvc_color ) )
		{
			RGBTrip temp = mMCLib.hvc_2_rgb( hvc_color, rgb_color ) ? rgb_color : back_color;
			
			if( mWebSafe )
			{
				*red   = (ASUInt8)CalculateClosestWebColorComponent( temp.red );
				*green = (ASUInt8)CalculateClosestWebColorComponent( temp.green );
				*blue  = (ASUInt8)CalculateClosestWebColorComponent( temp.blue );
			}
			else
				UnpackRGB( red, green, blue, temp );
		}

		increment_x_buffer_color( hvc_color );
	}
}

/**********************************************************************************************/

// Initiate an HVC update check for the Hue Z plane
Color_Selector::Update_Check HVC_Hue_Selector::BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner )
{
	return Update_Check (new HVC_Update_Check(p_Z_map, p_XY_map, p_runner, *this, HUE));
}

// Get the Value parameter
short HVC_Hue_Selector::GetX()
{
	return GetColor()[VALUE];
}

// Get the Chroma parameter
short HVC_Hue_Selector::GetY()
{
	return GetColor()[CHROMA];
}

// Get the Hue parameter
short HVC_Hue_Selector::GetZ()
{
	return GetColor()[HUE];
}


void HVC_Hue_Selector::StoreX( short x )
{
	GetColor()[VALUE] = x;
}

void HVC_Hue_Selector::StoreY( short y )
{
	GetColor()[CHROMA] = y;
}

void HVC_Hue_Selector::StoreZ( short z )
{
	GetColor()[HUE] = z;
}


// Initialize the Z buffer color
void HVC_Hue_Selector::initialize_z_buffer_color( HVCTrip& hvc_color, HVCTrip& hvc_static_color ) const
{
	hvc_static_color.hue = hvc_color.hue = RAW_MAX;	// Initial Hue value  -- !!@ look here for error...
	hvc_color.value  = (unsigned char)GetColor()[VALUE];			// Real Value value
	hvc_color.chroma = (unsigned char)GetColor()[CHROMA];			// Real Chroma value

	hvc_static_color.value = DEFAULT_VALUE;			// Initial Value value
	hvc_static_color.chroma = DEFAULT_CHROMA;		// Initial Chroma value
}

// Initialize the x plane of the XY buffer color
void HVC_Hue_Selector::initialize_x_buffer_color(HVCTrip& hvc_color) const
{
	hvc_color.hue = (unsigned char)GetColor()[HUE];			// Real Hue value
	hvc_color.value = RAW_MAX;						// Initial Value value
}

// Initialize the y plane of the XY buffer color
void HVC_Hue_Selector::initialize_y_buffer_color(HVCTrip& hvc_color) const
{
	hvc_color.chroma = 0;							// Initial Chroma value
}

// Adjust the Value
void HVC_Hue_Selector::increment_x_buffer_color(HVCTrip& hvc_color) const
{
	--hvc_color.value;
}

// Adjust the Chroma
void HVC_Hue_Selector::increment_y_buffer_color(HVCTrip& hvc_color) const
{
	++hvc_color.chroma;
}

// Adjust the Hue values
void HVC_Hue_Selector::increment_z_buffer_color(HVCTrip& hvc_color, HVCTrip& hvc_static_color) const
{
	hvc_static_color.hue = --hvc_color.hue;
}

// Return the Z buffer background color
RGBTrip HVC_Hue_Selector::get_background_color() const
{
	RGBTrip	grey = { 128, 128, 128 };

	return grey;
}

/**********************************************************************************************/

// Initiate an HVC update check for the Value Z plane
Color_Selector::Update_Check Value_Selector::BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner )
{
	return Update_Check( new HVC_Update_Check( p_Z_map, p_XY_map, p_runner, *this, VALUE ) );
}

// Get the Hue parameter
short Value_Selector::GetX()
{
	return GetColor()[HUE];
}

// Get the Chroma parameter
short Value_Selector::GetY()
{
	return GetColor()[CHROMA];
}

// Get the Value parameter
short Value_Selector::GetZ()
{
	return GetColor()[VALUE];
}


void Value_Selector::StoreX( short x )
{
	GetColor()[HUE] = x;
}

void Value_Selector::StoreY( short y )
{
	GetColor()[CHROMA] = y;
}

void Value_Selector::StoreZ( short z )
{
	GetColor()[VALUE] = z;
}



// Initialize the Z buffer color
void Value_Selector::initialize_z_buffer_color( HVCTrip& hvc_color, HVCTrip& hvc_static_color ) const
{
	hvc_static_color.hue = hvc_color.hue = (unsigned char)GetColor()[HUE];	// Real Hue value
	hvc_static_color.value = hvc_color.value = RAW_MAX;				// Initial Value value
	hvc_color.chroma = (unsigned char)GetColor()[CHROMA];						// Real Chroma value
	hvc_static_color.chroma = 0;									// Initial Chroma value
}

// Initialize the x plane of the XY buffer color
void Value_Selector::initialize_x_buffer_color(HVCTrip& hvc_color) const
{
	hvc_color.value = (unsigned char)GetColor()[VALUE];						// Real Value value
	hvc_color.hue = RAW_MAX;										// Initial Hue value
}

// Initialize the y plane of the XY buffer color
void Value_Selector::initialize_y_buffer_color(HVCTrip& hvc_color) const
{
	hvc_color.chroma = 0;											// Initial Chroma value
}

// Adjust the Hue
void Value_Selector::increment_x_buffer_color(HVCTrip& hvc_color) const
{
	--hvc_color.hue;
}

// Adjust the Chroma
void Value_Selector::increment_y_buffer_color(HVCTrip& hvc_color) const
{
	++hvc_color.chroma;
}

// Adjust the Value values
void Value_Selector::increment_z_buffer_color(HVCTrip& hvc_color, HVCTrip& hvc_static_color) const
{
	hvc_static_color.value = --hvc_color.value;
}

// Return the Z buffer background color
RGBTrip Value_Selector::get_background_color() const
{
	RGBTrip	black = { 0 };

	return black;
}

/**********************************************************************************************/

// Initiate an HVC update check for the Chroma Z plane
Color_Selector::Update_Check Chroma_Selector::BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner )
{
	return Update_Check( new HVC_Update_Check( p_Z_map, p_XY_map, p_runner, *this, CHROMA ) );
}

// Get the Hue parameter
short Chroma_Selector::GetX()
{
	return GetColor()[HUE];
}

// Get the Value parameter
short Chroma_Selector::GetY()
{
	return GetColor()[VALUE];
}

// Get the Chroma parameter
short Chroma_Selector::GetZ()
{
	return GetColor()[CHROMA];
}



void Chroma_Selector::StoreX( short x )
{
	GetColor()[HUE] = x;
}

void Chroma_Selector::StoreY( short y )
{
	GetColor()[VALUE] = y;
}

void Chroma_Selector::StoreZ( short z )
{
	GetColor()[CHROMA] = z;
}


// Initialize the Z buffer color
void Chroma_Selector::initialize_z_buffer_color( HVCTrip& hvc_color, HVCTrip& hvc_static_color ) const
{
	hvc_color.hue    = (unsigned char)GetColor()[HUE];		// Real Hue value
	hvc_color.value  = (unsigned char)GetColor()[VALUE];	// Real Value value
	hvc_color.chroma = 255;									// Initial Chroma value  !!@ -- what up with this???
	hvc_static_color = hvc_color;							// Initialize the static color
}

// Initialize the x plane of the XY buffer color
void Chroma_Selector::initialize_x_buffer_color( HVCTrip& hvc_color ) const
{
	hvc_color.chroma = (unsigned char)GetColor()[CHROMA];	// Real Chroma value
	hvc_color.hue = RAW_MAX;					// Initial Hue value
}

// Initialize the y plane of the XY buffer color
void Chroma_Selector::initialize_y_buffer_color( HVCTrip& hvc_color ) const
{
	hvc_color.value = 0;						// Initial Value value
}

// Adjust the Hue
void Chroma_Selector::increment_x_buffer_color( HVCTrip& hvc_color ) const
{
	--hvc_color.hue;
}

// Adjust the Value
void Chroma_Selector::increment_y_buffer_color( HVCTrip& hvc_color ) const
{
	++hvc_color.value;
}

// Adjust the Chroma values
void Chroma_Selector::increment_z_buffer_color( HVCTrip& hvc_color, HVCTrip& hvc_static_color ) const
{
	hvc_static_color.chroma = --hvc_color.chroma;
}

// Return the Z buffer background color
RGBTrip Chroma_Selector::get_background_color() const
{
	RGBTrip	black = { 0 };

	return black;
}

// EOF
