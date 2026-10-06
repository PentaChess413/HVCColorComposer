/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Radio_Button_Group_h
#define Radio_Button_Group_h

#include "ADMTypes.h"

class ComposerUI;
class Dialog_Runner;


class Radio_Button_Group
{
	public:
		ASInt32			GetCurrentButton() const { return m_current_button; }
		void			SetCurrentButton( ASInt32 new_button );

	protected:
						Radio_Button_Group( ComposerUI& p_dialog_runner, const ASInt32 first_button, const ASInt32 last_button );
		virtual			~Radio_Button_Group() = 0;

		Dialog_Runner&	dialog() const { return m_dialog_runner; }
		ADMItemRef		get_item( ASInt32 button );

		void			append_button_list( ASInt32 first_button, const ASInt32 last_button );

	private:
		ASInt32			m_current_button, m_low_id, m_high_id, m_control_count;
		Dialog_Runner&	m_dialog_runner;

		virtual bool	ButtonChanging( ASInt32 new_button ) = 0;	// Return true to allow the change

		void			HandleNotify( ADMItemRef p_control );

		void			add_button( ASInt32 button );
		void			remove_button( ADMItemRef target );
		
		// static routines
		static ASAPI void NotifyCallback( ADMItemRef p_control, ADMNotifierRef notifier );
		static ASAPI void DestroyCallback( ADMItemRef p_control );
};

#endif

// EOF
