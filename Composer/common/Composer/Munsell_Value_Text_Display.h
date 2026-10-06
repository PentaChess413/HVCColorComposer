/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Munsell_Value_Text_Display_h
#define Munsell_Value_Text_Display_h

#include "Color_Text_Display.h"


class	ComposerUI;

class	Munsell_Value_Text_Display : public Color_Text_Display
{
	typedef Color_Text_Display	base;

	public:
		Munsell_Value_Text_Display( ComposerUI& dialog_runner, const ASInt32 edit_text_id, short& p_field );

	protected:
		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		HandleNotify( ADMNotifierRef notifier );

		void				update_value(const ASInt32 new_value);

	private:
		short&	m_field;
};


#endif
