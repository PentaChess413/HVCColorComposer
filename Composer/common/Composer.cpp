/*
 *
 *	Copyright 2005 Master Colors.
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

#include "PhotoshopSDK.h"

#include "Composer.h"
#include "Composer_UI.h"
#include "Selected_Color.h"
#include "Custom_UI.h"
#include "Helpers.h"

#include "MCLicense.h"

#include "admBasic.h"


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Defines
//
//////////////////////////////////////////////////////////////////////////////////////////////


// this dumps out all the CMY -> RGB data as requested by [PII Redacted]
// #define DUMP_FOR_ED //extended data

// !!@ fix me !!@
#ifndef MAX_PATH
#define MAX_PATH 1024
#endif


const OSType kPSHostSignature  = '8BIM';

#if WIN32
const char*  kPSHostNameString = "Photoshop.exe";
#else
const char*  kPSHostNameString = "Photoshop";
#endif

const int32  kPSMinimumHostVersion       = 8;
const int32  kElementsMinimumHostVersion = 4;

#ifndef custom_uiID
//	#define custom_uiID	AboutID + 200
	#define custom_uiID	16002
#endif

// GLOBAL needed for access to Adobe stuff ???
SPBasicSuite* sSPBasic = NULL;


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Code
//
//////////////////////////////////////////////////////////////////////////////////////////////

Composer::Composer()
{
}

void Composer::Pick()
{
	// startup the license code
	// void*		licenseData = ::LicenseStartup();
	ComposerUI* composerUI  = NULL;
	CustomUI*	customUI    = NULL;

	MCLib_Wrapper mcLib;	// doesn't throw... 

	// startup all the color conversion utilities...	
	MCColor::Startup( pickerRecord->colorServices, &mcLib );
	SelectedColor	selected_color( *pickerRecord );
	MCColor			originalColor( selected_color.GetOriginalColor() );

#ifdef DUMP_FOR_ED
	FILE* outputFile = ::fopen( "Freezer:cmy2sRgb.txt", "w" );
	
	if( outputFile )
	{
		int cIncrement = 5;
		int mIncrement = 5;
		int yIncrement = 5;
		
		// loop thru most CMY values
		for( int y = 0; y <= 100; y += yIncrement )
		{
			for( int m = 0; m <= 100; m += mIncrement )
			{
#if 0
				// this prints out the input data
				for( int c = 0; c <= 100; c += cIncrement )
				{
					::fprintf( outputFile, "%d %d %d\n", c, m, y );
				}
				::fprintf( outputFile, "\n" );
#else				
				// this prints out the output
				for( int c = 0; c <= 100; c += cIncrement )
				{
					// convert cmy to rgb
					short componentData[4];
					
					// convert from 0..100 to 255..0
					componentData[0] = (RAW_MAX - expand_percent( c ));
					componentData[1] = (RAW_MAX - expand_percent( m ));
					componentData[2] = (RAW_MAX - expand_percent( y ));
					componentData[3] = RAW_MAX;
					originalColor.SetColor( kCMYKSpace, componentData );
					
					// convert to RGB
					originalColor.GetColor( kRGBSpace, componentData );
					
					::fprintf( outputFile, "%d %d %d %d %d %d\n", c, m, y, componentData[0], componentData[1], componentData[2] );
				}
//				::fprintf( outputFile, "\n" );
#endif
			}
//			::fprintf( outputFile, "\n" );
		}
		::fclose( outputFile );	
	}

#else
	try
	{
		bool purchased = true;
		
		/*if( licenseData )		
			purchased = ::LicenseValid( licenseData );*/

		ASInt32				result = ComposerUI::Picker_CustomLibraries;
		ASInt32				out_error;
		ASInt32				out_error_data;
		
		// Create the picker dialog
		composerUI = new ComposerUI( selected_color, "HVC Color Composer", uiID );
		if( !composerUI )
			return;

		// we need to give the dialog access to the license data in case the user wants to purchase the code
		// composerUI->SetLicenseDataRef( licenseData );

		// Loop until OK or Cancel are clicked -- we beep for other IDs... !!@
		while( result == ComposerUI::Picker_CustomLibraries  )				
		{
			// Run the picker dialog
			result = composerUI->run();				

			if( result == ComposerUI::Picker_CustomLibraries || result == ComposerUI::Picker_Ok )
			{
				// !!@ We blow off 'resultSpace == -1' case 
				MCSpace	currentSpace = selected_color.GetColorNoConvert( NULL );
				
				if( currentSpace == kHVCSpace )
				{
					// extract the color from the selection and pass to host
					selected_color.GetColorForPS( MCSpace( pickerRecord->pickParms.sourceSpace ), (short*)pickerRecord->pickParms.colorComponents );
	
					// we did the conversion so now just make sure the record reflects this
					pickerRecord->pickParms.resultSpace = pickerRecord->pickParms.sourceSpace;
				}
				else
				{
					// extract the color from the selection and pass to host
					selected_color.GetColorForPS( currentSpace, (short*)pickerRecord->pickParms.colorComponents );
	
					// force the color space...
					pickerRecord->pickParms.resultSpace = currentSpace;
				}		

				if( result == ComposerUI::Picker_CustomLibraries )
				{
					if( !customUI )
						customUI = new CustomUI( selected_color, "Libraries", custom_uiID );

					// run the custom libraries dialog...			
					if( customUI )
						result = customUI->run();
						
					// if they pressed the "picker" button we need to set the color they picked as the original...
					if( result == ComposerUI::Picker_CustomLibraries || result == ComposerUI::Picker_Ok )
					{
						// extract the color from the selection and pass to host -- !!@ note: the color should never be in HVC here!
						MCSpace	resultSpace = selected_color.GetColorNoConvert( NULL );
						
						// convert only if somehow an HVC color sneaks thru (this can happen!)
						if( resultSpace == kHVCSpace )
							resultSpace = kRGBSpace;	// convert it to RGB for PS.
							
						selected_color.GetColorForPS( resultSpace, (short*)pickerRecord->pickParms.colorComponents );
						
						// we did the conversion so now just make sure the record reflects this
						// here we force PhotoShop to use the color space the color is in rather than converting it.
						pickerRecord->pickParms.resultSpace = resultSpace;
					}
				}

				// save the state NOTE: in purchase mode you should reset the clicks and not decrement them !!@
				if( purchased )
				{
					composerUI->ResetClicksLeft();
					composerUI->SaveState( false );
				}
				else
					composerUI->SaveState( true );
			}
			
			if( ComposerUI::Picker_CustomLibraries < result )
			{
				show_about_box();
				result = ComposerUI::Picker_CustomLibraries;
			}

			if( result == ComposerUI::Picker_Cancel )
			{
				// restore the color to the original color
				originalColor.GetColorForPS( MCSpace( pickerRecord->pickParms.sourceSpace ), (short*)pickerRecord->pickParms.colorComponents );
			
			
#if XDEBUG && WIN32
				// this is for the debug version only...
				// check for the control key...
				short result = ::GetAsyncKeyState( VK_CONTROL );
				
				if( result & 0x8000 )	// top of short indicates state
				{
					composerUI->ResetClicksLeft();
				}
#endif

				// save the state even when they pressed cancel...don't decrement clicks left in this case ever.
				composerUI->SaveState( false );
			}
			
			if( ADM_Access::basic_suite()->GetLastADMError( &out_error, &out_error_data ) )
				ADM_Access::basic_suite()->LightweightErrorAlert("An error has occured");
		
		}	// end while
	}
	catch(...)
	{	// Don't let C++ exceptions escape
		ADM_Access::basic_suite()->LightweightErrorAlert("An unknown error has occured");
	}
#endif
	
	
	// shutdown the licensing code
	//::LicenseShutdown( licenseData );
	
	// trash the dialogs...
	delete composerUI;
	delete customUI;
}


Plugin* AllocatePlugin( int16 selector, void * record )
{
	ADM_Access::initialize();
	
	// only check for version on invocation, not About...
	if( selector != pickerSelectorAbout )
	{
		// we assume we are running in PhotoShop- it's in the PiPL. -- this doesn't prevent us from running in PS "clones"
		// i.e. stripped down versions of PhotoShop that report back that they are photoshop even when they aren't-- doh!
		// so what we do is check the host signature...
		PIPickerParams* pickerRecord = static_cast<PIPickerParams*>( record );
		if( !pickerRecord || (pickerRecord && pickerRecord->hostSig != kPSHostSignature) )
			return NULL;

		bool photoShop = false;
		
		
		char szAppPath[MAX_PATH] = "";
		std::string strAppName;

#if WIN32
		::GetModuleFileName( NULL, szAppPath, MAX_PATH );

		// Extract name -- windows style path
		strAppName = szAppPath;
		strAppName = strAppName.substr( strAppName.rfind( "\\" ) + 1 );
#else
//		strAppName = 		!!@ implement me if needed
#endif

		// see what filename we end up with.
		if( strAppName.compare( kPSHostNameString ) == 0 )
			photoShop = true;

		// check version
		int32 major;
		int32 minor;
		int32 fix;
		::PIGetHostVersion( major, minor, fix );

		if( photoShop )
		{
			// don't construct if it's not a supported version...  
			// For now we don't support PS7 due to layout problems
			if( major < kPSMinimumHostVersion )
				return NULL;	
		}
		else
		{
			// we are in elements, we have only tested under version 4.0
			if( major < kElementsMinimumHostVersion )
				return NULL;	
		}
	}
		
	return new Composer;
}
 // fossified version: hi, im here because the sdk code was modified from stock and i cant distribute that so...
Plugin* AllocatePlugin(void)
{
	ADM_Access::initialize();
	return new Composer;
}

// EOF
