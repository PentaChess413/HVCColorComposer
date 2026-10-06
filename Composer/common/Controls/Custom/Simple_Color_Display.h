/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Simple_Color_Display_h
#define Simple_Color_Display_h

#include "Custom_Draw_Control.h"
#include "MCColor.h"

class Simple_Color_Display : public Custom_Draw_Control
{
	typedef Custom_Draw_Control	base;

	public:
		Simple_Color_Display( Dialog_Runner& ui, const ASInt32 control_id, MCColorRef p_color ) :
			base( ui, control_id ), 
			mColorRef( p_color ) 
		{}


		MCColorRef		GetColorRef() const    { return mColorRef; }

	private:
		virtual void	Draw( ADMDrawerRef environment )
		{
			ASRect displayBounds = bounds( true );
			mColorRef.Draw( environment, &displayBounds );
		}
		
		// data
		MCColorRef		mColorRef;
};

#endif

// EOF
