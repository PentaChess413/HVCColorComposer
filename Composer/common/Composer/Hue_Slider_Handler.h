/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Hue_Slider_Handler_h
#define Hue_Slider_Handler_h

#include "Custom_Draw_Control.h"
#include "Copy_Bit_Map.h"


class	Hue_Slider_Handler : public Custom_Draw_Control, public Copy_Bit_Map
{
	typedef Custom_Draw_Control	base;
	typedef Copy_Bit_Map		alt_base;

	public:
		Hue_Slider_Handler( ComposerUI& dialog_runner, const ASInt32 control_id, DisplayPixelsProc displayProc );

protected:
	virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
	virtual void		Draw( ADMDrawerRef environment );
};

#endif

// EOF
