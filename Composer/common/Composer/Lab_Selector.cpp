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

#include "Lab_Selector.h"
#include "Composer_Dialog_Indexes.h"


enum {	L, A, B };

// Class to check for changes to the Lab color
class Lab_Selector::Lab_Update_Check : public Color_Selector::Check_For_Update
{
	typedef Color_Selector::Check_For_Update base;

	public:
		// Constructors:
		Lab_Update_Check( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner, Lab_Selector& p_selector, const int p_z_index ) :
			base( p_Z_map, p_XY_map, p_runner, p_selector, p_selector.m_Lab, p_z_index ) 
		{}
		
		virtual	~Lab_Update_Check()
		{
			CheckForChange( static_cast<Lab_Selector&>( selector() ).m_Lab );
		}
};



// Return the Lab mode
ASInt32 Lab_Selector::GetCurrentMode() const
{
	return plugInModeLabColor;
}

// Return the RGB mode
SkipBlock Lab_Selector::GetSkipBlock() const
{
	return SKIP_LAB;
}


// Update the selected color with the new Lab color
void Lab_Selector::SetNewColor( ComposerUI& ui ) const
{
	Color_Selector::SetNewColor( ui, LAB_L_TEXT, LAB_B_TEXT, plugIncolorServicesLabSpace, m_Lab );
}

/**********************************************************************************************/

// Initiate a Lab update check for the L Z plane
Color_Selector::Update_Check L_Selector::BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner )
{
	return Update_Check( new Lab_Update_Check( p_Z_map, p_XY_map, p_runner, *this, L ) );
}

// Get the a parameter
short L_Selector::GetX()
{
	return GetColor()[A];
}

// Get the b parameter
short L_Selector::GetY()
{
	return GetColor()[B];
}

// Get the L parameter
short L_Selector::GetZ()
{
	return GetColor()[L];
}



void L_Selector::StoreX( short x )
{
	GetColor()[A] = x;
}

void L_Selector::StoreY( short y )
{
	GetColor()[B] = y;
}

void L_Selector::StoreZ( short z )
{
	GetColor()[L] = z;
}


/**********************************************************************************************/

// Initiate a Lab update check for the a Z plane
Color_Selector::Update_Check A_Selector::BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner )
{
	return Update_Check( new Lab_Update_Check( p_Z_map, p_XY_map, p_runner, *this, A ) );
}

// Get the L parameter
short A_Selector::GetX()
{
	return GetColor()[L];
}

// Get the b parameter
short A_Selector::GetY()
{
	return GetColor()[B];
}

// Get the a parameter
short A_Selector::GetZ()
{
	return GetColor()[A];
}



void A_Selector::StoreX( short x )
{
	GetColor()[L] = x;
}

void A_Selector::StoreY( short y )
{
	GetColor()[B] = y;
}

void A_Selector::StoreZ( short z )
{
	GetColor()[A] = z;
}

/**********************************************************************************************/

// Initiate a Lab update check for the b Z plane
Color_Selector::Update_Check B_Selector::BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner )
{
	return Update_Check( new Lab_Update_Check( p_Z_map, p_XY_map, p_runner, *this, B ) );
}

// Get the L parameter
short B_Selector::GetX()
{
	return GetColor()[L];
}

// Get the a parameter
short B_Selector::GetY()
{
	return GetColor()[A];
}

// Get the b parameter
short B_Selector::GetZ()
{
	return GetColor()[B];
}



void B_Selector::StoreX( short x )
{
	GetColor()[L] = x;
}

void B_Selector::StoreY( short y )
{
	GetColor()[A] = y;
}

void B_Selector::StoreZ( short z )
{
	GetColor()[B] = z;
}
// EOF
