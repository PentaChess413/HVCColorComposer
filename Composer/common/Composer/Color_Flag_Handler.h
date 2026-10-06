/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Color_Flag_Handler_h
#define Color_Flag_Handler_h

#include "Custom_Draw_Control.h"
#include "Copy_Bit_Map.h"


class	HVC_RGB_Color_Pair;

class Color_Flag_Handler : public Custom_Draw_Control, public Copy_Bit_Map
{
	typedef Custom_Draw_Control	base;
	typedef Copy_Bit_Map		alt_base;

	public:
		Color_Flag_Handler( ComposerUI& dialog_runner, const ASInt32 control_id, DisplayPixelsProc proc );

		void SetPosition( short x_coord, short y_coord )
		{
			mX = x_coord;
			mY = y_coord;
		}

	protected:
		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		Draw( ADMDrawerRef environment );

	private:
		short				mX;
		short				mY;
	//	const ASPoint		indicator_offset;

		void				DrawIndicator( ADMDrawerRef environment, const ASRect& bounds ) const;
};

#endif

// EOF
