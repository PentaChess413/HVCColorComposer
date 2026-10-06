/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef ADM_Access_h
#define ADM_Access_h

#include "PhotoshopSDK.h"
 

#include "admBasic.h"
#include "admDialog.h"
#include "admItem.h"
#include "admNotifier.h"
#include "admTracker.h"
#include "admDrawer.h"
#include "ADMList.h"
#include "ADMEntry.h"

/*
struct ADMBasicSuite8;
struct ADMDialogSuite8;
struct ADMItemSuite8;
struct ADMNotifierSuite2;
struct ADMTrackerSuite1;
struct ADMDrawerSuite5;
*/

typedef ADMBasicSuite8		Basic_Suite;
typedef ADMDialogSuite8		Dialog_Suite;
typedef ADMItemSuite8		Item_Suite;
typedef ADMNotifierSuite2	Notifier_Suite;
typedef ADMTrackerSuite1	Tracking_Suite;
typedef ADMDrawerSuite5		Drawing_Suite;
typedef ADMListSuite3		List_Suite;
typedef ADMEntrySuite5		Entry_Suite;

class ADM_Access
{
	public:
		static const Basic_Suite*		basic_suite();

		static const Dialog_Suite*		dialog_suite();
		static const Item_Suite*		item_suite();
		static const Notifier_Suite*	notifier_suite();
		static const Tracking_Suite*	tracking_suite();
		static const Drawing_Suite*		drawing_suite();
		static const List_Suite*		list_suite();
		static const Entry_Suite*		entry_suite();

		static void						initialize();

	private:
		AutoSuite<Basic_Suite>		m_basic_suite;
		AutoSuite<Dialog_Suite>		m_dialog_suite;
		AutoSuite<Item_Suite>		m_item_suite;
		AutoSuite<Notifier_Suite>	m_notifier_suite;
		AutoSuite<Tracking_Suite>	m_tracking_suite;
		AutoSuite<Drawing_Suite>	m_drawing_suite;
		AutoSuite<List_Suite>		m_list_suite;
		AutoSuite<Entry_Suite>		m_entry_suite;

									ADM_Access();
};

#endif

// EOF
