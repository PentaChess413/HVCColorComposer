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

#ifndef _H_MCPrefs
#define _H_MCPrefs

#include "PIActions.h"
#include "MCColor.h"


const unsigned long kComposerPrefsCurrentVersion	= 2;

enum
{
	kPaletteEntries_Color1 = 0,
	kPaletteEntries_Color2,
	kPaletteEntries_Selection,
	kPaletteEntries_Split1,
	kPaletteEntries_Split2,
	
	// please leave last
	kPaletteEntries_Count
};

// NOTE: !!@ this needs packing pragmas !!@

// Struct to hold the parameters to store in the Photoshop preferences
typedef struct
{
	unsigned long	blockVersion;	// used for forward/backward compatibility
	
	unsigned char	munsellCheck;	// is the munsell checkbox checked?
	unsigned char	webColorCheck;	// web color checked?
	unsigned char	toggleOne;		// this is the first disclosure toggle state
	unsigned char	toggleTwo;		// this is the second disclosure toggle state
		
	// color selector
	short			selectedQuick;	// this is an index of the selected quick color
	short			selectedRadio;	// which color plane is selected?
	short			clicksLeft;		// demo version clicks left
	short			reserved0;		// unused in this version...
	
	// palette controls -- these are exposed in the Pro version of the Plugin
	float			accuracy;		
	float			proportion;		
	long			range;			
	long			depth;			
	short			sortOrder;		
	short			selectedPaletteRadio;
	unsigned char	paletteCreated;	
	unsigned char	selectedSplit;	// 0 or 1
	short			reserved2;		// unused
	
	// this block contains all the colors (they must be properly packed)
	// these are the palette split swatch colors and selection
	MCPackedColor	paletteColors[kPaletteEntries_Count];	
	MCPackedColor	quickColors[12];	// this is the array of packed quick colors
} PSPaletteBlock;




class ComposerUI;

// Class to store the persistent state in the Photoshop preferences
class MCPrefs
{
	public:
		// Constructors:
		MCPrefs( ComposerUI& uiRef );
		virtual ~MCPrefs() {}

		// Load state data from the preferences
		void	RestoreState( bool loadFromDisk = true );

		// Save the dialog state to prefs -- when user presses ok (in demo mode) you should set decrement to true
		void	SaveState( bool decrementClicksLeft );
		
		// if there are no prefs data, this gets called instead.
		// it's not private as you can call it any time to default the preference data
		// and set the dialog to its initial state
		void	DefaultState();
		
		void	ResetClicksLeft();

	protected:
		MCPackedColor	MakePackedGrey( unsigned char component );
		
		// this unpacks a color that was packd with MakePackedColor
		MCColor			UnpackColor( MCPackedColor packedColor );
		const MCColor& 	UnpackColor( MCPackedColor packedColor ) const;

		void			PopulateQuickColors();
		void			StoreQuickColors();
		
		// these read and write the prefs data
		bool			ReadPrefsData( PSPaletteBlock* );
		bool			WritePrefsData( PSPaletteBlock* );
		
		
	private:
		AutoSuite<PSDescriptorRegistryProcs>	mRegistryProcs;
		AutoSuite<PSActionDescriptorProcs>		mDescriptorProcs;
		PSPaletteBlock							mStateData;
		ComposerUI&								mUIRef;
};


#endif	// !_H_MCPrefs

// EOF

