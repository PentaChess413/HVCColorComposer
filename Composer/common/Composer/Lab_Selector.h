/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Lab_Selector_h
#define Lab_Selector_h

#include "Standard_X_Selector.h"
#include "Standard_Y_Selector.h"
#include "Standard_Z_Selector.h"


// Base Lab color selector class
// This class implements the common parts of the 3 Lab selector classes
class	Lab_Selector : public Color_Selector
{
	typedef Color_Selector base;

	public:
		// Destructor:
		virtual	~Lab_Selector() {}

		// Common Lab enums:
		enum { HALF_RAW = RAW_MAX / 2 };

	protected:
		// Constructor:
		Lab_Selector( short* p_Lab ) : m_Lab( p_Lab ) {}

		// Getters:
		short*			GetColor()	{ return m_Lab; }
		
		// don't allow websafe display as we aren't using RGB here
		virtual bool		GetWebSafe() const { return false; }
		virtual void		SetWebSafe( bool ) { mWebSafe = false; }

		// Getters required by the Standard @ Selector template classes:
		ASUInt8			default_x() const { return HALF_RAW; }
		ASUInt8			default_y() const { return HALF_RAW; }

		class			Lab_Update_Check;
		friend class	Lab_Update_Check;

	private:
		// Virtual functions implmenting common code, should not to be overridden:
		virtual ASInt32		GetCurrentMode() const;
		virtual SkipBlock	GetSkipBlock() const;
		
		virtual void		SetNewColor( ComposerUI& runner ) const;

		short*				m_Lab;
};

// L color selector class
// This class handles selecting the Lab L plane as the Z axis.
class L_Selector : public Standard_X_Selector<Lab_Selector>
{
	typedef Standard_X_Selector<Lab_Selector>	base;

	public:
		// Constructors:
		L_Selector( short* p_Lab ) : base( p_Lab ) {}
		virtual	~L_Selector() {}

	private:
		// Remaining virtual functions, class specific implementation
		virtual Update_Check	BeginUpdateCheck( const PSPixelMap& p_Z_map, const PSPixelMap& p_XY_map, ComposerUI& p_runner);

		virtual short			GetX();
		virtual short			GetY();
		virtual short			GetZ();
		virtual void			StoreX( short x );
		virtual void			StoreY( short y );
		virtual void			StoreZ( short z );
};



// a color selector class
// This class handles selecting the Lab a plane as the Z axis.
class A_Selector : public Standard_Y_Selector<Lab_Selector>
{
	typedef Standard_Y_Selector<Lab_Selector>	base;

	public:
		// Constructors:
		A_Selector( short* p_Lab ) : base( p_Lab ) {}
		virtual	~A_Selector() {}

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

// b color selector class
// This class handles selecting the Lab b plane as the Z axis.
class B_Selector : public Standard_Z_Selector<Lab_Selector>
{
	typedef Standard_Z_Selector<Lab_Selector>	base;

	public:
		// Constructors:
		B_Selector( short* p_Lab ) : base( p_Lab ) {}
		virtual	~B_Selector() {}

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
