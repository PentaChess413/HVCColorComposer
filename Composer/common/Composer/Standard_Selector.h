/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Standard_Selector_h
#define Standard_Selector_h

#include "Color_Selector.h"


// DEAD CODE NOW!

// Intermediate class extracting common code from multiple final classes.
class Standard_Selector : public Color_Selector
{
	typedef Color_Selector	base;

	public:
		// Destructor:
		virtual			~Standard_Selector()								{ }

	protected:
		// Constructor:
						Standard_Selector()									{ }

	private:
		// Virtual functions implmenting common code, should not to be overridden:
		virtual void	UpdateZbuffer( const PSPixelMap& map );
		virtual void	UpdateXYbuffer( const PSPixelMap& map );

		// Virtual functions to be overridden by derived classes:
		virtual void	fill_z_buffer_0(ASUInt8* line, ASUInt8* last_line, int row_bytes) = 0;
		virtual void	fill_z_buffer_1(ASUInt8* line, ASUInt8* last_line, int row_bytes) = 0;
		virtual void	fill_z_buffer_2(ASUInt8* line, ASUInt8* last_line, int row_bytes) = 0;

		virtual void	fill_xy_buffer_0(ASUInt8* line, ASUInt8* last_line, int row_bytes) = 0;
		virtual void	fill_xy_buffer_1(ASUInt8* line, ASUInt8* last_line, int row_bytes) = 0;
		virtual void	fill_xy_buffer_2(ASUInt8* line, ASUInt8* last_line, int row_bytes) = 0;
};

#endif

// EOF
