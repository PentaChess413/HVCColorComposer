/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef _H_MCDualColorSwatch
#define _H_MCDualColorSwatch

#include "Split_Color_Display_Handler.h"


class MCDualColorSwatch : public Split_Color_Display_Handler
{
	typedef Split_Color_Display_Handler	base;

	public:
		MCDualColorSwatch( Dialog_Runner& dialog_runner, const ASInt32 control_id, MCColorRef p_left_color, MCColorRef p_right_color );
		virtual ~MCDualColorSwatch() {}
		
		// see if the right side is selected... 
		bool	RightSideSelected() { return m_tracking_right; }
		void	SetRightSideSelected( bool right ) { m_tracking_right = right; }
		
	protected:
		// overrides
		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		Draw( ADMDrawerRef environment );
};

#endif // !_H_MCDualColorSwatch

// EOF
