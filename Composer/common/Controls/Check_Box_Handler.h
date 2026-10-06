/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Check_Box_Handler_h
#define Check_Box_Handler_h

#include "Control_Runner.h"
#include "Composer_UI.h"


class	Check_Box_Handler : public Control_Runner
{
	typedef Control_Runner	base;

	public:
		Check_Box_Handler( ComposerUI& dialog_runner, const ASInt32 control_id, ControlRunnerCallback p_updater ) :
 						base( dialog_runner, control_id ), 
 						m_updater( p_updater ) 
 		{}

	protected:
		virtual void	HandleNotify( ADMNotifierRef notifier );

	private:
		virtual void	Update();

		ControlRunnerCallback m_updater;
};

#endif

// EOF
