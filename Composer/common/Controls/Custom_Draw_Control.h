/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Custom_Draw_Control_h
#define Custom_Draw_Control_h

#include "Control_Runner.h"

class	Custom_Draw_Control : public Control_Runner
{
	typedef Control_Runner	base;

	protected:
		Custom_Draw_Control( Dialog_Runner& dialog_runner, const ASInt32 control_id ) :
			base( dialog_runner, control_id ) 
		{ 
			set_proc( DrawCallback ); 
		}
												
		virtual ~Custom_Draw_Control() { set_proc( ADMItemDrawProc( NULL ) ); }

		// required overrides
		virtual void				Update();

		// static utility functions
		static void					draw_frame(ADMDrawerRef environment, ASRect& bounds);
		static void					frame_draw(ADMDrawerRef environment, ASRect& bounds);
		static void					fill_with_color(ADMDrawerRef environment, const ASRect& bounds,	const ASRGBColor& color);

		static Custom_Draw_Control*	GetDrawControlFromNative( ADMItemRef p_control ) { return (Custom_Draw_Control*)GetControlFromNative( p_control ); }

	private:
		virtual void				Draw( ADMDrawerRef environment ) = 0;
		
		// draw callback calls the Draw() method.
		static void ASAPI			DrawCallback( ADMItemRef p_control, ADMDrawerRef environment );
};

#endif

// EOF
