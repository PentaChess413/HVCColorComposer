/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	Source code rewritten by: Alex Lelievre ([PII Redacted]), 2006.
 *
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Composer_UI_h
#define Composer_UI_h


#include "Dialog_Runner.h"
#include "Control_Interface.h"
#include "Color_Plane_Selector.h"
#include "MCPrefs.h"
#include "MCRangeSelector.h"

#if WIN32
#include "Composer-sym.h"
#endif


class Color_Text_Display;
class Raw_Text_Display;
class HVC_RGB_Color_Pair;
class Quick_Colors_Handler;
class ComposerUI;


// for use with the toggle state methods
typedef enum
{
	kSeparator1 = 0,
	kSeparator2
} ToggleArrow;


typedef void (ComposerUI::*VoidCallback)();


class ComposerUI : public Dialog_Runner
{
	typedef Dialog_Runner base;
	
	public:
		ComposerUI( SelectedColorRef p_selected_color, const char* p_name, ASInt32 p_dialog_id );
		virtual	~ComposerUI();

#if WIN32
		enum 
		{	
			Picker_Ok = 1, 
			Picker_Cancel,
			Picker_CustomLibraries = IDC_CUSTOM_BUTTON
		};
#else
		enum 
		{	
			Picker_Ok = 1, 
			Picker_Cancel,
			Picker_CustomLibraries
		};
#endif
		// a necessary evil-  we override run() so that we can restore the state...
		virtual ASInt32	run();

		// call this right before destroying this object to save its state
		void		SaveState( bool decrementClicksLeft );
		void		ResetClicksLeft();
		void		SetClicksLeftUI( short numClicksLeft );
		
		// these all call thru to the color selector because the controls can't access it directly...
		void		SelectNewColor( MCColorRef newColor, SelectionFlags flags = kUpdateUnspecifiedFlag );
		void		UpdateXYDisplay( ASInt32 new_z );
		void		UpdateZDisplay( ASInt32 new_x, ASInt32 new_y );
		
		
		// State data accessors -- these doesn't do a color update or anything ///////////////////////
		void		SetQuickColor( MCColorRef, short index );
		MCColor*	GetQuickColor( short index );
		void		SetQuickColorSelection( short index );
		short		GetQuickColorSelection();
		
		void		SetPaletteColor( MCColorRef, short index );
		MCColor*	GetPaletteColor( short index );
		
		// make sure you set the palette colors first...
		void		CreatePalette( bool setColors = true );	
		bool		PaletteExists() { return mPaletteExists; }

		void		SetPaletteSelection( MCColorRef );
		MCColor*	GetPaletteSelection();

		bool		GetToggleState( ToggleArrow whichOne );
		void		SetToggleState( ToggleArrow whichOne, bool );
		
		bool		GetWebCheckboxState(); 
		void		SetWebCheckboxState( bool );
		 
		bool		GetMunsellCheckboxState();
		void		SetMunsellCheckboxState( bool );
		
		short		GetSelectedColorPlane();
		void		SetSelectedColorPlane( short );
		
		short		GetSelectedRange();
		void		SetSelectedRange( short );

		unsigned char GetSelectedSplit();
		void		  SetSelectedSplit( unsigned char );
		
		void		SetSplitColor( MCColorRef, short index );
		MCColor*	GetSplitColor( short index );

		void		SetSortOrder( ASInt8 order );
		ASInt8		GetSortOrder();

		// palette parameters
		short		GetRange();
		void		SetRange( short );
		short		GetDepth();
		void		SetDepth( short );
		float		GetAccuracy();
		void		SetAccuracy( float );
		float		GetProportion();
		void		SetProportion( float );


		// this is when for when demo mode had ended...
		void		DisableOkayButton();
		
		// we need this in order to be able to call the licensing functions...
		void		SetLicenseDataRef( void* licenseData );
		
		// called to enable or disable certain controls
		void		CheckLicense();
		
		
		// callbacks! //////////////////////////////////////////////////////////////////////////////////
		virtual void		HandleSeparator1();
		virtual void		HandleSeparator2();
		
		
		// I don't want classes calling the color selector directly unless absolutely necessary!!@
		ColorPlaneSelector*	GetColorSelector() { return mCPSelector; }
		
#if PROVERSION		
		MCRangeSelector*	GetRangeSelector() { return mRangeSelector; }
#endif
	
	protected:
		friend class MCRangeSelector;
		friend class MCEditText;
		
		void				InitializeScrollbar();
		
		// palette display control
		void				ShowHidePalette( bool shide );			// reveal/hide the palette
		void				ShowHidePaletteControls( bool shide );	// reveal/hide the controls for the palette
		
		// Palette routines
		static int			depth() { return 50; }
		void				UpdatePaletteContrast( SelectionFlags flags = kUpdateUnspecifiedFlag );
		void				UpdateSplitSwatch( short swatch_item, const HVCTrip& first_color, const HVCTrip& second_color );

#if PROVERSION
		void				HandlePaletteOrderChange( ASInt32 whichSortIcon );
#endif

		// static routines
		static ASAPI void	ButtonCallback( ADMItemRef p_control, ADMNotifierRef tracker );
		static ASAPI void	ScrollNotifyCB( ADMItemRef p_control, ADMNotifierRef tracker );


	private:
		// private data
		ColorPlaneSelector*	mCPSelector;
		Control_Runner*		mSplit;
		Control_Runner*		mPalette;
		Control_Runner*		mTri1;
		Control_Runner*		mTri2;

		
		SelectedColorRef	mSelectedColorRef;
		MCColor				m_palette_source;
		MCColor				m_palette_selection;
		bool				mPaletteExists;
		bool				mPaletteShowing;
		bool				mPaletteControlsShowing;
		ASInt32				mControlsHeight;
		
		MCPrefs*			mState;
		
		void*				mLicenseData;
#if PROVERSION
		MCRangeSelector*	mRangeSelector;
#endif
};

typedef ComposerUI& ComposerUIRef;

#endif // !Composer_UI_h


// EOF
