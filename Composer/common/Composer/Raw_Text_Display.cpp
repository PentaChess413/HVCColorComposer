/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Raw_Text_Display.h"
#include "Composer_UI.h"
#include "Helpers.h"

#include "admNotifier.h"

#define MIN	0.0
#define MAX 22.0

enum {	MAX_TEXT_LENGTH = 6, BUFFER_LENGTH = 7, PLANE_SIZE = 8 };

Raw_Text_Display::Raw_Text_Display( ComposerUI& dialog_runner, const ASInt32 control_id, const short p_rgb[4] ) :
	base( dialog_runner, control_id )
{
	copy_color( m_rgb, p_rgb );
	set_max_text_length( MAX_TEXT_LENGTH );
	Raw_Text_Display::Update();				// hackish !!@
}

inline void Raw_Text_Display::update_value()
{
	char buffer[BUFFER_LENGTH] = {0};
	
	text(buffer, BUFFER_LENGTH);

	std::string			holder(buffer);
	std::istringstream	hex_input(holder);
	unsigned long		temp;
	short				result[3];
	bool				changed = false;

	hex_input >> std::hex >> temp;

	for (int i = 3; i-- > 0; temp >>= 8)				// Copy the color components
	{
		result[i] = (short)(temp & 0xFF);
		if (result[i] != m_rgb[i])
			changed = true;								// The color did change
	}

	if (changed && 0 == temp)
	{
		for (int i = 3; i-- > 0;)
			m_rgb[i] = result[i];

		GetComposerUI()->GetColorSelector()->RawCallback( *this );
	}
}


void Raw_Text_Display::HandleNotify( ADMNotifierRef notifier )
{
	if( ADM_Access::notifier_suite()->IsNotifierType( notifier, kADMUserChangedNotifier ) )
		update_value();
}


ASBoolean Raw_Text_Display::HandleTracking( ADMTrackerRef tracker )
{
	ASBoolean result = base::HandleTracking( tracker );

	update_value();

	return result;
}


void Raw_Text_Display::update_color( const short p_rgb[4] )
{
	copy_color( m_rgb, p_rgb );
	Update();
}


void Raw_Text_Display::Update()
{
	std::ostringstream	hex_output;						// Calculate the text representation

	for( int i = 0; i < 3; ++i)
	{
		if( 0x10 > m_rgb[i] )
			hex_output << 0;

		hex_output << std::uppercase << std::hex << m_rgb[i];
	}

	set_text( hex_output.str().c_str() );
}
