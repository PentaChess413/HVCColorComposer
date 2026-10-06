/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Palette_Display_Base_h
#define Palette_Display_Base_h

#include "Custom_Draw_Control.h"
#include "MCDefs.h"

#include <vector>

class	MCLib_Wrapper;

typedef std::vector<HVCTrip> HVCArray;

enum 
{	
	PALETTE_COLUMNS = 52, 
	PALETTE_INSET = 1, 
	PALETTE_SIZE = 12 
};


class Palette_Display_Base : public Custom_Draw_Control
{
	typedef Custom_Draw_Control	base;

	public:
		const HVCArray&		GetPalette() const { return mPalette; }
		ASInt32				GetSize() const	{ return mPalette.size(); }

		// save a palette to disk? -- puts up the save dialog and everything.
		void				SavePalette();
			
		// utility routines...
		static double	CalculateDistance( const HVCTrip& first_color, const HVCTrip& second_color );
		static double	CalculateModifier( double chroma_1, double chroma_2 );
		static bool		CheckColorRange( const HVCTrip& i, double range, double accuracy, const HVCTrip& limit_1, const HVCTrip& limit_2 );
		static bool		CheckColorProportion( const HVCTrip& i, double proportion, double accuracy, const HVCTrip& limit_1, const HVCTrip& limit_2 );
		

	protected:
		Palette_Display_Base( Dialog_Runner& dialog_runner, const ASInt32 control_id, MCLib_Wrapper& p_mclib ) :
			base( dialog_runner, control_id ), 
			mMCLib( p_mclib )
		{
			mSelectedSwatch.top = mSelectedSwatch.left = mSelectedSwatch.right = mSelectedSwatch.bottom = 0;
		}
		
		~Palette_Display_Base()	{}
		
		
		// required overrides
		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		Draw( ADMDrawerRef environment );
		
		// select a color from the palette based on the index (and the current scroll position)
		virtual bool		SelectPaletteColor( ASInt32 hitIndex );

		HVCArray&			GetPalette()	{ return mPalette; }
		MCLib_Wrapper&		GetMCLibRef()	{ return mMCLib; }
		
		// write a palette to disk...
		ASErr				WritePalette( ConstSpCharPtr paletteFilePath );

	private:
		HVCArray		mPalette;
		MCLib_Wrapper&	mMCLib;
		ASRect			mSelectedSwatch;	 // used during tracking...
};


// Maximum values
#define RAW_MAX_D		static_cast<double> (RAW_MAX)
#define MAX_CHROMA		22.0
#define MAX_VALUE		86.0
#define HALF_MAX_HUE	RAW_MAX_D / 2.0
#define MAX_DIST		10.0

#endif

// EOF
