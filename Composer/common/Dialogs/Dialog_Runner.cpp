/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Dialog_Runner.h"

#include "admItem.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////

Dialog_Runner::Dialog_Runner() : mDialog( NULL )
{
}

Dialog_Runner::~Dialog_Runner()
{
	if( mDialog )
		ADM_Access::dialog_suite()->Destroy( mDialog );
}



///////////////////////////////////////////////////////////////////////////////////////////////////////

bool Dialog_Runner::Create( SPPluginRef pluginRef, const char* p_name, ASInt32 p_dialog_id )
{
	// force roman font
	ADM_Access::dialog_suite()->Create( pluginRef, p_name, p_dialog_id, kADMModalDialogStyle, &InitCallback, this, 0 );
//	ADM_Access::dialog_suite()->Create( pluginRef, p_name, p_dialog_id, kADMModalDialogStyle, &InitCallback, this, true );

	if( !mDialog )
		return false;

//	ADM_Access::dialog_suite()->SetUserData( mDialog, this );

	return true;
}



ASAPI ASErr Dialog_Runner::InitCallback( ADMDialogRef pDialog )
{
	Dialog_Runner* me = (Dialog_Runner*)ADM_Access::dialog_suite()->GetUserData( pDialog );
	if( !me )
		return kSPBadParameterError;
		
	// now set a reference to ourselves
	me->mDialog = pDialog;
	return kSPNoError;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////

Control_Runner* Dialog_Runner::get_item( ASInt32 item_id )
{
	return (Control_Runner*)ADM_Access::item_suite()->GetUserData( get_native_item( item_id ) );
}

void Dialog_Runner::UpdateControls( ASInt32 first_item, ASInt32 last_item )
{
	while( first_item <= last_item )
	{
		Control_Runner* control = get_item( first_item++ );
	
		// don't crash when items are missing...
		if( control )
			control->Update();
	}
}

ASInt32 Dialog_Runner::run()
{
	ADM_Access::dialog_suite()->Show( mDialog, true );

	return ADM_Access::dialog_suite()->DisplayAsModal( mDialog );
}

void Dialog_Runner::end_modal( ASInt32 result_id, ASBoolean cancelling )
{
	ADM_Access::dialog_suite()->EndModal( mDialog, result_id, cancelling );
	ADM_Access::dialog_suite()->Show( mDialog, false );
}

void Dialog_Runner::GetBoundsRect( ASRect* outBoundsRect, bool getClientRect )
{
	if( getClientRect )
		ADM_Access::dialog_suite()->GetLocalRect( mDialog, outBoundsRect );
	else
		ADM_Access::dialog_suite()->GetBoundsRect( mDialog, outBoundsRect );
}


void Dialog_Runner::Size( ASInt32 inWidth, ASInt32 inHeight )
{
	ADM_Access::dialog_suite()->Size( mDialog, inWidth, inHeight );
}

ASAPI void Dialog_Runner::DialogFinishedCallback( ADMItemRef controlRef, ADMNotifierRef )
{
	Control_Interface ct( controlRef );
	
	// here we gain access to the Composer UI and then call the end model method...
	ct.Dialog()->end_modal( ct.id(), ASBoolean( true ) );
}


void Dialog_Runner::InvalidateRect( ASRect* rect )
{
	ADM_Access::dialog_suite()->InvalidateRect( mDialog, rect );
}


// EOF
