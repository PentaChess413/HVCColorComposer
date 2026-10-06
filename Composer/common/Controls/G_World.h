
#ifndef G_World_h
#define G_World_h

#include "pigeneral.h"
#include "asTypes.h"

// Class to manage an offscreen pixel buffer
class	G_World
{
	public:
		// contructors:
		G_World() : m_world( empty_map() ) {}

		// Class to auto-dispose of a G_World
		class Hold
		{
			public:
				Hold( G_World& p_world ): m_gworld( &p_world ) {}
				Hold( const Hold& s )	: m_gworld( s.m_gworld ) { s.m_gworld = NULL; }
				~Hold();

				void release() { m_gworld = NULL; }

			private:
				Hold&	operator=( const Hold& );
			
				// data members
				mutable G_World* m_gworld;
		};


		// Manipulators:

		// Is it valid
		operator			bool() const { return (m_world.baseAddr != NULL); }

		// Manage system memory
		Hold		create(const ASRect& bounds, int16 planes = 4 );
		void		dispose();

		// Get the system memory
		const PSPixelMap&	get() const							{ return m_world; }

		// Change the mode
		void				set_mode(int32 image_mode)			{ m_world.imageMode = image_mode; }

	private:
		G_World(const G_World&);
		G_World& operator=(const G_World&);

		// Create an empty pixel buffer
		static PSPixelMap empty_map();

		// data members
		PSPixelMap	m_world;
};

#endif

// EOF
