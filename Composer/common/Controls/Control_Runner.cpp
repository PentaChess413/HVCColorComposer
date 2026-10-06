/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Control_Runner.h"
#include "Composer_UI.h"
#include "Custom_UI.h"

#include "admDialog.h"
#include "admNotifier.h"
#include "admBasic.h"

Control_Runner::Control_Runner( Dialog_Runner& dialog_runner, const ASInt32 control_id ) :
	base( dialog_runner.get_native_item( control_id ) )
{
	// check to see if somehow there is already an object associated with this control...
	if( user_data() )
		throw ASErr( kBadParameterErr );
	
	// set ourselves as the user data for this control
	set_user_data( this );
	
	// set various procs needed for implementing controls
	set_proc( DestroyCallback );
	set_proc( NotifyCallback );
	set_proc( TrackingCallback );
}

Control_Runner::~Control_Runner()
{
	if( native() )
	{
		set_user_data( NULL );
		set_proc( ADMItemDestroyProc( NULL ) );
		set_proc( ADMItemNotifyProc( NULL ) );
		set_proc( ADMItemTrackProc( NULL ) );
		clear();
	}
}

ComposerUI*	Control_Runner::GetComposerUI() const 
{ 
	return dynamic_cast<ComposerUI*>( Dialog() ); 
}


CustomUI* Control_Runner::GetLibrariesUI() const 
{ 
	return dynamic_cast<CustomUI*>( Dialog() ); 
}


Dialog_Runner* Control_Interface::Dialog() const
{
	return (Dialog_Runner*)ADM_Access::dialog_suite()->GetUserData( ADM_Access::item_suite()->GetDialog( m_control ) );
}

Control_Runner* Control_Runner::GetControlFromNative( ADMItemRef p_control )
{
	if( !p_control )
		return NULL;

	return (Control_Runner*)ADM_Access::item_suite()->GetUserData( p_control );
}

// This routine post processes key stroke events
ASBoolean ASAPI Control_Runner::TrackingCallback( ADMItemRef p_control, ADMTrackerRef tracker )
{
	try
	{
		Control_Runner*	control = GetControlFromNative( p_control );
		
		if( control )
			return control->HandleTracking( tracker );
	}
	catch(...)
	{
		XDEBUG_PRINT( "runaway exception caught! - in tracking callback\n" );
	}

	return false;
}

void ASAPI Control_Runner::NotifyCallback( ADMItemRef p_control, ADMNotifierRef notifier )
{
	try
	{
		Control_Runner*	control = GetControlFromNative( p_control );

		// check notification type -- make sure it isn't kADMTextSelectionChangedNotifier
		if( !ADM_Access::notifier_suite()->IsNotifierType( notifier, kADMTextSelectionChangedNotifier ) && control )
			control->HandleNotify( notifier );
	}
	catch(...)
	{
		XDEBUG_PRINT( "runaway exception caught! - in notify callback\n" );
	}
}

void ASAPI Control_Runner::DestroyCallback( ADMItemRef p_control )
{
	try
	{
		Control_Runner*	control = GetControlFromNative( p_control );
		
		// this if statement is here for debugging...
		if( control )
		{
//			XDEBUG_PRINT2( "Control: %d, %x\n", control->id(), control );
			delete control;
		}
	}
	catch(...)
	{
		XDEBUG_PRINT( "runaway exception caught! - in destroy callback\n" );
	}
}

void Control_Runner::HandleNotify( ADMNotifierRef )
{
//	XDEBUG_PRINT( "unhandled notify callback!\n" );
}

ASBoolean Control_Runner::HandleTracking( ADMTrackerRef tracker )
{
	return ADM_Access::item_suite()->DefaultTrack( native(), tracker );
}


// EOF
