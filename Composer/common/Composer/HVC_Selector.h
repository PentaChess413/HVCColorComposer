/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef HVC_Selector_h
#define HVC_Selector_h

#include "Color_Selector.h"

struct	HVCTrip;
class	MCLib_Wrapper;


// Base HVC color selector class
// This class implements the common parts of the 3 HVC selector classes
class HVC_Selector : public Color_Selector
{
	public:
		// Destructor:
		virtual	~HVC_Selector() {}

	protected:
		// Constructor:
		HVC_Selector( short* hvc, MCLib_Wrapper& mclib ) :
			m_hvc( hvc ),
			mMCLib( mclib )
		{
		}

		// Getters:
		virtual short*			GetColor()			{ return m_hvc; }
		virtual const short*	GetColor() const	{ return m_hvc; }

		class			HVC_Update_Check;
		friend class	HVC_Update_Check;

	private:
		virtual ASInt32		GetCurrentMode() const;
		virtual SkipBlock	GetSkipBlock() const;

		virtual void		SetNewColor( ComposerUI& runner ) const;

		// Virtual functions implmenting common code, should not to be overridden:
		virtual void		UpdateZBuffer( const PSPixelMap& map );
		virtual void		UpdateXYBuffer( const PSPixelMap& map );

		// Virtual functions to be overridden be derived classes:
		virtual void		initialize_x_buffer_color( HVCTrip& hvc_color ) const = 0;
		virtual void		initialize_y_buffer_color( HVCTrip& hvc_color ) const = 0;
		virtual void		initialize_z_buffer_color( HVCTrip& hvc_color, HVCTrip& hvc_hue_color ) const = 0;

		virtual void		increment_x_buffer_color( HVCTrip& hvc_color ) const = 0;
		virtual void		increment_y_buffer_color( HVCTrip& hvc_color ) const = 0;
		virtual void		increment_z_buffer_color( HVCTrip& hvc_color, HVCTrip& hvc_hue_color ) const = 0;

		virtual RGBTrip		get_background_color() const = 0;
		
		// data
		short*				m_hvc;
		MCLib_Wrapper&		mMCLib;
};



// Hue color selector class
// This class handles selecting the HVC Hue plane as the Z axis.
class HVC_Hue_Selector : public HVC_Selector
{
	typedef HVC_Selector base;

	public:
		// Constructors:
		HVC_Hue_Selector( short* p_HVC, MCLib_Wrapper& mclib ) :
			base( p_HVC, mclib ) 
		{}
			
		virtual	~HVC_Hue_Selector() {}

	private:
		// Remaining virtual functions, class specific implementation
		virtual Update_Check	BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner );

		virtual short			GetX();
		virtual short			GetY();
		virtual short			GetZ();
		virtual void			StoreX( short x );
		virtual void			StoreY( short y );
		virtual void			StoreZ( short z );

		virtual void			initialize_x_buffer_color( HVCTrip& hvc_color ) const;
		virtual void			initialize_y_buffer_color( HVCTrip& hvc_color ) const;
		virtual void			initialize_z_buffer_color( HVCTrip& hvc_color, HVCTrip& hvc_hue_color ) const;

		virtual void			increment_x_buffer_color( HVCTrip& hvc_color ) const;
		virtual void			increment_y_buffer_color( HVCTrip& hvc_color ) const;
		virtual void			increment_z_buffer_color( HVCTrip& hvc_color, HVCTrip& hvc_hue_color ) const;

		virtual RGBTrip			get_background_color() const;
};



// Value color selector class
// This class handles selecting the HVC Value plane as the Z axis.
class Value_Selector : public HVC_Selector
{
	typedef HVC_Selector base;

	public:
		// Constructors:
		Value_Selector( short* p_HVC, MCLib_Wrapper& mclib ) :
			base( p_HVC, mclib ) 
		{}
			
		virtual	~Value_Selector() {}

	private:
		// Remaining virtual functions, class specific implementation
		virtual Update_Check	BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner );

		virtual short			GetX();
		virtual short			GetY();
		virtual short			GetZ();
		virtual void			StoreX( short x );
		virtual void			StoreY( short y );
		virtual void			StoreZ( short z );

		virtual void			initialize_x_buffer_color( HVCTrip& hvc_color ) const;
		virtual void			initialize_y_buffer_color( HVCTrip& hvc_color ) const;
		virtual void			initialize_z_buffer_color( HVCTrip& hvc_color, HVCTrip& hvc_hue_color ) const;

		virtual void			increment_x_buffer_color( HVCTrip& hvc_color ) const;
		virtual void			increment_y_buffer_color( HVCTrip& hvc_color ) const;
		virtual void			increment_z_buffer_color( HVCTrip& hvc_color, HVCTrip& hvc_hue_color ) const;

		virtual RGBTrip			get_background_color() const;
};



// Chroma color selector class
// This class handles selecting the HVC Chroma plane as the Z axis.
class	Chroma_Selector : public HVC_Selector
{
	typedef HVC_Selector	base;

	public:
		// Constructors:
		Chroma_Selector( short* p_HVC, MCLib_Wrapper& mclib ) :
			base( p_HVC, mclib ) 
		{}
		
		virtual	~Chroma_Selector() {}

	private:
		// Remaining virtual functions, class specific implementation
		virtual Update_Check	BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner );

		virtual short			GetX();
		virtual short			GetY();
		virtual short			GetZ();
		virtual void			StoreX( short x );
		virtual void			StoreY( short y );
		virtual void			StoreZ( short z );

		virtual void			initialize_x_buffer_color( HVCTrip& hvc_color ) const;
		virtual void			initialize_y_buffer_color( HVCTrip& hvc_color ) const;
		virtual void			initialize_z_buffer_color( HVCTrip& hvc_color, HVCTrip& hvc_hue_color ) const;

		virtual void			increment_x_buffer_color( HVCTrip& hvc_color ) const;
		virtual void			increment_y_buffer_color( HVCTrip& hvc_color ) const;
		virtual void			increment_z_buffer_color( HVCTrip& hvc_color, HVCTrip& hvc_hue_color ) const;

		virtual RGBTrip			get_background_color() const;
};

#endif

// EOF
