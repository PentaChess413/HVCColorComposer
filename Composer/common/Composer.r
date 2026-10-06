/*

 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */
#include "Scripting.h"
#include "PIDefines.h"
#ifdef __PIMac__
	#include "Types.r"
	#include "SysTypes.r"
	#include "PIGeneral.r"
	#include "PIUtilities.r"
//	#include "DialogUtilities.r" -- gives us duplicate DLOG/DITL warnings
#elif defined(__PIWin__)
	#include "PIGeneral.h"
	#include "PIUtilities.r"
	#include "WinDialogUtils.r"
#endif

#include "PIActions.h"

//-------------------------------------------------------------------------------

resource 'PiPL' ( 16000 /*ResourceID*/, plugInName " PiPL", purgeable)
{
    {
	    Kind { Picker },
	    Name { plugInName },
	    Category { vendorName },
	    Version { (latestPickerVersion << 16) | latestPickerSubVersion },
	    RequiredHost { '8BIM' },
	    
		#ifdef __PIMac__
	        CodeCarbonPowerPC { 0, 0, "" },
		#elif defined (__PIWin__)
			CodeWin32X86 { "PluginMain" },
		#endif

		PickerID { vendorName " " plugInName },
		
		SupportedModes
		{
			doesSupportBitmap, doesSupportGrayScale,
			doesSupportIndexedColor, doesSupportRGBColor,
			doesSupportCMYKColor, doesSupportHSLColor,
			doesSupportHSBColor, doesSupportMultichannel,
			doesSupportDuotone, doesSupportLABColor
		},
				
		EnableInfo { "true" },
	}
};

//-------------------------------------------------------------------------------
//	URL strings
//-------------------------------------------------------------------------------

resource StringResource (16100, "", purgeable)
{
	"http://www.master-colors.com/support/composer/"
};

resource StringResource (16101, "", purgeable)
{
	"http://www.master-colors.com"
};

resource StringResource (16102, "", purgeable)
{
	"http://www.master-colors.com/store"
};

resource StringResource (16200, "", purgeable)
{
	"Between:"
};

resource StringResource (16201, "", purgeable)
{
	"From:"
};

resource StringResource (16202, "", purgeable)
{
	"Contrast Range:"
};

resource StringResource (16203, "", purgeable)
{
	"Proportional:"
};

resource StringResource (16204, "", purgeable)
{
	"Range:"
};

resource StringResource (16205, "", purgeable)
{
	"Proportion:"
};

resource StringResource (16206, "", purgeable)
{
	"Auto"
};

resource StringResource (16300, "", purgeable)
{
	"HVC"
};

resource StringResource (16301, "", purgeable)
{
	"HCV"
};

resource StringResource (16302, "", purgeable)
{
	"CHV"
};

resource StringResource (16303, "", purgeable)
{
	"CVH"
};

resource StringResource (16304, "", purgeable)
{
	"VCH"
};

resource StringResource (16305, "", purgeable)
{
	"VHC"
};

resource StringResource (16400, "", purgeable)
{
	"Palette sort order: %s"
};


// EOF
