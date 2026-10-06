/*
 *
 *	Copyright 2005-2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "PhotoshopSDK.h"

#include "Composer.h"
#include "Scripting.h"
#include "Composer-sym.h"
#include "ADM_Access.h"
#include "MCGotoURL.h"
#include "Dialog_Runner.h"
#include "MCLicense.h"
#include "License.h"

// !!@ hack alert
#ifndef IDOK
#define IDOK 1
#endif


// use this when you don't have a licensing library to link to- 
#define BYPASS_LICENSE


// Error string indexes
enum {	ERROR_STRINGS = 16102, PARAM_ERROR = 1, WEB_ERROR };

enum {	BUFFER_LENGTH = 2048, USER_DATA_LENGTH = 512 };

// Class to handle the about dialog
class	Run_About : public Dialog_Runner, public License_Handler
{
	typedef Dialog_Runner	base;
	typedef License_Handler	alt_base;

public:
	// Constructors:
					Run_About(SPPluginRef p_plugin_ref);
	virtual			~Run_About()									{ }

	virtual ASInt32	run();

	virtual void	do_licensed_setup(const char* owner_name);
	virtual void	do_unlicensed_setup();

private:
	SPPluginRef	m_plugin_ref;

	// Virtual functions from the base:
	virtual bool	do_handle_item(short item);

	void			show_help();
	void			show_company();
	void			show_purchase_page();
	// Handle registration data:
	bool			do_register();
	ASRect			move_control(ASInt32 item_id, const ASRect& top_bounds);
	
	// used to forward notifies to the do_handle_item routine...
	static void ASAPI ItemCallback( ADMItemRef p_control, ADMNotifierRef notifier );
};


// Initialize the dialog
Run_About::Run_About( SPPluginRef p_plugin_ref ) :
	m_plugin_ref( p_plugin_ref )
{
	if( !Create( p_plugin_ref, "About HVC Color Composer", 16003 ) )
		return;

	ADM_Access::dialog_suite()->Show(GetNative(), false);
	SetDialogFinishedCallback( IDOK );

	// make the buttons responsive:
	get_item_interface( IDC_DO_REGISTER ).set_proc( ItemCallback );
	get_item_interface( IDC_COMPOSER_LOGO ).set_proc( ItemCallback );
	get_item_interface( IDC_GOTO_MASTER_COLORS ).set_proc( ItemCallback );
	get_item_interface( IDC_GOTO_PURCHASE ).set_proc( ItemCallback );
}

// Handle an item hit
bool Run_About::do_handle_item(short item)
{
	switch (item)
	{
		case IDC_DO_REGISTER:
			return do_register();

		case IDC_COMPOSER_LOGO:
			show_help();
			break;

		case IDC_GOTO_MASTER_COLORS:
			show_company();
			break;

		case IDC_GOTO_PURCHASE:
			show_purchase_page();
			break;

		default:
			break;
	}

	return true;					// Continue the event loop
}

// Adjust the dialog to the licensed mode
inline ASRect Run_About::move_control(ASInt32 item_id, const ASRect& top_bounds)
{
	Control_Interface	control = get_item_interface(item_id);
	ASRect				bottom_bounds = control.bounds();
	ASPoint				new_position = { bottom_bounds.left, top_bounds.top};

	control.move(new_position);

	return bottom_bounds;
}

// Adjust the dialog to the licensed mode
void Run_About::do_licensed_setup(const char* owner_name)
{
	ASRect	top_bounds = get_item_interface(IDC_DO_REGISTER).bounds();

	show_hide_range(IDC_GOTO_PURCHASE, IDC_OWNER_NAME_LABEL, false);					// Hide the registration controls

	get_item_interface(IDC_OWNER_NAME).set_visible(true);

	// Move the buttons
	move_control(IDC_GOTO_MASTER_COLORS, top_bounds);
	ASRect	bottom_bounds = move_control(IDOK, top_bounds);

	// Shrink the window
	ASRect dialog_rect = { 0 };

	GetBoundsRect(&dialog_rect, true);
	
	Size(dialog_rect.right - dialog_rect.left,
		dialog_rect.bottom - dialog_rect.top +
		top_bounds.top - bottom_bounds.top);

	// Display the owner name in the owner name control
	get_item_interface(IDC_OWNER_NAME).set_text(owner_name);
}

// Adjust the dialog to the demo mode
void Run_About::do_unlicensed_setup()
{
	get_item_interface(IDC_OWNER_NAME).set_visible(false);
}

// Ask the licensing system to try to license the program
bool Run_About::do_register()
{
	char	owner_buffer[USER_DATA_LENGTH] = { 0 };
	char	code_buffer[USER_DATA_LENGTH] = { 0 };

	get_item_interface(IDC_ENTER_OWNER_NAME).text(owner_buffer, USER_DATA_LENGTH - 1);
	get_item_interface(IDC_ENTER_PURCHASE_CODE).text(code_buffer, USER_DATA_LENGTH - 1);

	char		buffer[BUFFER_LENGTH] = { 0 };
	int			buffer_length = BUFFER_LENGTH - 1;
	const char	short_version[10] = ShortVersionString;

#ifdef BYPASS_LICENSE
	long	err = noErr;
#else
	// Ask the licensing system to license the program
	long	err = register_software( owner_buffer, code_buffer, short_version, buffer, &buffer_length );
#endif

	if (noErr == err)
	{
		if (buffer_length)								// Pass the result URL to the browser
			MCURL::Go(buffer);

		return false;									// Licensing successful
	}

	if (0 == buffer_length)
	{
		sprintf(buffer, "An error occured trying to verify your "
				"purchase.\n\n\n%Li", err);
	}

	ADM_Access::basic_suite()->ErrorAlert(buffer);

	return true;
}

// Pass the help URL to the browser
void Run_About::show_help()
{
	MCURL::Go( m_plugin_ref, HELP_URL );
}

// Pass the company URL to the browser
void Run_About::show_company()
{
	// Pass the url to the browser
	MCURL::Go( m_plugin_ref, COMPANY_URL );
}

// Pass the purchase URL to the browser
void Run_About::show_purchase_page()
{
	// Pass the url to the browser
	MCURL::Go( m_plugin_ref, PURCHASE_URL );								
}

ASInt32 Run_About::run()
{
#ifdef BYPASS_LICENSE
	ASUInt32 error = 0;

#else
	ASUInt32 error = license_check();
#endif

	if( error )
		throw error;

	return base::run();
}

void ASAPI Run_About::ItemCallback( ADMItemRef p_control, ADMNotifierRef notifier )
{
	// only execute this code when the notify is that the user changed...
	if( !ADM_Access::notifier_suite()->IsNotifierType( notifier, kADMUserChangedNotifier ) )
		return;

	Control_Interface	control( p_control );
	Run_About*			ui = dynamic_cast<Run_About*>( control.Dialog() );

	if( !ui )
		return;
		
	// extract the item number and call the do_handle_item
	if( !ui->do_handle_item( (short)control.id() ) )
	{
		ADM_Access::dialog_suite()->DefaultNotify( ADM_Access::item_suite()->GetDialog( p_control ), notifier );
		ui->end_modal(control.id(), false);
	}
}

// Handle the Photoshop call by displaying and running the about box
const bool Composer::About( AboutRecord* about_record )
{
	if( !about_record || !about_record->plugInRef )
		return false;

	Run_About	about_dialog( static_cast<SPPluginRef>( about_record->plugInRef ) );

	// Run the dialog until IDOK selected
	while( IDOK != about_dialog.run() )
		;

	return true;
}

// Show and run the about box
void Composer::show_about_box()
{
	Run_About	about_dialog( static_cast<SPPluginRef>( pickerRecord->plugInRef ) );

	// Run the dialog until IDOK or Register selected
	about_dialog.run();
}

// EOF
