/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Quick_Colors_Handler_h
#define Quick_Colors_Handler_h

#include "Custom_Draw_Control.h"
#include "MCColor.h"

const short kNumQuickColors = 12;


class Quick_Colors_Handler : public Custom_Draw_Control
{
	typedef Custom_Draw_Control	base;

	public:
		Quick_Colors_Handler( ComposerUI& dialog_runner, const ASInt32 control_id );

		void				SetCurrentColor( MCColorRef );
		MCColor*			GetCurrentColor();

		void				SetColor( MCColorRef, short index );
		MCColor*			GetColor( short index );

		void				SetSelection( short index );
		short				GetSelection();
		
	protected:
		/// overrides
		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		Draw( ADMDrawerRef environment );

	private:
		MCColor		mColors[kNumQuickColors];
		ASRect		m_tracking_bounds;
		short		mIndex;
};

#endif

// EOF
