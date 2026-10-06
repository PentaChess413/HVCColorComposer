/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Radio_Button_Group.h"
#include "Composer_UI.h"

#include "admDialog.h"
#include "admNotifier.h"


Radio_Button_Group* get_runner(ADMItemRef p_control)
{
	if( !p_control )
		throw ASErr (kBadParameterErr);

	Radio_Button_Group*	runner = static_cast<Radio_Button_Group*>( ADM_Access::item_suite()->GetUserData( p_control ) );

	if( !runner )
		throw ASErr( kBadParameterErr );

	return runner;
}

Radio_Button_Group::Radio_Button_Group( ComposerUI& p_dialog_runner, ASInt32 first_button, ASInt32 last_button ) :
					m_current_button( last_button ), 
					m_low_id( first_button ),
					m_high_id( last_button ), 
					m_control_count( 0 ), 
					m_dialog_runner( p_dialog_runner )
{
	append_button_list( first_button, last_button );
	SetCurrentButton( first_button );
}


inline void Radio_Button_Group::remove_button( ADMItemRef target )
{
	if( this == ADM_Access::item_suite()->GetUserData( target ) )
	{
		ADM_Access::item_suite()->SetUserData( target, NULL );
		ADM_Access::item_suite()->SetNotifyProc( target, NULL );
		ADM_Access::item_suite()->SetDestroyProc (target, NULL );
	}
}


Radio_Button_Group::~Radio_Button_Group()
{
	if (0 < m_control_count)
		for (; m_high_id >= m_low_id; ++m_low_id)
			remove_button( m_dialog_runner.get_native_item( m_low_id ) );
}


inline void Radio_Button_Group::add_button( ASInt32 button )
{
	ADMItemRef	target = get_item( button );

	if( NULL != ADM_Access::item_suite()->GetUserData( target ) )
		throw ASErr( kBadParameterErr );

	ADM_Access::item_suite()->SetUserData( target, this );
	ADM_Access::item_suite()->SetDestroyProc( target, &DestroyCallback );
	ADM_Access::item_suite()->SetNotifyProc( target, &NotifyCallback );
}


void Radio_Button_Group::append_button_list(ASInt32 first_button, const ASInt32 last_button)
{
	if (0 > last_button || 0 > first_button || first_button > last_button)
		throw paramErr;

	if (m_low_id > first_button)
		m_low_id = first_button;

	if (m_high_id < last_button)
		m_high_id = last_button;

	do
	{
		add_button(first_button);
	} while (last_button >= ++first_button);
}

ADMItemRef Radio_Button_Group::get_item(ASInt32 button)
{
	return m_dialog_runner.get_native_item(button);
}

void Radio_Button_Group::SetCurrentButton( ASInt32 new_button )
{
	m_current_button = new_button;
	ADM_Access::item_suite()->SetBooleanValue( get_item( m_current_button ), false );
	ADM_Access::item_suite()->SetBooleanValue( get_item( new_button ), true );
}

inline void Radio_Button_Group::HandleNotify( ADMItemRef p_control )
{
	const ASInt32	new_button = ADM_Access::item_suite()->GetID( p_control );

	if( new_button != m_current_button && ButtonChanging( new_button ) )
	{
		ADM_Access::item_suite()->SetBooleanValue( p_control, true );
		ADM_Access::item_suite()->SetBooleanValue( get_item( m_current_button ), false );
		m_current_button = new_button;
	}
}

void ASAPI Radio_Button_Group::NotifyCallback( ADMItemRef p_control, ADMNotifierRef notifier )
{
	try
	{
		if (ADM_Access::notifier_suite()->IsNotifierType( notifier, kADMUserChangedNotifier ) )
			get_runner(p_control)->HandleNotify( p_control );
	}
	catch(...)
	{
	}
}

void ASAPI Radio_Button_Group::DestroyCallback( ADMItemRef p_control )
{
	try
	{
		Radio_Button_Group*	runner = get_runner( p_control );

		if( 0 == --runner->m_control_count )
			delete runner;
	}
	catch(...)
	{
	}
}


// EOF
