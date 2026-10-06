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

#ifndef _H_MCRangeSelector
#define _H_MCRangeSelector

#include "Composer_Dialog_Indexes.h"

#include "Radio_Button_Group.h"
#include "Helpers.h"

#include "Palette_Display_Handler.h"
#include "Split_Color_Display_Handler.h"
#include "MCColorSwatch.h"
#include "MCDualColorSwatch.h"

class MCEditText;
class Control_Runner;
class ComposerUI;
class MCIcon;
class MCSortFrame;

typedef enum
{
	kMCRange_Range,
	kMCRange_Auto,
	kMCRange_Proportional
} MCRangeSelection;


typedef struct
{
	short range;
	short depth;
	float accuracy;
	float proportion;
} MCPaletteParams;


const int kNumSortIcons = 6;
const int kNumEditTexts = 4;


class MCRangeSelector : public Radio_Button_Group
{
	typedef Radio_Button_Group	base;

	public:
		MCRangeSelector( ComposerUI& p_dialog_runner, ASInt32 first_button, ASInt32 last_button );
		virtual ~MCRangeSelector();
		
		// please call this to get the initial UI state correct...
		void			Initialize( ASInt32 new_button );

		// this is the only ways to set a new color in the displays...
		void			SelectNewColor( MCColorRef, SelectionFlags flags = kUpdateUnspecifiedFlag );
		
		// create a palette using the specified colors...
		void			CreatePalette( Palette_Display_Handler*, bool setColors = true );
		
		// call this to manually update the contrast...
		void			UpdatePaletteContrast( SelectionFlags flags = kUpdateUnspecifiedFlag );
		
		// call this after showing the controls to make sure they should the right ones.
		void			UpdateControls();
		
		// called when the web safe checkbox is clicked...
		void			UpdateWebSafe( bool safe );
		
		// this returns true if it successfully switched radio buttons -- required
		virtual bool	ButtonChanging( ASInt32 new_button );	

		void			SetPaletteColor( MCColorRef colorRef, short index );
		MCColor*		GetPaletteColor( short index );
		void			SetPaletteSelection( MCColorRef colorRef );
		MCColor*		GetPaletteSelection();
		short			GetSelectedRange();
		void			SetSelectedRange( short );
		unsigned char	GetSelectedSplit();
		void			SetSelectedSplit( unsigned char );
		void			SetSplitColor( MCColorRef, short index );
		MCColor*		GetSplitColor( short index );

		void			SetPaletteSortOrder( ASInt8	newOrder );
		ASInt8			GetPaletteSortOrder();
		
		// palette parameters
		short			GetRange();
		void			SetRange( short );
		short			GetDepth();
		void			SetDepth( short );
		float			GetAccuracy();
		void			SetAccuracy( float );
		float			GetProportion();
		void			SetProportion( float );


	protected:
		void			UpdateSplitContrast( short text_item, const HVCTrip& first_color, const HVCTrip& second_color );
		void			ShowHideSplitSwatches( bool show );
		void			UpdatePaletteResults();

		static void		TextCallback( const MCEditText& target, void* userData );
		
	private:
		ComposerUI&					 mUIRef;
		MCRangeSelection			 mRadio;
		MCRangeSelection			 mPaletteType;
		Palette_Display_Handler*	 mPaletteRef;	// just a reference do not dispose

		ASInt8						 mCurrentSortOrder;

		MCDualColorSwatch*			 mMainSplit;
		
		MCColorSwatch*				 mSwatch1;
		MCColorSwatch*	 			 mSwatch2;
		Split_Color_Display_Handler* mSplit1;
		Split_Color_Display_Handler* mSplit2;
		
		MCSortFrame*				 mSortFrame;
		
		// these are "live" colors, they get updated when you select a color
		MCColor						 mColor1;			
		MCColor						 mColor2;

		MCColor						 mSplitColor1;			
		MCColor						 mSplitColor2;
		
		// these are the colors used to generate the palette 
		// and are visible from the split swatches in the results area
		MCColor						 mPaletteColor1;
		MCColor						 mPaletteColor2;
		MCColor						 mPaletteSelection;
		
		// used by the palette code
		HVCTrip						 mHVC1;
		HVCTrip						 mHVC2;

		MCIcon*						 mSortIcons[kNumSortIcons];
		MCEditText*					 mEditTexts[kNumEditTexts];
		
		bool						 mPaletteSelectionMade;
		
		MCPaletteParams				 mLiveParams;
		MCPaletteParams				 mPaletteParams;
};

#endif	// !_H_MCRangeSelector
// EOF
