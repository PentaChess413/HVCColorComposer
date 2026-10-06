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

#ifndef Color_Selector_h
#define Color_Selector_h

#include "Helpers.h"

#include "PIGeneral.h"
#include "asTypes.h"

#include <memory>

class ComposerUI;
class Color_Selector;

// Base class for handling the color flag and color slider controls
// The z functions refer to the color plane displayed in the color slider control
// The y functions refer to the color plane used for the vertical part of the color flag control
// The x functions refer to the color plane used for the horizontal part of the color flag control
class	Color_Selector
{
	protected:
		// Constructor:
		Color_Selector() : mWebSafe( false ) {}

	public:
		// Destructor:
		virtual	~Color_Selector() {};
		
		// you must implement this...
		virtual short*		GetColor() = 0;
		
		// Mode getters:
		virtual ASInt32		GetCurrentMode() const = 0;
		virtual SkipBlock	GetSkipBlock() const = 0;
		
		virtual bool		GetWebSafe() const { return mWebSafe; }
		virtual void		SetWebSafe( bool v ) { mWebSafe = v; }

		// Updaters:
		virtual void		UpdateZBuffer( const PSPixelMap& map );
		virtual void		UpdateXYBuffer( const PSPixelMap& map );

		bool				SetX( short new_value, ComposerUI& runner );
		bool				SetY( short new_value, ComposerUI& runner );
		bool				SetZ( short new_value, ComposerUI& runner );

		// Coordinate getters:
		virtual short		GetX() = 0;
		virtual short		GetY() = 0;
		virtual short		GetZ() = 0;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Base class to check for changes to the display
			class Check_For_Update
			{
				public:
					// Destructor -- where this code stupidly does its work.
					virtual	~Check_For_Update() = 0;

				protected:
					// Constructor:
					Check_For_Update( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner, Color_Selector& p_selector, const short p_color[3], int p_z_index );

					// Update the display if the color has changed
					void CheckForChange( const short new_color[3] );

					// Return the originating selector
					Color_Selector&	selector() { return m_selector; }

				private:
					const PSPixelMap&	m_Z_map;
					const PSPixelMap&	m_XY_map;
					ComposerUI&			m_runner;
					Color_Selector&		m_selector;
					short				m_color[3];
					const int			m_z_index;
			};

		// Class to check for changes to the display
		typedef std::auto_ptr<Check_For_Update>	Update_Check;

		// Initiate a check for changes to the display when the returned object is destroyed
		virtual Update_Check BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner ) = 0;

	protected:
		// these store a new value in the external data space
		virtual void		StoreX( short ) = 0;
		virtual void		StoreY( short ) = 0;
		virtual void		StoreZ( short ) = 0;

		// Common functions for filling lines of a color map
		// Fill a color plane in a pixel map with a single color
		void fill_plane( const ASUInt8 color, ASUInt8* line, const ASUInt8* last_line, const int32 row_bytes );
		
		// Partially fill half a color plane in a pixel map with one color, then finish the plane with another
		void fill_plane( const ASUInt8 real_color, const ASUInt8 default_color, ASUInt8* line, const ASUInt8* last_line, const int32 row_bytes );
		
		// Fill a color plane in a pixel map with a decrementing value from [255-0] for each line
		void decrement_plane( ASUInt8* line, const ASUInt8* last_line, const int32 row_bytes );
		
		// Fill a color plane in a pixel map with a decrementing value from [255-0] in each line
		void fill_row( ASUInt8* line, const ASUInt8* last_line, const int32 row_bytes );

		virtual void SetNewColor( ComposerUI& runner ) const = 0;

		// Virtual functions to be overridden by derived classes: -- called by Update[XYZ]Buffer()...
		virtual void	fill_z_buffer_0( ASUInt8*, ASUInt8*, int ) {}
		virtual void	fill_z_buffer_1( ASUInt8*, ASUInt8*, int ) {}
		virtual void	fill_z_buffer_2( ASUInt8*, ASUInt8*, int ) {}

		virtual void	fill_xy_buffer_0( ASUInt8*, ASUInt8*, int ) {}
		virtual void	fill_xy_buffer_1( ASUInt8*, ASUInt8*, int ) {}
		virtual void	fill_xy_buffer_2( ASUInt8*, ASUInt8*, int ) {}

		// static methods
		static void	SetNewColor( ComposerUI& runner, const ASInt32 start, const ASInt32 end, const short source_space, const short* const source_data );


		// data
		bool mWebSafe;	// whether or not to show the colors as web safe
};


#endif

// EOF
