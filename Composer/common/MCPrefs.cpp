/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	Source code rewritten by: Alex Lelievre ([PII Redacted]), 2006.
 *
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "PhotoshopSDK.h"


#include "MCPrefs.h"
#include "MCEditText.h"

#include "Composer_UI.h"
#include "Quick_Colors_Handler.h" // for only one define


// Simple color data
enum 
{	
	BLACK = 0, 
	WHITE = 0x00FF,
	DARK_GREY = WHITE / 4 + 1, 
	MEDIUM_GREY = DARK_GREY * 2, 
	LIGHT_GREY = DARK_GREY * 3
};


const DescriptorKeyID	kPrefsKey				= 'MC06';
const short				kComposerClicksLeft		= 1025; // shouldn't appear but you never know

#if PROVERSION
const char*				kComposerPrefsUniqueID	= "4F72FCD0-E952-11D8-A01B-0030657D12DD"; // taken from 1.3 mac version source base and changed for pro
#else
const char*				kComposerPrefsUniqueID	= "4F72FCD0-E952-11D8-A01B-0030657D12DA"; // taken from 1.3 mac version source base
#endif

MCPrefs::MCPrefs( ComposerUI& uiRef ) :
	mRegistryProcs( kPSDescriptorRegistrySuite, kPSDescriptorRegistrySuiteVersion ),
	mDescriptorProcs( kPSActionDescriptorSuite, kPSActionDescriptorSuiteVersion ),
	mUIRef( uiRef )
{
	::memset( &mStateData, 0, sizeof( mStateData ) );
}


void MCPrefs::RestoreState( bool loadFromDisk )
{
	// we are told to read the prefs from disk, otherwise it's assumed that
	// the mStateData already contains valid data
	if( loadFromDisk )
	{
		// if we can't read the prefs, they might not exist so default the data...
		// note:  we will come back into this function (recurse) but we will not
		// hit this bit of code...
		if( !ReadPrefsData( &mStateData ) )
			DefaultState();
	}
	
	// NOTE: we ASSUME that the state data is valid at this point...and we act on it...
	
	// do all the checkboxes
	mUIRef.SetMunsellCheckboxState( mStateData.munsellCheck == 1 );
	mUIRef.SetWebCheckboxState( mStateData.webColorCheck == 1 );

	// select the color plane
	mUIRef.SetSelectedColorPlane( mStateData.selectedRadio );

	// deal with the disclosure triangles...
	mUIRef.SetToggleState( kSeparator1, mStateData.toggleOne == 1 );
	mUIRef.SetToggleState( kSeparator2, mStateData.toggleTwo == 1 );
	
	// set the palette source colors...
	MCColor unpackedP1 = UnpackColor( mStateData.paletteColors[kPaletteEntries_Color1] );
	MCColor unpackedP2 = UnpackColor( mStateData.paletteColors[kPaletteEntries_Color2] );
	
	// gcc doesn't know how to pick the right function here so we must use a local explicitely. (or use -fpermissive)
	mUIRef.SetPaletteColor( unpackedP1, 0 );
	mUIRef.SetPaletteColor( unpackedP2, 1 );

#if PROVERSION
	MCColor selection = UnpackColor( mStateData.paletteColors[kPaletteEntries_Selection] );
	MCColor split1    = UnpackColor( mStateData.paletteColors[kPaletteEntries_Split1] );
	MCColor split2    = UnpackColor( mStateData.paletteColors[kPaletteEntries_Split2] );
	
	mUIRef.SetPaletteSelection( selection );
	mUIRef.SetSelectedRange( mStateData.selectedPaletteRadio );
	
	mUIRef.SetSplitColor( split1, 0 );
	mUIRef.SetSplitColor( split2, 1 );
	mUIRef.SetSelectedSplit( mStateData.selectedSplit );

	mUIRef.SetAccuracy( mStateData.accuracy );
	mUIRef.SetProportion( mStateData.proportion );
	mUIRef.SetDepth( (short)mStateData.depth );
	mUIRef.SetRange( (short)mStateData.range );
	mUIRef.SetSortOrder( (ASInt8)mStateData.sortOrder );
#endif

	// was there was a palette actually generated...
	// if so, generate the last palette used
	if( mStateData.paletteCreated )
		mUIRef.CreatePalette( false );	// don't reset the colors...

	// now populate the quick colors UI
	PopulateQuickColors();
	
	// set the quick color selection
	mUIRef.SetQuickColorSelection( mStateData.selectedQuick );
	
	mUIRef.SetClicksLeftUI( mStateData.clicksLeft );

	if( mStateData.clicksLeft == 0 )
		mUIRef.DisableOkayButton();
}



void MCPrefs::SaveState( bool decrementClicksLeft )
{
	// do all the checkboxes
	mStateData.munsellCheck  = mUIRef.GetMunsellCheckboxState();
	mStateData.webColorCheck = mUIRef.GetWebCheckboxState();
	
	// select the color plane
	mStateData.selectedRadio = mUIRef.GetSelectedColorPlane();
	
	// did the user generate a palette?
	mStateData.paletteCreated = mUIRef.PaletteExists();

	// deal with the disclosure triangles...
	mStateData.toggleOne = mUIRef.GetToggleState( kSeparator1 );
	mStateData.toggleTwo = mUIRef.GetToggleState( kSeparator2 );

	// get the palette split swatch colors...
	for( short i = 0; i < 2; i++ )
	{
		MCColor* pColor = mUIRef.GetPaletteColor( i );
		if( pColor )
			mStateData.paletteColors[i] = pColor->GetPackedColor();
	}

#if PROVERSION
	// get main split watch colors...
	for( short i = kPaletteEntries_Split1; i <= kPaletteEntries_Split2; i++ )
	{
		MCColor* pColor = mUIRef.GetSplitColor( i - kPaletteEntries_Split1 );
		if( pColor )
			mStateData.paletteColors[i] = pColor->GetPackedColor();
	}

	// save the selected patch
	mStateData.selectedSplit = mUIRef.GetSelectedSplit();

	// get the last selected palette color...
	MCColor* selectedColor = mUIRef.GetPaletteSelection();
	if( selectedColor )
		mStateData.paletteColors[2] = selectedColor->GetPackedColor();
#endif

	// pack all those quick colors
	StoreQuickColors();
	
	// what quick color is selected?
	mStateData.selectedQuick = mUIRef.GetQuickColorSelection();

	// if we are told to deal with clicks left, do it here...
	if( decrementClicksLeft )
	{
		--mStateData.clicksLeft;
		if( mStateData.clicksLeft < 0 )
			mStateData.clicksLeft = 0;
	}

#if PROVERSION	
	// NOTE: we want to be careful here and not change any data that this version doesn't own.
	// the standard version is careful not to mess with values that only apply to the Pro version.

	// Pro version stuff...
	mStateData.accuracy   = mUIRef.GetAccuracy();
	mStateData.proportion = mUIRef.GetProportion();
	mStateData.depth      = mUIRef.GetDepth();
	mStateData.range      = mUIRef.GetRange();
	mStateData.sortOrder  = mUIRef.GetSortOrder();

	mStateData.selectedPaletteRadio = mUIRef.GetSelectedRange();
#endif

	// store that stuff to disk
	WritePrefsData( &mStateData );
}




// Here we just fill in what we think our defaults should be and then call RestoreState...
void MCPrefs::DefaultState()
{
	mStateData.blockVersion = kComposerPrefsCurrentVersion;

	// note: all these colors are being described in RGB
	mStateData.quickColors[0]  = MakePackedGrey( BLACK );
	mStateData.quickColors[1]  = MakePackedGrey( DARK_GREY );
	mStateData.quickColors[2]  = MakePackedGrey( MEDIUM_GREY );
	mStateData.quickColors[3]  = MakePackedGrey( LIGHT_GREY );
	mStateData.quickColors[4]  = MakePackedGrey( WHITE );
	
	mStateData.quickColors[5]  = MCColor::MakePackedColor( WHITE, BLACK, BLACK, 0 );
	mStateData.quickColors[6]  = MCColor::MakePackedColor( BLACK, WHITE, BLACK, 0 );
	mStateData.quickColors[7]  = MCColor::MakePackedColor( BLACK, BLACK, WHITE, 0 );
	mStateData.quickColors[8]  = MCColor::MakePackedColor( BLACK, WHITE, WHITE, 0 );
	mStateData.quickColors[9]  = MCColor::MakePackedColor( WHITE, BLACK, WHITE, 0 );
	mStateData.quickColors[10] = MCColor::MakePackedColor( WHITE, WHITE, BLACK, 0 );
	mStateData.quickColors[11] = MCColor::MakePackedColor( DARK_GREY, LIGHT_GREY, LIGHT_GREY, 0 );

	// set these to "black"
	for( int i = 0; i < kPaletteEntries_Count; i++ )
		mStateData.paletteColors[i] = MCColor::MakePackedColor( 0,0,0,0 );

	// now default the controls...
	mStateData.munsellCheck  = false;
	mStateData.webColorCheck = false;
	mStateData.selectedRadio = HSB_HUE_RADIO;
	mStateData.selectedQuick = 0;
	mStateData.selectedPaletteRadio = 0;
	mStateData.toggleOne = true;				// this is the first disclosure toggle state - turn it downwards

	// demo mode stuff
	mStateData.clicksLeft = kComposerClicksLeft;

	mStateData.accuracy   = kPaletteDefault_Accuracy;
	mStateData.proportion = kPaletteDefault_Proportion;
	mStateData.depth      = kPaletteDefault_Depth;
	mStateData.range      = kPaletteDefault_Range;
	mStateData.sortOrder  = 0;

	// now update the UI... however don't read from the prefs file (chances are there isn't one)
	RestoreState( false );
}
		

void MCPrefs::ResetClicksLeft()
{
	mStateData.clicksLeft = kComposerClicksLeft;
	mUIRef.SetClicksLeftUI( mStateData.clicksLeft );
}


MCPackedColor MCPrefs::MakePackedGrey( unsigned char component )
{
	MCPackedColor packedColor = { component, component, component, 0, kRGBSpace };
	
	return packedColor;
}


MCColor MCPrefs::UnpackColor( MCPackedColor packedColor )
{
	short components[4] = {0};
	
	// unpack the components
	components[0] = packedColor.components[0];
	components[1] = packedColor.components[1];
	components[2] = packedColor.components[2];
	components[3] = packedColor.components[3];
		
	return MCColor( MCSpace( packedColor.colorSpace ), components );
}

const MCColor& MCPrefs::UnpackColor( MCPackedColor packedColor ) const
{
	short components[4] = {0};
	
	// unpack the components
	components[0] = packedColor.components[0];
	components[1] = packedColor.components[1];
	components[2] = packedColor.components[2];
	components[3] = packedColor.components[3];
		
	return MCColor( MCSpace( packedColor.colorSpace ), components );
}


void MCPrefs::PopulateQuickColors()
{
	// just loop through all the colors
	for( short i = 0; i < kNumQuickColors; i++ )
	{
		MCColor unpacked = UnpackColor( mStateData.quickColors[i] );
		mUIRef.SetQuickColor( unpacked, i );
	}
}


void MCPrefs::StoreQuickColors()
{
	// get the quick color and pack it...
	for( short i = 0; i < kNumQuickColors; i++ )
	{	
		MCColor* qColor = mUIRef.GetQuickColor( i );

		// shove into our param block
		if( qColor )
			mStateData.quickColors[i] = qColor->GetPackedColor();
	}	
}


bool MCPrefs::ReadPrefsData( PSPaletteBlock* dataBlock )
{
	if( !dataBlock )
		return false;

	// get our data from the "host's registry"
	PIActionDescriptor prefs = NULL;
	OSErr err = mRegistryProcs->Get( kComposerPrefsUniqueID, &prefs );
	if( err || !prefs )
		return false;
	
	// check the data length.  this code should be written to only read
	// as much as we know about to ensure forward compatibility but I'm too
	// far behind schedule to deal with that...
	int32 length = 0;
	mDescriptorProcs->GetDataLength( prefs, kPrefsKey, &length );
	
	// this if statement will break forward compatibility  !!@
	if( length == sizeof( *dataBlock ) )
	{
		// extract our data from the descriptor	
		err = mDescriptorProcs->GetData( prefs, kPrefsKey, dataBlock );
	}
	else
		err = readErr;
	
	// dispose the descriptor...
	mDescriptorProcs->Free( prefs );
	return err == noErr;
}


bool MCPrefs::WritePrefsData( PSPaletteBlock* dataBlock )
{
	if( !dataBlock )
		return false;
		
	// create a descriptor...
	PIActionDescriptor prefs = NULL;
	OSErr err = mDescriptorProcs->Make( &prefs );
	if( err || !prefs )
		return false;
		
	// shove our data into the descriptor
	err = mDescriptorProcs->PutData( prefs, kPrefsKey, sizeof( *dataBlock ), dataBlock );
	if( !err )
	{
		// shove that data into the "host's prefs" - true means the data is persistent
		err = mRegistryProcs->Register( kComposerPrefsUniqueID, prefs, true );
	}
	
	// dispose the descriptor...
	mDescriptorProcs->Free( prefs );
	
	return err == noErr;
}


// EOF
