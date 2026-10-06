/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "ADM_Access.h"

ADM_Access*	adm_access = NULL;

ADM_Access::ADM_Access()	:
			m_basic_suite(kADMBasicSuite, kADMBasicSuiteVersion8),
			m_dialog_suite(kADMDialogSuite, kADMDialogSuiteVersion8),
			m_item_suite(kADMItemSuite, kADMItemSuiteVersion8),
			m_notifier_suite(kADMNotifierSuite, kADMNotifierSuiteVersion2),
			m_tracking_suite(kADMTrackerSuite, kADMTrackerSuiteVersion),
			m_drawing_suite(kADMDrawerSuite, kADMDrawerSuiteVersion5),
			m_list_suite( kADMListSuite, kADMListSuiteVersion3 ),
			m_entry_suite( kADMEntrySuite, kADMEntrySuiteVersion5 )
{
}

void ADM_Access::initialize()
{
	if (!adm_access)
		adm_access = new ADM_Access();
}

const Basic_Suite* ADM_Access::basic_suite()
{
	return *adm_access->m_basic_suite;
}

const Dialog_Suite* ADM_Access::dialog_suite()
{
	return *adm_access->m_dialog_suite;
}

const Item_Suite* ADM_Access::item_suite()
{
	return *adm_access->m_item_suite;
}

const Notifier_Suite* ADM_Access::notifier_suite()
{
	return *adm_access->m_notifier_suite;
}

const Tracking_Suite* ADM_Access::tracking_suite()
{
	return *adm_access->m_tracking_suite;
}

const Drawing_Suite* ADM_Access::drawing_suite()
{
	return *adm_access->m_drawing_suite;
}

const List_Suite* ADM_Access::list_suite()
{
	return *adm_access->m_list_suite;
}

const Entry_Suite* ADM_Access::entry_suite()
{
	return *adm_access->m_entry_suite;
}
