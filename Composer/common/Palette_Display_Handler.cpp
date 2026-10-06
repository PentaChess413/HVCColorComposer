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

#include "PhotoShopSDK.h"

#include "Palette_Display_Handler.h"
#include "Generate_Palette_Base.h"
#include "Palette_Display_Base.h"

#include "Composer_UI.h"


enum
{	
	kSORT_HVC = 0, 
	kSORT_HCV, 
	kSORT_CHV, 
	kSORT_CVH, 
	kSORT_VCH, 
	kSORT_VHC 
};




// Class to implement palette generation
class Palette_Display_Handler::Generate_Palette : public Generate_Palette_Base
{
	typedef Generate_Palette_Base	base;

	public:
		// Constructor:
		Generate_Palette( int32 p_step_count, const HVCTrip& p_limit, Check_Proc p_check_proc ) :
			base( p_step_count, 1.0, p_limit, p_limit, p_check_proc ),
			mRange( 0.0 )
		{}
		
		Generate_Palette( int32 p_step_count, double p_range, double p_accuracy, const HVCTrip& p_limit_1, const HVCTrip& p_limit_2, Check_Proc p_check_proc ) :
			base( p_step_count, p_accuracy, p_limit_1, p_limit_2, p_check_proc ),
			mRange( p_range )
		{}

		// Generate callback
		HVCArray Generate( MCLib_Wrapper& p_mclib ) const
		{
			HVCArray result;

			for( double range = 5; range <= 40; range *= 2 )
				generate_hvc( result, range, p_mclib );

			return result;
		}
		
		HVCArray Generate( MCLib_Wrapper& p_mclib, int sort_order ) const;
		
	private:
		double	mRange;
};





// Constructor:
Palette_Display_Handler::Palette_Display_Handler( Dialog_Runner& dialog_runner, const ASInt32 control_id, MCLib_Wrapper& p_mclib ) :
	base( dialog_runner, control_id, p_mclib ),
	mNewPalette( false )
{
}


Palette_Display_Handler::~Palette_Display_Handler()
{
}


// Generate a new palette
void Palette_Display_Handler::generate_palette( int32 p_step_count, const HVCTrip& p_limit )
{
	GP_ptr generator( new Generate_Palette( p_step_count, p_limit, &Palette_Display_Base::CheckColorRange ) );

	m_generator = generator;
	generate_palette();
}

void Palette_Display_Handler::generate_palette( int32 p_step_count, double p_range, double p_accuracy, const HVCTrip& p_limit_1, const HVCTrip& p_limit_2, bool p_show_range )
{
	GP_ptr generator( new Generate_Palette( p_step_count, p_range, p_accuracy, p_limit_1, p_limit_2, p_show_range ? Palette_Display_Base::CheckColorRange : Palette_Display_Base::CheckColorProportion) );

	m_generator = generator;
	generate_palette();
}

// Regenerate the palette
void Palette_Display_Handler::generate_palette()
{
#if PROVERSION
	ComposerUI* ui = GetComposerUI();
	if( !ui )
		return;
		
	GetPalette() = m_generator->Generate( GetMCLibRef(), ui->GetSortOrder() );
#else
	GetPalette() = m_generator->Generate( GetMCLibRef() );
#endif

	mNewPalette = true;
}

// Generate the palette using the auto-palette algorithm
HVCArray Palette_Display_Handler::Generate_Palette::Generate( MCLib_Wrapper& p_mclib, int sort_order ) const
{
	typedef void (Generate_Palette_Base::*Generator)( HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib ) const;

	HVCArray	result;
	Generator	generator = NULL;

	// Create a new palette generator
	switch( sort_order )
	{
		case kSORT_HVC:
			generator = (&Palette_Display_Handler::Generate_Palette::generate_hvc);
			break;

		case kSORT_HCV:
			generator = (&Palette_Display_Handler::Generate_Palette::generate_hcv);
			break;

		case kSORT_CHV:
			generator = (&Palette_Display_Handler::Generate_Palette::generate_chv);
			break;

		case kSORT_CVH:
			generator = (&Palette_Display_Handler::Generate_Palette::generate_cvh);
			break;

		case kSORT_VCH:
			generator = (&Palette_Display_Handler::Generate_Palette::generate_vch);
			break;

		case kSORT_VHC:
			generator = (&Palette_Display_Handler::Generate_Palette::generate_vhc);
			break;

		default:
			return result;
	}

	// this member function pointer stuff is stupid.  In the above switch statement we could have just called the
	// specific generate function without all this crap here...
	
	if( mRange >= 0 )
	{
		// Generate the palette
		(this->*generator)( result, mRange, p_mclib );		
	}
	else
	{
		// Generate colors with ranges of 5, 10, 20, and 40
		for( double range = 5; range <= 40; range *= 2 )
			(this->*generator)( result, range, p_mclib );
	}
	
	return result;
}



void Palette_Display_Handler::Draw( ADMDrawerRef environment )
{
	// check to see if we should do a full clear first.  We do this only when a new palette was
	// created because if the palette is smaller than what was already shown, lines will be left
	// over from the drawing (erasing) code...
	if(	mNewPalette )
	{
		// clear the entire rectangle
		ADM_Access::drawing_suite()->Clear( environment );

		// reset it
		mNewPalette = false;
	}
	
	// draw the palette
	base::Draw( environment );
}

// EOF
