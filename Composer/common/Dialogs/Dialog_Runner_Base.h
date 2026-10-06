/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Dialog_Runner_Base_h
#define Dialog_Runner_Base_h

#include "ADM_Access.h"

#include "admDialog.h"


class Dialog_Runner_Base
{
		friend class Dialog_Runner;

	public:
		ADMItemRef		get_native_item( ASInt32 item_id ) { return ADM_Access::dialog_suite()->GetItem( m_dialog, item_id ); }
		ADMDialogRef	GetNative() { return m_dialog; }

	protected:
		Dialog_Runner_Base() {}
		Dialog_Runner_Base( ADMDialogRef p_dialog ) : m_dialog( p_dialog ) {}
		
		~Dialog_Runner_Base() {}

	private:
		ADMDialogRef	m_dialog;
};

#endif

// EOF
