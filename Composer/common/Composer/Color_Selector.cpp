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

#include "Color_Selector.h"
#include "Composer_UI.h"

#include "ADM_Access.h"
#include "Color_Flag_Handler.h"
#include "Composer_Dialog_Indexes.h"

#include "admItem.h"


// Initialize the parameters
Color_Selector::Check_For_Update::Check_For_Update( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner, Color_Selector& p_selector, const short p_color[3], const int p_z_index ) :
	m_Z_map( p_Z_map ), 
	m_XY_map( p_XY_map ),
	m_runner( p_runner ),
	m_selector( p_selector ),
	m_z_index( p_z_index )
{
	// Save the current color for comparison
	copy_color( m_color, p_color );	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Fill a color plane in a pixel map with a single color
void Color_Selector::fill_plane( const ASUInt8 color, ASUInt8* line, const ASUInt8* last_line, const int32 row_bytes )
{
	for (; last_line > line; line += row_bytes)							// Fill each line
		for (ASUInt8* pixel = line; line + row_bytes > pixel; ++pixel)	// Fill a line
			*pixel = mWebSafe ? (ASUInt8)CalculateClosestWebColorComponent( color ) : color;												// Set the pixel
}

// Partially fill half a color plane in a pixel map with one color, then finish the plane with another
void Color_Selector::fill_plane( const ASUInt8 real_color, const ASUInt8 default_color, ASUInt8* line, const ASUInt8* last_line, const int32 row_bytes)
{
	const	int32	half_row = row_bytes >> 1;

	for (; last_line > line; line += row_bytes)		// Fill each line
	{
		ASUInt8*	pixel = line;

		while (line + half_row > pixel)				// First half of the line
			*pixel++ = default_color;				// Set the pixel

		while (line + row_bytes > pixel)			// Second half of the line
			*pixel++ = real_color;					// Set the pixel
	}
}

// Fill a color plane in a pixel map with a decrementing value from [255-0] for each line
void Color_Selector::decrement_plane( ASUInt8* line, const ASUInt8* last_line, const int32 row_bytes )
{
	for (ASUInt8 color = RAW_MAX; last_line > line; line += row_bytes, --color)	// Fill each line, calculating the color for the line
		for (ASUInt8* pixel = line; line + row_bytes > pixel; ++pixel)					// Fill a line
			*pixel = mWebSafe ? (ASUInt8)CalculateClosestWebColorComponent( color ) : color;																// Set the pixel
}

// Fill a color plane in a pixel map with a decrementing value from [255-0] in each line
void Color_Selector::fill_row( ASUInt8* line, const ASUInt8* last_line, const int32 row_bytes )
{
	for (; last_line > line; line += row_bytes)												// Fill each line
		for (ASUInt8 color = 0, *pixel = line; line + row_bytes > pixel; ++pixel, ++color)	// Fill a line, calculating the color for each pixel
			*pixel = mWebSafe ? (ASUInt8)CalculateClosestWebColorComponent( color ) : color;																// Set the pixel
}




// Compare the new value to the old value, updating the old value and the runner if different
bool Color_Selector::SetX( short new_value, ComposerUI& runner )
{
	if( new_value == GetX() )	// No change
		return false;

	StoreX( new_value );		// Save the value
	SetNewColor( runner );		// Update the runner

	return true;
}

// Compare the new value to the old value, updating the old value and the runner if different
bool Color_Selector::SetY( short new_value, ComposerUI& runner )
{
	if( new_value == GetY() )	// No change
		return false;

	StoreY( new_value );		// Save the value
	SetNewColor( runner );		// Update the runner

	return true;
}

// Compare the new value to the old value, updating the old value and the runner if different
bool Color_Selector::SetZ( short new_value, ComposerUI& runner )
{
	if( new_value == GetZ() )	// No change
		return false;

	StoreZ( new_value );		// Save the value
	SetNewColor( runner );		// Update the runner

	return true;
}


void Color_Selector::SetNewColor( ComposerUI& ui, const ASInt32 start, const ASInt32 end, short source_space, const short* const source_data )
{
	// select the new color -- this seems round about because it calls the UI which calls the color selector.  
	MCColor newColor( MCSpace( source_space ), source_data );
	
	ui.SelectNewColor( newColor );

	// Update the text displays
	ui.UpdateControls( start, end );								
}



// Implement the simple z buffer fill algorithm
void Color_Selector::UpdateZBuffer( const PSPixelMap& map )
{
	ASUInt8*	line = reinterpret_cast<ASUInt8*>( map.baseAddr );
	ASUInt8*	last_line = line + map.planeBytes;

	fill_z_buffer_0( line, last_line, map.rowBytes );		// Fill the first z buffer plane

	line = last_line;
	last_line += map.planeBytes;
	fill_z_buffer_1( line, last_line, map.rowBytes );		// Fill the second z buffer plane

	line = last_line;
	last_line += map.planeBytes;
	fill_z_buffer_2( line, last_line, map.rowBytes );		// Fill the third z buffer plane
}


// Implement the simple xy buffer fill algorithm
void Color_Selector::UpdateXYBuffer( const PSPixelMap& map )
{
	ASUInt8*	line = reinterpret_cast<ASUInt8*>( map.baseAddr );
	ASUInt8*	last_line = line + map.planeBytes;

	fill_xy_buffer_0( line, last_line, map.rowBytes );		// Fill the first xy buffer plane

	line = last_line;
	last_line += map.planeBytes;
	fill_xy_buffer_1( line, last_line, map.rowBytes );		// Fill the second xy buffer plane

	line = last_line;
	last_line += map.planeBytes;
	fill_xy_buffer_2( line, last_line, map.rowBytes );		// Fill the third xy buffer plane
}








// Update the color flag and the color slider if the color changed
void Color_Selector::Check_For_Update::CheckForChange( const short new_color[3] )
{
	bool	changed_0 = m_color[0] != new_color[0];					// Did the plane change?
	bool	changed_1 = m_color[1] != new_color[1];
	bool	changed_2 = m_color[2] != new_color[2];
	
	bool	all_changed = changed_0 && changed_1 && changed_2;
	
	bool	two_changed = !all_changed && (changed_0 ? changed_1 || changed_2 :	changed_1 && changed_2);
	
	bool	z_changed = all_changed || two_changed, xy_changed = z_changed;
	
	bool	one_changed = !z_changed && (changed_0 || changed_1 || changed_2);


	// Did the z plane change?
	if( !z_changed )
	{													
		switch( m_z_index )
		{
			case 0:
				(changed_0 ? xy_changed : z_changed) = true;
				break;

			case 1:
				(changed_1 ? xy_changed : z_changed) = true;
				break;

			case 2:
				(changed_2 ? xy_changed : z_changed) = true;
				break;
		}
	}
	
	// Update the z plane
	if( z_changed )													
		m_selector.UpdateZBuffer( m_Z_map );

	// Update the xy planes
	if( xy_changed )													
		m_selector.UpdateXYBuffer( m_XY_map );
}



Color_Selector::Check_For_Update::~Check_For_Update()
{
	Control_Runner*		hue_slider = m_runner.get_item( HUE_SLIDER );
	Color_Flag_Handler*	color_flag = (Color_Flag_Handler*)m_runner.get_item( COLOR_FLAG );
	
	if( !hue_slider || !color_flag )
		return;
		
	// Update the color slider
	hue_slider->set_int_value( RAW_MAX - m_selector.GetZ() );		
	
	// Update the color flag
	color_flag->SetPosition( (short)(RAW_MAX - m_selector.GetX()), (short)m_selector.GetY() );	
	
	// make sure they update
	hue_slider->invalidate();
	color_flag->invalidate();
}






// EOF
