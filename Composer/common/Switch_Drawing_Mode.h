/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Switch_Drawing_Mode_h
#define Switch_Drawing_Mode_h

#include "ADM_Access.h"
#include "admDrawer.h"


class	Switch_Drawing_Mode
{
	public:
		Switch_Drawing_Mode(ADMDrawerRef p_environment, const ADMDrawMode p_new_mode) :
			m_environment(p_environment ),
			m_old_mode( ADM_Access::drawing_suite()->GetDrawMode( p_environment ) )
		{
			set_drawing_mode(p_new_mode);
		}
		
		~Switch_Drawing_Mode() { set_drawing_mode(m_old_mode); }

	private:
		ADMDrawerRef	m_environment;
		ADMDrawMode		m_old_mode;

		inline void	set_drawing_mode(const ADMDrawMode new_mode) { ADM_Access::drawing_suite()->SetDrawMode( m_environment, new_mode ); }
};

#endif

// EOF
