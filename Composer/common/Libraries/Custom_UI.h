/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef _H_Custom_UI
#define _H_Custom_UI

#include "Dialog_Runner.h"

#include "MCColorList.h"
#include "MCColorSlider.h"
#include "MCBookPopup.h"
#include "MCColorLibraries.h"

#include "Selected_Color.h"
#include "Selected_Color_Display.h"
#include "Warning_Color_Display.h"
#include "Color_Plane_Selector.h"

#if WIN32
#include "Composer-sym.h"
#endif

#include <vector>


// Class to run the "Custom Libraries" color picker dialog
class CustomUI : public Dialog_Runner
{
	typedef Dialog_Runner base;
	friend class MCBookPopup;
	
	public:
		// Constructors:
		CustomUI( SelectedColorRef p_selected_color, const char* p_name, ASInt32 p_dialog_id );
		virtual	~CustomUI();

		// Dialog control indexes
		enum 
		{	
#if WIN32
			OK     = IDOK, 
			CANCEL = IDCANCEL, 
			PICKER = IDC_PICKER,

			COLOR_DISPLAY = IDC_SELECTION, 
			
			PRINTABLE_WARNING_ICON = IDC_WARN_CMYK_ICON, 
			PRINTABLE_WARNING_COLOR,
			
			WEB_COLOR_WARNING_ICON = IDC_WARN_RGB_ICON, 
			WEB_COLOR_WARNING_COLOR,

			BOOK_MENU    = IDC_LIBRARY_POPUP, 
			COLOR_LIST   = IDC_COLOR_LIST, 
			COLOR_SLIDER = IDC_COLOR_SLIDER,

			COMPONENT_1 = IDC_CMYK_C_STATIC, 
			COMPONENT_2 = IDC_CMYK_M_STATIC, 
			COMPONENT_3 = IDC_CMYK_Y_STATIC,
			COMPONENT_4 = IDC_CMYK_K_STATIC,
			
			COMPONENT_LABEL_1 = IDC_CMYK_C_LABEL, 
			COMPONENT_LABEL_2 = IDC_CMYK_M_LABEL, 
			COMPONENT_LABEL_3 = IDC_CMYK_Y_LABEL, 
			COMPONENT_LABEL_4 = IDC_CMYK_K_LABEL,

			TYPING_LABEL = IDC_SEARCH_LABEL
#else
			OK     = 1, 
			CANCEL,
			PICKER,

			COLOR_DISPLAY,
			
			PRINTABLE_WARNING_ICON,
			PRINTABLE_WARNING_COLOR,
			
			WEB_COLOR_WARNING_ICON, 
			WEB_COLOR_WARNING_COLOR,

			BOOK_MENU, 
			COLOR_LIST, 
			COLOR_SLIDER,

			COMPONENT_1, 
			COMPONENT_2, 
			COMPONENT_3,
			COMPONENT_4,
			
			COMPONENT_LABEL_1, 
			COMPONENT_LABEL_2, 
			COMPONENT_LABEL_3, 
			COMPONENT_LABEL_4,

			TYPING_LABEL
#endif			
		};

		// Artificial max number of color book files that can be in the popup
		enum {	MAX_BOOKS = 100 };
		
		// override to setup the colors and stuff before display...
		virtual ASInt32 run();
		
		// required function
		virtual void SelectNewColor( MCColorRef newColor, SelectionFlags flags = kUpdateUnspecifiedFlag );

		MCColorbook*		GetColorbook();
		MCColorSlider*		GetColorSlider();
		
		Control_Interface	GetControl( ASInt32 item_id )			{ return get_item_interface( item_id ); }
		Control_Interface	get_item_interface( ASInt32 item_id )	{ return get_native_item( item_id ); }

	protected:
		static ASAPI ASBoolean  DialogTrackProc( ADMDialogRef inDialog, ADMTrackerRef inTracker );
		
		// this sets the text in the labels
		void		UpdateLabels();

		// this calculates all the warning colors
		void		CalculateWarnings();
		void		UpdateWarning( ASInt32 pictureID, bool not_safe, bool color_changed, bool& safe_color );
		
		bool		CalculatePrintSafeColor();
		bool		CalculateWebSafeColor( bool& notSafe );

		void		HandleColorChange( SkipBlock skipBlock, SelectionFlags flags );

	private:
		SelectedColorRef			mSelectedColorRef;
		bool						mIsPrintSafe;
		bool						mIsWebSafe;
		MCColor						mPrintColor;
		MCColor						mWebColor;

		Selected_Color_Display*		mSelectedColor;
		Warning_Color_Display*		mPrintWarning;
		Warning_Color_Display*		mWebWarning;

		MCColorList*				mColorList;
		MCColorSlider*				mColorSlider;
		MCBookPopup*				mBookPopup;
		
//		MCColorbook*				mCurrentBook;
		MCColorLibraries*		    mColorLibraries;
};


#endif // !_H_Custom_UI


// EOF
