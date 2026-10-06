/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Check_Box_Handler.h"



#include "admNotifier.h"

void Check_Box_Handler::HandleNotify( ADMNotifierRef notifier )
{
	// react to change by calling our callback if we have one... NOTE: the weird syntax is
	// a pointer to a member function!!
	if( ADM_Access::notifier_suite()->IsNotifierType(notifier, kADMUserChangedNotifier ) )
		(GetComposerUI()->GetColorSelector()->*m_updater)( *this );
}

void Check_Box_Handler::Update()
{
	XDEBUG_PRINT( "unimplemented Update() in Checkbox handler...\n" );
}
