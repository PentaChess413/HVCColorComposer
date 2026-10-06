/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef HVC_Hue_Text_Display_h
#define HVC_Hue_Text_Display_h

#include "Transformed_Text_Display.h"


class HVC_Hue_Text_Display : public Transformed_Int_Text_Display
{
	typedef Transformed_Int_Text_Display	base;

	public:
		HVC_Hue_Text_Display( ComposerUI& dialog_runner, const ASInt32 control_id, short& p_field, ColorTextCallback p_updater );

	protected:
		virtual void	Update();
		virtual void	HandleTextTracking( ADMTrackerRef tracker );
		virtual void	HandleNotify( ADMNotifierRef notifier );
};

#endif

// EOF
