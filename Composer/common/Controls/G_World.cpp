
#include "PhotoshopSDK.h"
#include "G_World.h"

extern FilterRecord* gFilterRecord;

// Dispose of a valid G_World
G_World::Hold::~Hold()
{
	if( m_gworld )
		m_gworld->dispose();
}


// Create the pixel memory from system memory
G_World::Hold G_World::create(const ASRect& bounds, int16 planes)
{
	// define the pixel bounds
	m_world.bounds.top    = bounds.top;
	m_world.bounds.left   = bounds.left;
	m_world.bounds.bottom = bounds.bottom;
	m_world.bounds.right  = bounds.right;
	m_world.rowBytes      = bounds.right - bounds.left;
	m_world.planeBytes    = (bounds.bottom - bounds.top) * m_world.rowBytes;

	// Allocate the pixel memory
	m_world.baseAddr = ::malloc( m_world.planeBytes * planes );

	return *this;
}


// Release the pixel memory -- fix memory leak...
void G_World::dispose()
{
	::free( m_world.baseAddr );
	m_world.baseAddr = NULL;
}

// Create an empty pixel buffer
PSPixelMap G_World::empty_map()
{
	PSPixelMap	empty = { 1 };

	empty.colBytes = 1;

	return empty;
}

// EOF
