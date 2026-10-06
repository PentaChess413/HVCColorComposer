/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef HSB_Selector_h
#define HSB_Selector_h

#include "Color_Selector.h"


// Base HSB color selector class
// This class implements the common parts of the 3 HSB selector classes
class HSB_Selector_Base : public Color_Selector
{
	typedef Color_Selector	base;

	public:
		// Destructor:
		virtual	~HSB_Selector_Base() {}

		enum 
		{	
			HUE, 
			SATURATION, 
			BRIGHTNESS 
		};

	protected:
		// Constructor:
		HSB_Selector_Base( short* p_HSB ) : m_HSB( p_HSB ) {}

		// Getters:
		virtual short*			GetColor()			{ return m_HSB; }
		virtual const short*	GetColor() const	{ return m_HSB; }

		class			HSB_Update_Check;
		friend class	HSB_Update_Check;

		// Initializers required by the HSB_Selector template class:
		virtual void	initialize_x_buffer_color( short hsbColor[3] ) const = 0;
		virtual void	initialize_y_buffer_color( short hsbColor[3] ) const = 0;
		virtual void	initialize_z_buffer_color( short hsbColor[3], short hsbStaticColor[3] ) const = 0;

		// Incrementers required by the HSB_Selector template class:
		virtual void	increment_x_buffer_color( short hsbColor[3] ) const = 0;
		virtual void	increment_y_buffer_color( short hsbColor[3] ) const = 0;
		virtual void	increment_z_buffer_color( short hsbColor[3], short hsbStaticColor[3] ) const = 0;


		// Color space conversions:
		static void		hsb_2_rgb( short rgb_color[3], const short hsb_color[3] );

	private:
		// Virtual functions implmenting common code, should not to be overridden:
		virtual ASInt32		GetCurrentMode() const;
		virtual SkipBlock	GetSkipBlock() const;
		virtual void		UpdateZBuffer( const PSPixelMap& map );
		virtual void		UpdateXYBuffer( const PSPixelMap& map );
		
		virtual void		SetNewColor( ComposerUI& runner ) const;

		short*				m_HSB;
//		mutable short		m_hue_offset;		// !!@ mutable -- only needed from const routines...
};


// Hue color selector class
// This class handles selecting the HSB Hue plane as the Z axis.
class	HSB_Hue_Selector : public HSB_Selector_Base
{
	typedef HSB_Selector_Base	base;

	public:
		// Constructor:
		HSB_Hue_Selector( short* p_HSB ) : base( p_HSB ) {}
		virtual	~HSB_Hue_Selector() {}

	protected:
		// Initializers required by the HSB_Selector template class:
		virtual void			initialize_x_buffer_color( short hsbColor[3] ) const;
		virtual void			initialize_y_buffer_color( short hsbColor[3] ) const;
		virtual void			initialize_z_buffer_color( short hsbColor[3], short hsbStaticColor[3] ) const;

		// Incrementers required by the HSB_Selector template class:
		virtual void			increment_x_buffer_color( short hsbColor[3] ) const;
		virtual void			increment_y_buffer_color( short hsbColor[3] ) const;
		virtual void			increment_z_buffer_color( short hsbColor[3], short hsbStaticColor[3] ) const;

	private:
		// Remaining virtual functions, class specific implementation
		virtual Update_Check	BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner );
		
		virtual short			GetX();
		virtual short			GetY();
		virtual short			GetZ();
		virtual void			StoreX( short x );
		virtual void			StoreY( short y );
		virtual void			StoreZ( short z );
};

// Saturation color selector class
// This class handles selecting the HSB Saturation plane as the Z axis.
class Saturation_Selector : public HSB_Selector_Base
{
	typedef HSB_Selector_Base base;

	public:
		// Constructor:
		Saturation_Selector( short* p_HSB ) : base( p_HSB ) {}
		virtual	~Saturation_Selector()	{}

	protected:
		// Initializers required by the HSB_Selector template class:
		virtual void			initialize_x_buffer_color( short hsbColor[3] ) const;
		virtual void			initialize_y_buffer_color( short hsbColor[3] ) const;
		virtual void			initialize_z_buffer_color( short hsbColor[3], short hsbStaticColor[3] ) const;

		// Incrementers required by the HSB_Selector template class:
		virtual void			increment_x_buffer_color( short hsbColor[3] ) const;
		virtual void			increment_y_buffer_color( short hsbColor[3] ) const;
		virtual void			increment_z_buffer_color( short hsbColor[3], short hsbStaticColor[3] ) const;

	private:
		// Remaining virtual functions, class specific implementation
		virtual Update_Check	BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner );

		virtual short			GetX();
		virtual short			GetY();
		virtual short			GetZ();
		virtual void			StoreX( short x );
		virtual void			StoreY( short y );
		virtual void			StoreZ( short z );

		mutable bool			m_half;		// !!@ argh
};

// Brightness color selector class
// This class handles selecting the HSB Brightness plane as the Z axis.
class	Brightness_Selector : public HSB_Selector_Base
{
	typedef HSB_Selector_Base base;

	public:
		// Constructor:
		Brightness_Selector( short* p_HSB ) : base( p_HSB ) {}
		virtual	~Brightness_Selector()	{}

	protected:
		// Initializers required by the HSB_Selector template class:
		virtual void			initialize_x_buffer_color( short hsbColor[3] ) const;
		virtual void			initialize_y_buffer_color( short hsbColor[3] ) const;
		virtual void			initialize_z_buffer_color( short hsbColor[3], short hsbStaticColor[3] ) const;

		// Incrementers required by the HSB_Selector template class:
		virtual void			increment_x_buffer_color( short hsbColor[3] ) const;
		virtual void			increment_y_buffer_color( short hsbColor[3] ) const;
		virtual void			increment_z_buffer_color( short hsbColor[3], short hsbStaticColor[3] ) const;

	private:
		// Remaining virtual functions, class specific implementation
		virtual Update_Check	BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner );

		virtual short			GetX();
		virtual short			GetY();
		virtual short			GetZ();
		virtual void			StoreX( short x );
		virtual void			StoreY( short y );
		virtual void			StoreZ( short z );
};


#endif

// EOF
