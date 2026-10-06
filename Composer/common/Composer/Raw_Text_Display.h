/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Raw_Text_Display_h
#define Raw_Text_Display_h

#include "Control_Runner.h"

class ComposerUI;

class Raw_Text_Display : public Control_Runner
{
	typedef Control_Runner base;

	public:
		Raw_Text_Display( ComposerUI& dialog_runner, const ASInt32 control_id, const short p_rgb[4] );


		void update_color( const short p_rgb[4] );
		
		const short* GetColor() const { return m_rgb; }

	protected:
		virtual void		Update();

	private:
		short	m_rgb[4];

		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		HandleNotify( ADMNotifierRef notifier );

		void				update_value();
};

#endif

// EOF
