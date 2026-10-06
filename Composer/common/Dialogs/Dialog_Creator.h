/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Dialog_Creator_h
#define Dialog_Creator_h

#include "Selected_Color.h"

template<typename T>class Dialog_Creator : public T
{
	public:
		typedef std::auto_ptr<Dialog_Creator>	Dialog_Creator_Ptr;

		static Dialog_Creator_Ptr create( SelectedColorRef p_selected_color, const char* p_name, ASInt32 p_dialog_id );

	private:
		Dialog_Creator( ADMDialogRef p_dialog, SelectedColorRef p_selected_color ) :
			T( p_selected_color, p_dialog ), 
		{}

		struct Creation_Data
		{
			Dialog_Creator*		m_dialog_handler;
			SelectedColorRef	m_selected_color;
		};

		static ASAPI ASErr dialog_init_handler( ADMDialogRef p_dialog );
};


template<typename T>std::auto_ptr< Dialog_Creator<T> > Dialog_Creator<T>::create( SelectedColorRef p_selected_color, const char* p_name, ASInt32 p_dialog_id)
{
	Creation_Data	creation_data = { NULL, p_selected_color };

//	ADM_Access::dialog_suite()->Create( p_selected_color.GetPluginRef(), p_name, p_dialog_id, kADMModalDialogStyle, &dialog_init_handler, &creation_data, 0 );
	
	// force roman font
	ADM_Access::dialog_suite()->Create( p_selected_color.GetPluginRef(), p_name, p_dialog_id, kADMModalDialogStyle, &dialog_init_handler, &creation_data, true );
	if (!creation_data.m_dialog_handler)
		throw ASErr (kSPTroubleInitializingError);

	return Dialog_Creator_Ptr (creation_data.m_dialog_handler);
}


template<typename T>ASAPI ASErr Dialog_Creator<T>::dialog_init_handler(ADMDialogRef p_dialog)
{
	if (!p_dialog)
		return kSPBadParameterError;

	Creation_Data*	creation_data = static_cast<Creation_Data*> (
									ADM_Access::dialog_suite()->GetUserData(p_dialog));

	if (!creation_data)
		return kSPBadParameterError;

	try
	{
		creation_data->m_dialog_handler = new Dialog_Creator(p_dialog,
												creation_data->m_selected_color);
	}
	catch(const ASErr& err)
	{
#if XDEBUG	
		OutputDebugString( "ASErr caught.\n" );
#endif
		return err;
	}
	catch(...)
	{
#if XDEBUG	
		OutputDebugString( "runaway exception caught!\n" );
#endif
		return kSPTroubleInitializingError;
	}

	return kSPNoError;
}

#endif

// EOF
