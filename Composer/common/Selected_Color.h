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

#ifndef Selected_Color_h
#define Selected_Color_h

#include "MCColor.h"
#include "PIPicker.h"


class SelectedColor : public MCColor
{
	public:
		// Constructors:
		SelectedColor( PIPickerParams& );
		
		// sets the color back to the original color that is stored in the host color record
		void				ResetColor();

		MCColor				GetOriginalColor();
		void				GetOriginalColor( short desiredColorSpace, short destination_data[4] );
		ASRGBColor			GetOriginalColorForDisplay();
		
		// misc. utilities
		SPPluginRef			GetPluginRef() const { return (SPPluginRef)mHostRecord.plugInRef; }
		DisplayPixelsProc	GetDisplayProc() const { return mHostRecord.displayPixels; }

	private:
		PIPickerParams&		mHostRecord;
};
typedef SelectedColor& SelectedColorRef;

#endif

// EOF
