/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Standard_Z_Selector_h
#define Standard_Z_Selector_h

#include "Color_Selector.h"


// Template class to implement the Z selector buffer fill functions independent of the color space
template<typename T> class Standard_Z_Selector : public T
{
	typedef T	base;

	protected:
		// Constructors:
		Standard_Z_Selector( short* p_color ) : 
			base( p_color ) 
		{}

		Standard_Z_Selector( short* p_color, short* p_color_text ) :
			base( p_color, p_color_text ) 
		{}

		virtual	~Standard_Z_Selector() {}

	private:
		// Virtual functions implmenting common code, should not to be overridden:
		virtual void	fill_z_buffer_0(ASUInt8* line, ASUInt8* last_line, int row_bytes);
		virtual void	fill_z_buffer_1(ASUInt8* line, ASUInt8* last_line, int row_bytes);
		virtual void	fill_z_buffer_2(ASUInt8* line, ASUInt8* last_line, int row_bytes);

		virtual void	fill_xy_buffer_0(ASUInt8* line, ASUInt8* last_line, int row_bytes);
		virtual void	fill_xy_buffer_1(ASUInt8* line, ASUInt8* last_line, int row_bytes);
		virtual void	fill_xy_buffer_2(ASUInt8* line, ASUInt8* last_line, int row_bytes);
};

// Split fill the x plane of the Z buffer with the actual and the base x values
template<typename T>inline void Standard_Z_Selector<T>::fill_z_buffer_0(ASUInt8* line, ASUInt8* last_line, int row_bytes)
{
	return this->fill_plane( (ASInt8)this->GetX(), this->default_x(), line, last_line, row_bytes );
}

// Split fill the y plane of the Z buffer with the actual and the base y values
template<typename T>inline void Standard_Z_Selector<T>::fill_z_buffer_1(ASUInt8* line, ASUInt8* last_line, int row_bytes)
{
	return this->fill_plane( (ASInt8)this->GetY(), this->default_y(), line, last_line, row_bytes );
}

// Fill the z plane of the Z buffer using the decrement algorithm
template<typename T>inline void Standard_Z_Selector<T>::fill_z_buffer_2(ASUInt8* line, ASUInt8* last_line, int row_bytes)
{
	return this->decrement_plane( line, last_line, row_bytes );
}

// Fill the x plane of the XY buffer using the decrement algorithm
template<typename T>inline void Standard_Z_Selector<T>::fill_xy_buffer_0(ASUInt8* line, ASUInt8* last_line, int row_bytes)
{
	return this->decrement_plane( line, last_line, row_bytes );
}

// Fill the y plane of the XY buffer using the row algorithm
template<typename T>inline void Standard_Z_Selector<T>::fill_xy_buffer_1(ASUInt8* line, ASUInt8* last_line, int row_bytes)
{
	return this->fill_row( line, last_line, row_bytes );
}

// Fill the z plane of the XY buffer with the actual z value
template<typename T>inline void Standard_Z_Selector<T>::fill_xy_buffer_2(ASUInt8* line, ASUInt8* last_line, int row_bytes)
{
	return this->fill_plane( (ASInt8)this->GetZ(), line, last_line, row_bytes );
}

#endif

// EOF
