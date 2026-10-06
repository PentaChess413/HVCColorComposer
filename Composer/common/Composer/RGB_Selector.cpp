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

#include "RGB_Selector.h"
#include "Composer_Dialog_Indexes.h"


// Class to check for changes to the RGB color
class RGB_Selector::RGB_Update_Check : public Color_Selector::Check_For_Update
{
	typedef Color_Selector::Check_For_Update base;

	public:
		// Constructors:
		RGB_Update_Check( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner, RGB_Selector& p_selector, int p_z_index ) :
		    base( p_Z_map, p_XY_map, p_runner, p_selector, p_selector.m_RGB, p_z_index )
		{}
		
		virtual	~RGB_Update_Check()
		{
			CheckForChange( static_cast<RGB_Selector&>( selector() ).m_RGB );
		}
};



// Return the RGB mode
ASInt32 RGB_Selector::GetCurrentMode() const
{
	return plugInModeRGBColor;
}

// Return the RGB mode
SkipBlock RGB_Selector::GetSkipBlock() const
{
	return SKIP_RGB;
}

// Update the selected color with the new RGB color
void RGB_Selector::SetNewColor( ComposerUI& ui ) const
{
	Color_Selector::SetNewColor( ui, RGB_RED_TEXT, RGB_BLUE_TEXT, plugIncolorServicesRGBSpace, m_RGB );
}


/**********************************************************************************************/

// Initiate an RGB update check for the Red Z plane
Color_Selector::Update_Check Red_Selector::BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner )
{
	return Update_Check( new RGB_Update_Check( p_Z_map, p_XY_map, p_runner, *this, RED ) );
}

// Get the Green parameter
short Red_Selector::GetX()
{
	return GetColor()[GREEN];
}

// Get the Blue parameter
short Red_Selector::GetY()
{
	return GetColor()[BLUE];
}

// Get the Red parameter
short Red_Selector::GetZ()
{
	return GetColor()[RED];
}



void Red_Selector::StoreX( short x )
{
	GetColor()[GREEN] = x;
}

void Red_Selector::StoreY( short y )
{
	GetColor()[BLUE] = y;
}

void Red_Selector::StoreZ( short z )
{
	GetColor()[RED] = z;
}

/**********************************************************************************************/

// Initiate an RGB update check for the Green Z plane
Color_Selector::Update_Check Green_Selector::BeginUpdateCheck(const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner )
{
	return Update_Check( new RGB_Update_Check( p_Z_map, p_XY_map, p_runner, *this, GREEN ) );
}

// Get the Red parameter
short Green_Selector::GetX()
{
	return GetColor()[RED];
}

// Get the Blue parameter
short Green_Selector::GetY()
{
	return GetColor()[BLUE];
}

// Get the Green parameter
short Green_Selector::GetZ()
{
	return GetColor()[GREEN];
}



void Green_Selector::StoreX( short x )
{
	GetColor()[RED] = x;
}

void Green_Selector::StoreY( short y )
{
	GetColor()[BLUE] = y;
}

void Green_Selector::StoreZ( short z )
{
	GetColor()[GREEN] = z;
}

/**********************************************************************************************/

// Initiate an RGB update check for the Blue Z plane
Color_Selector::Update_Check Blue_Selector::BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner )
{
	return Update_Check( new RGB_Update_Check( p_Z_map, p_XY_map, p_runner, *this, BLUE ) );
}

// Get the Red parameter
short Blue_Selector::GetX()
{
	return GetColor()[RED];
}

// Get the Green parameter
short Blue_Selector::GetY()
{
	return GetColor()[GREEN];
}

// Get the Blue parameter
short Blue_Selector::GetZ()
{
	return GetColor()[BLUE];
}


void Blue_Selector::StoreX( short x )
{
	GetColor()[RED] = x;
}

void Blue_Selector::StoreY( short y )
{
	GetColor()[GREEN] = y;
}

void Blue_Selector::StoreZ( short z )
{
	GetColor()[BLUE] = z;
}
// EOF

