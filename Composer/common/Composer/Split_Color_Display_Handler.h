/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Split_Color_Display_Handler_h
#define Split_Color_Display_Handler_h

#include "Custom_Draw_Control.h"
#include "MCColor.h"


class Split_Color_Display_Handler : public Custom_Draw_Control
{
	typedef Custom_Draw_Control	base;

	public:
		Split_Color_Display_Handler( Dialog_Runner& dialog_runner, const ASInt32 control_id, MCColorRef p_left_color, MCColorRef p_right_color ) :
			base( dialog_runner, control_id ),
			m_left_color( p_left_color ),
			m_right_color( p_right_color ),
			m_tracking_right( false )
			{}

	protected:
		// overrides
		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		Draw( ADMDrawerRef environment );
	
		// data
		MCColorRef	m_left_color;
		MCColorRef	m_right_color;
		bool		m_tracking_right;
};

#endif

// EOF
