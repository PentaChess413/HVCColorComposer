/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include "PhotoShopSDK.h"

#include "MCBookPopup.h"
#include "MCPrefs.h"
#include "Custom_UI.h"


const DescriptorKeyID	kLibrariesKey = 'MLIB';
const char*				kLibrariesPrefsUniqueID	= "4F72FCD0-E952-11D8-A01B-0030657D12DB";


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
//////////////////////////////////////////////////////////////////////////////////////////////


MCBookPopup::MCBookPopup( Dialog_Runner& dialog_runner, const ASInt32 control_id, MCColorLibraries& books ) :
	base( dialog_runner, control_id ),
	mBooks( books )
{
#if DO_PREFS
	// Acquire the callback objects
	AutoSuite<PSDescriptorRegistryProcs>	registryProcs(   kPSDescriptorRegistrySuite, kPSDescriptorRegistrySuiteVersion );
	AutoSuite<PSActionDescriptorProcs>		descriptorProcs( kPSActionDescriptorSuite,   kPSActionDescriptorSuiteVersion );
#endif

	// fill the list with the color books
	ADMListRef myList = GetList();

	if( myList )
	{
		// add all the items in the color libraries folder...
		for( Int32 i = 0; i < mBooks.GetNumBooks(); i++ )
		{
			RVWideNameField name;
			if( mBooks.GetBookTitle( i, name ) )
			{
				ADMEntryRef entry = ADM_Access::list_suite()->InsertEntry( myList, i );
				if( entry )
					ADM_Access::entry_suite()->SetTextW( entry, name );
			}
		}

#if DO_PREFS
		// Get the preferences object
		PIActionDescriptor prefs = NULL;
		OSErr err = registryProcs->Get( kLibrariesPrefsUniqueID, &prefs );
		
		if( !err && prefs )
		{
			ASInt32 defaultItem = 0;
			ASInt32 dataLen     = 0;
			descriptorProcs->GetDataLength( prefs, kLibrariesKey, &dataLen );
			
			// Retrive the selected color book index
			if( dataLen == sizeof( ASInt32 ) && !descriptorProcs->GetData( prefs, kLibrariesKey, &defaultItem ) )
			{
				// get defaultItem
				ADMEntryRef menuItem = ADM_Access::list_suite()->IndexEntry( myList, defaultItem );
				
				if( menuItem )
				{
					// now set the default item...
					ADM_Access::entry_suite()->Select( menuItem, true );
					mBooks.SetCurrentBook( defaultItem );
				}
			}
			descriptorProcs->Free( prefs );
		}
		else
#endif
		{
			// default the menu...
			ADMEntryRef menuItem = ADM_Access::list_suite()->IndexEntry( myList, 0 );
				
			if( menuItem )
			{
				// now set the default item...
				ADM_Access::entry_suite()->Select( menuItem, true );
				mBooks.SetCurrentBook( 0 );
			}
		}
	}
}

MCBookPopup::~MCBookPopup()
{
	// save the current selection...  !!@ note, if the order of the items changes this will pick the wrong item !!@
	ASInt32 index = GetSelection();

#if DO_PREFS
	// Acquire the callback objects
	AutoSuite<PSDescriptorRegistryProcs>	registryProcs(   kPSDescriptorRegistrySuite, kPSDescriptorRegistrySuiteVersion );
	AutoSuite<PSActionDescriptorProcs>		descriptorProcs( kPSActionDescriptorSuite,   kPSActionDescriptorSuiteVersion );
	
	// Create the preferences object
	PIActionDescriptor prefs = NULL;
	OSErr err = descriptorProcs->Make( &prefs );
	if( err || !prefs )
		return;

	// Save the selected color book's name in the preferences object
	descriptorProcs->PutData( prefs, kLibrariesKey, sizeof( index ), &index );

	// Store the preferences
	registryProcs->Register( kLibrariesPrefsUniqueID, prefs, true );
	
	descriptorProcs->Free( prefs );
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Methods
//
//////////////////////////////////////////////////////////////////////////////////////////////


void MCBookPopup::HandleNotify( ADMNotifierRef )
{
	CustomUI* ui = GetLibrariesUI();
	if( !ui )
		return;
	
	ASInt32 index = GetSelection();
	
	// tell the color libraries
	mBooks.SetCurrentBook( index );
	
	// start all over-- the easy way, by selecting the selected color, 
	// we force the code to search the new color book.
	ui->SelectNewColor( ui->mSelectedColorRef );
}


ASInt32 MCBookPopup::GetSelection()
{
	// gain access to the popup...
	ADMListRef myList = GetList();
	if( !myList )
		return -1;
		
	ADMEntryRef entry = ADM_Access::list_suite()->GetActiveEntry( myList );
	if( !entry )
		return -1;
		
	return ADM_Access::entry_suite()->GetIndex( entry );
}
// EOF
