/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Generate_Palette_Base_h
#define Generate_Palette_Base_h

#include "MCDefs.h"
#include "Palette_Display_Handler.h"

class MCLib_Wrapper;


// Base class for generating the palette
class Generate_Palette_Base
{
	public:
		// The check_color function calls a function using this prototype
		typedef bool (*Check_Proc)( const HVCTrip& i, double parameter, double accuracy, const HVCTrip& limit_1, const HVCTrip& limit_2 );

	protected:
		// Constructor:
		Generate_Palette_Base( int32 p_step_count, double p_accuracy, const HVCTrip& p_limit_1, const HVCTrip& p_limit_2, Check_Proc p_check_proc ) :
			m_step_count( p_step_count ), 
			m_accuracy( p_accuracy ),
			m_limit_1( p_limit_1 ),
			m_limit_2( p_limit_2 ),
			m_check_proc( p_check_proc )
		{}

		// Generation algorithms:
		void	generate_hvc( HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib ) const;
		void	generate_hcv( HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib ) const;
		void	generate_chv( HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib ) const;
		void	generate_cvh( HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib ) const;
		void	generate_vch( HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib ) const;
		void	generate_vhc( HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib ) const;

	private:
		int32			m_step_count;
		double			m_accuracy;
		const HVCTrip&	m_limit_1;
		const HVCTrip&	m_limit_2;
		Check_Proc		m_check_proc;

		// Add valid colors to the palette
		void	check_color( HVCArray& result, double p_parameter, const HVCTrip& i ) const;
};


#endif

// EOF
