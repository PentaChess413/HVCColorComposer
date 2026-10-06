/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef RGB_Selector_h
#define RGB_Selector_h

#include "Standard_X_Selector.h"
#include "Standard_Y_Selector.h"
#include "Standard_Z_Selector.h"


// Base RGB color selector class
// This class implements the common parts of the 3 RGB selector classes
class RGB_Selector : public Color_Selector
{
	typedef Color_Selector base;

	public:
		enum
		{
			RED = 0,
			GREEN,
			BLUE
		};

		// Destructor:
		virtual	~RGB_Selector()	{}

	protected:
		// Constructor:
		RGB_Selector( short* p_RGB ) : m_RGB( p_RGB ) {}

		// Getters:
		virtual short*		GetColor() { return m_RGB; }

		// Getters required by the Standard @ Selector template classes:
		ASUInt8				default_x() const { return 0; }
		ASUInt8				default_y() const { return 0; }

		class				RGB_Update_Check;
		friend class		RGB_Update_Check;

	private:
		// Virtual functions implmenting common code, should not to be overridden:
		virtual ASInt32		GetCurrentMode() const;
		virtual SkipBlock	GetSkipBlock() const;

		virtual void		SetNewColor( ComposerUI& runner ) const;
		
		// a pointer to the RGB data
		short*				m_RGB;
};




// Red color selector class
// This class handles selecting the RGB Red plane as the Z axis.
class	Red_Selector : public Standard_X_Selector<RGB_Selector>
{
	typedef Standard_X_Selector<RGB_Selector>	base;

	public:
		// Constructors:
		Red_Selector( short* p_RGB ) : base( p_RGB ) {}
		virtual	~Red_Selector()	{}

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



// Green color selector class
// This class handles selecting the RGB Green plane as the Z axis.
class Green_Selector : public Standard_Y_Selector<RGB_Selector>
{
	typedef Standard_Y_Selector<RGB_Selector> base;

	public:
		// Constructors:
		Green_Selector( short* p_RGB ) : base( p_RGB ) {}
		virtual	~Green_Selector() {}

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




// Blue color selector class
// This class handles selecting the RGB Blue plane as the Z axis.
class Blue_Selector : public Standard_Z_Selector<RGB_Selector>
{
	typedef Standard_Z_Selector<RGB_Selector>	base;

	public:
		// Constructors:
		Blue_Selector( short* p_RGB ) : base( p_RGB ) {}
		virtual	~Blue_Selector() {}

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
