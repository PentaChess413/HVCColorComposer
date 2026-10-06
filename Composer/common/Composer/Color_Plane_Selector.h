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

#ifndef Color_Plane_Selector_h
#define Color_Plane_Selector_h

#include "Composer_Dialog_Indexes.h"

#include "Radio_Button_Group.h"
#include "Helpers.h"

#include "Selected_Color.h"
#include "Color_Selector.h"


const int kNumControls = 27;

class G_World;
class MCEditText;
class Raw_Text_Display;
class Control_Runner;
class ComposerUI;
class Quick_Colors_Handler;
class Selected_Color_Display;

// callback prototypes to member functions... I don't like these so much because of the syntax required to use them.
class ColorPlaneSelector;
typedef void (ColorPlaneSelector::*ControlRunnerCallback)( const Control_Runner& target );


class ColorPlaneSelector : public Radio_Button_Group
{
	typedef Radio_Button_Group	base;

	public:
		ColorPlaneSelector( ComposerUI& p_dialog_runner, SelectedColorRef colorRef, const ASInt32 first_button, const ASInt32 last_button );
		virtual ~ColorPlaneSelector();
		
		void		Initialize( ASInt32 new_button );
		
		// this is the only ways to set a new color in the displays...
		void		SelectNewColor( MCColorRef, SelectionFlags flags = kUpdateUnspecifiedFlag );

		// this is called by tracking callbacks...
		void		UpdateXYDisplay( ASInt32 new_z );
		void		UpdateZDisplay( ASInt32 new_x, ASInt32 new_y );
	
		// these are for state data and do not cause color updates!!!
		void		SetQuickColor( MCColorRef, short index );
		MCColor*	GetQuickColor( short index );

		void		SetQuickColorSelection( short index );
		short		GetQuickColorSelection();

		bool		GetWebCheckboxState(); 
		void		SetWebCheckboxState( bool );
		 
		bool		GetMunsellCheckboxState();
		void		SetMunsellCheckboxState( bool );

		short		GetSelectedColorPlane();
		void		SetSelectedColorPlane( short );

	protected:
		friend class Raw_Text_Display;	// I don't want classes calling the color selector directly unless absolutely necessary!!@

		void		HandleColorChange( SkipBlock skip_block, SelectionFlags flags = kUpdateUnspecifiedFlag );

		// this updates all the spectrum displays...
		void		UpdateSpectrumDisplays();
		
		// this calculates all the warning colors and is the last step in updating the selected color, etc...
		void		CalculateWarnings();
		bool		CalculatePrintSafeColor();
		bool		CalculateClosestHVCColor( bool& not_safe );
		bool		CalculateWebSafeColor( bool& notSafe );

		// update a single warning icon
		void		UpdateWarning( const ASInt32 picture_item, const bool not_safe, const bool color_changed, bool& safe_color );


		// callbacks...
		static void	HSBCallback( const MCEditText& target, void* userData );
		static void	RGBCallback( const MCEditText& target, void* userData );
		static void	LabCallback( const MCEditText& target, void* userData );
		static void	CMYKCallback( const MCEditText& target, void* userData );
		static void	HVCCallback( const MCEditText& target, void* userData );
		
		void		RawCallback( const Raw_Text_Display& target );
		void		WebCallback( const Control_Runner& target );
		void		MunsellCallback( const Control_Runner& target );
		
		// these all just ask the selected color for the color in the requested color space...
		void		SetRaw();
		void		SetHVC();
		void		SetRGB();
		void		SetHSB();
		void		SetLab();
		void		SetCMYK();

		// this returns true if it successfully switched the color plane -- required
		virtual bool ButtonChanging( ASInt32 new_button );	
		
		// use this one when text is changed..
		Color_Selector::Update_Check			CheckForUpdate(); 
	 
		// arg! how hard is it to dispose something?  one line of code?
		typedef std::auto_ptr<Color_Selector>	Auto_Selector;



	private:
		ComposerUI&				mUIRef;
		MCLib_Wrapper&			mMCLib;
		Auto_Selector			mCurrentSelector;
		
		// these are all 4 shorts to deal with copy routines that expect 4 components		
		short					m_rgb[4];
		short					m_hsb[4];
		short					m_lab[4];
		short					m_hvc[4];
		short					m_cmyk[4];

		// these are used to stop the spectrum display from jumping around...
		short					m_rgb_spectrum[4];		
		short					m_hsb_spectrum[4];
		short					m_lab_spectrum[4];
		short					m_hvc_spectrum[4];
		
		// some state data
		bool					mIsHVCSafe;
		bool					mIsPrintSafe;
		bool					mIsWebSafe;
		MCColor					mPrintSafeColor;
		MCColor					mWebSafeColor;
		MCColor					mHVCSafeColor;
		
		// colors for display
		SelectedColorRef		mSelectedColorRef;		// this is the final result color, etc...
		Quick_Colors_Handler*	mQuickColors;
		Selected_Color_Display* mSelected;
		
		// stuff I'm still working on...
		G_World*				m_xy_buffer;
		G_World*				m_z_buffer;

		Control_Runner*			mControls[kNumControls];
};

#endif

// EOF
