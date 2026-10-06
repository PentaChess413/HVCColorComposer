/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Dialog_Runner_h
#define Dialog_Runner_h

#include "admDialog.h"
#include "ADM_Access.h"

#include "Control_Runner.h"

class Dialog_Runner
{
	public:
		Dialog_Runner();
		virtual ~Dialog_Runner();
		
		// run and display the dialog
		virtual ASInt32			run();
		virtual void			end_modal( ASInt32 result_id, ASBoolean cancelling );

		virtual Control_Runner*	get_item( ASInt32 item_id );
		virtual ADMItemRef		get_native_item( ASInt32 item_id ) { return ADM_Access::dialog_suite()->GetItem( mDialog, item_id ); }

		// control helpers.
		Control_Interface		GetControl( ASInt32 item_id )					 { return get_item_interface( item_id ); }
		void					InvalidateRect( ASRect* );
		void					InvalidateControl( ASInt32 item_id )			 { GetControl( item_id ).invalidate(); }
		void					ShowHideControl( ASInt32 item_id, bool shide )	 { GetControl( item_id ).set_visible( shide ); }
		Control_Interface		get_item_interface( ASInt32 item_id )			 { return get_native_item( item_id ); }
		void					show_hide_range( ASInt32 item_start, ASInt32 item_end, bool shide );

		virtual void			UpdateControls( ASInt32 first_item, ASInt32 last_item );
		virtual ADMDialogRef	GetNative() { return mDialog; }

		
		// new routines dealing with reveal/hide...
		virtual void			GetBoundsRect( ASRect* outBoundsRect, bool getClientRect = false );
		virtual void			Size( ASInt32 inWidth, ASInt32 inHeight );

	protected:
		// create the dialog...
		virtual bool Create( SPPluginRef pluginRef, const char* p_name, ASInt32 p_dialog_id );
		
		// static routines...
		static ASAPI ASErr	InitCallback( ADMDialogRef pDialog );
		static ASAPI void	DialogFinishedCallback(ADMItemRef p_control, ADMNotifierRef notifier);

		void				SetDialogFinishedCallback( ASInt32 item_id ) { get_item_interface( item_id ).set_proc( DialogFinishedCallback ); }
		
	private:
		ADMDialogRef		mDialog;
};

inline void Dialog_Runner::show_hide_range( ASInt32 item_start, ASInt32 item_end, bool shide )
{
	while( item_start != item_end )
		ShowHideControl( item_start++, shide );
}

#endif

// EOF
