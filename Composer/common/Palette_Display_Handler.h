/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Palette_Display_Handler_h
#define Palette_Display_Handler_h

#include "Palette_Display_Base.h"

enum {	PALETTE_ROWS = 15 };

class	Palette_Display_Handler : public Palette_Display_Base
{
	typedef Palette_Display_Base	base;

	public:
		Palette_Display_Handler( Dialog_Runner& dialog_runner, const ASInt32 control_id, MCLib_Wrapper& p_mclib );
		virtual ~Palette_Display_Handler();

		void	generate_palette();
		void	generate_palette( int32 p_step_count, const HVCTrip& p_limit );
		void	generate_palette( int32 p_step_count, double p_range, double p_accuracy, const HVCTrip& p_limit_1, const HVCTrip& p_limit_2, bool p_show_range );

		virtual void Draw( ADMDrawerRef environment );
		
	private:
		class	Generate_Palette;

		typedef std::auto_ptr<Generate_Palette>	GP_ptr;

		GP_ptr	m_generator;
		bool	mNewPalette;	// true when the palette is new (set to false after first draw)
};


#endif

// EOF

