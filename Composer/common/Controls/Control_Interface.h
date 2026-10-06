/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Control_Interface_h
#define Control_Interface_h

#include "ADM_Access.h"

#include "admItem.h"
#include "SpDebug.h"


class	Dialog_Runner;

class Control_Interface
{
	public:
		Control_Interface( ADMItemRef p_control ) : m_control(p_control) { }

		Dialog_Runner*	Dialog() const;

		ASInt32		id() const;

		void		text(char* buffer, const ASInt32 max_length) const;
		ASInt32		text_length() const;
		void		set_text(const char* buffer);

		bool		bool_value() const;
		void		set_bool_value(const bool p_value);

		ASInt32		int_value() const;
		void		set_int_value(const ASInt32 p_value);
		float		float_value() const;
		void		set_float_value(const float p_value);

		ASInt32		max_int_value() const;
		void		set_max_int_value(const ASInt32 p_max);
		float		max_float_value() const;
		void		set_max_float_value(const float p_max);

		ASInt32		min_int_value() const;
		void		set_min_int_value(const ASInt32 p_min);
		float		min_float_value() const;
		void		set_min_float_value(const float p_min);

		ASInt32		max_text_length() const;
		void		set_max_text_length(const ASInt32 p_length);

		void		invalidate();
		void		force_draw();

		ADMUnits	units() const;
		void		set_units(const ADMUnits p_length);

		ASRect		bounds( bool clientCoords = false ) const;
		void		bounds(ASRect& p_bounds) const;
		void		set_bounds(const ASRect& p_bounds);

		void		move(const ASPoint& p_position);
		void		resize(const ASPoint& p_size);

		bool		visible() const;
		void		set_visible(const bool p_show);

		void*		user_data() const;
		void		set_user_data(void* data);

		void		set_proc(ADMItemDrawProc draw_proc);
		void		set_proc(ADMItemTrackProc track_proc);
		void		set_proc(ADMItemNotifyProc notify_proc);
		void		set_proc(ADMItemDestroyProc destroy_proc);

		ASWindowRef GetWindowRef();

		void		Enable( bool inEnable );
		bool		IsEnabled();
		
		void		SetSmallIncrement( float inIncrement );	
		float		GetSmallIncrement();
		
		void		SetLargeIncrement( float inIncrement );
		float		GetLargeIncrement();

		ADMFont		GetFont();
		void		SetFont( ADMFont font );
		
		ADMListRef	GetList();
		
		ADMActionMask	GetMask();
		void			SetMask( ADMActionMask );
		
		void			SetPopupDialog( ASInt32 inPopupItemID, ADMDialogRef inDialog );
		ADMDialogRef	GetPopupDialog();

		void			SetItemStyle( ADMItemStyle inItemStyle );
		ADMItemStyle	GetItemStyle();

		void					SetFloatToTextProc( ADMItemFloatToTextProc inProc );
		ADMItemFloatToTextProc	GetFloatToTextProc();

		void					SetTextToFloatProc( ADMItemTextToFloatProc inProc );
		ADMItemTextToFloatProc	SetTextToFloatProc();

		ADMTimerRef				CreateTimer( ASUInt32 inMilliseconds, ADMActionMask inAbortMask, ADMItemTimerProc inTimerProc, ADMItemTimerAbortProc inTimerAbortProc, ASInt32 inOptions );
		void					AbortTimer( ADMTimerRef inTimer );


	protected:
		ADMItemRef	native()										{ return m_control; }

		void		clear()											{ m_control = NULL; }

	private:
		ADMItemRef	m_control;
};

inline ASInt32 Control_Interface::id() const
{
	return ADM_Access::item_suite()->GetID(m_control);
}

inline void Control_Interface::text(char* buffer, const ASInt32 max_length) const
{
	ADM_Access::item_suite()->GetText(m_control, buffer, max_length);
}

inline ASInt32 Control_Interface::text_length() const
{
	return ADM_Access::item_suite()->GetTextLength(m_control);
}

inline void Control_Interface::set_text(const char* buffer)
{
	ADM_Access::item_suite()->SetText(m_control, buffer);
}

inline bool Control_Interface::bool_value() const
{
	return (ADM_Access::item_suite()->GetBooleanValue( m_control ) == (ASBoolean)true);
}

inline void Control_Interface::set_bool_value(const bool p_value)
{
	ADM_Access::item_suite()->SetBooleanValue(m_control, p_value);
}

inline ASInt32 Control_Interface::int_value() const
{
	return ADM_Access::item_suite()->GetIntValue(m_control);
}

inline void Control_Interface::set_int_value(const ASInt32 p_value)
{
	ADM_Access::item_suite()->SetIntValue(m_control, p_value);
}

inline ASInt32 Control_Interface::max_int_value() const
{
	return ADM_Access::item_suite()->GetMaxIntValue(m_control);
}

inline void Control_Interface::set_max_int_value(const ASInt32 p_max)
{
	ADM_Access::item_suite()->SetMaxIntValue(m_control, p_max);
}

inline ASInt32 Control_Interface::min_int_value() const
{
	return ADM_Access::item_suite()->GetMinIntValue(m_control);
}

inline void Control_Interface::set_min_int_value(const ASInt32 p_min)
{
	ADM_Access::item_suite()->SetMinIntValue(m_control, p_min);
}

inline float Control_Interface::float_value() const
{
	return ADM_Access::item_suite()->GetFloatValue(m_control);
}

inline void Control_Interface::set_float_value(const float p_value)
{
	ADM_Access::item_suite()->SetFloatValue(m_control, p_value);
}

inline float Control_Interface::max_float_value() const
{
	return ADM_Access::item_suite()->GetMaxFloatValue(m_control);
}

inline void Control_Interface::set_max_float_value(const float p_max)
{
	ADM_Access::item_suite()->SetMaxFloatValue(m_control, p_max);
}

inline float Control_Interface::min_float_value() const
{
	return ADM_Access::item_suite()->GetMinFloatValue(m_control);
}

inline void Control_Interface::set_min_float_value(const float p_min)
{
	ADM_Access::item_suite()->SetMinFloatValue(m_control, p_min);
}

inline ASInt32 Control_Interface::max_text_length() const
{
	return ADM_Access::item_suite()->GetMaxTextLength(m_control);
}

inline void Control_Interface::set_max_text_length(const ASInt32 p_length)
{
	ADM_Access::item_suite()->SetMaxTextLength(m_control, p_length);
}

inline ADMUnits Control_Interface::units() const
{
	return ADM_Access::item_suite()->GetUnits(m_control);
}

inline void Control_Interface::set_units(const ADMUnits p_units)
{
	ADM_Access::item_suite()->SetUnits(m_control, p_units);
}

inline void Control_Interface::invalidate()
{
	ADM_Access::item_suite()->Invalidate( m_control );
}

inline void Control_Interface::force_draw()
{
	ADM_Access::item_suite()->Update(m_control);
}

inline void* Control_Interface::user_data() const
{
	return ADM_Access::item_suite()->GetUserData(m_control);
}

inline void Control_Interface::set_user_data(void* data)
{
	ADM_Access::item_suite()->SetUserData(m_control, data);
}

inline void Control_Interface::set_proc(ADMItemDrawProc draw_proc)
{
	ADM_Access::item_suite()->SetDrawProc(m_control, draw_proc);
}

inline void Control_Interface::set_proc(ADMItemTrackProc track_proc)
{
	ADM_Access::item_suite()->SetTrackProc(m_control, track_proc);
}

inline void Control_Interface::set_proc(ADMItemNotifyProc notify_proc)
{
	ADM_Access::item_suite()->SetNotifyProc(m_control, notify_proc);
}

inline void Control_Interface::set_proc(ADMItemDestroyProc destroy_proc)
{
	ADM_Access::item_suite()->SetDestroyProc(m_control, destroy_proc);
}

inline void Control_Interface::bounds(ASRect& p_bounds) const
{
	ADM_Access::item_suite()->GetBoundsRect(m_control, &p_bounds);
}

inline ASRect Control_Interface::bounds( bool clientCoords ) const
{
	ASRect	temp = { 0 };

	bounds(temp);

	if( clientCoords )
	{	
		// make this rectangle relative to 0,0
		temp.right  -= temp.left;
		temp.bottom -= temp.top;
		temp.left = temp.top = 0;
	}

	return temp;
}

inline void Control_Interface::set_bounds(const ASRect& p_bounds)
{
	ADM_Access::item_suite()->SetBoundsRect(m_control, &p_bounds);
}

inline void Control_Interface::move(const ASPoint& p_position)
{
	ADM_Access::item_suite()->Move(m_control, p_position.h, p_position.v);
}

inline void Control_Interface::resize(const ASPoint& p_size)
{
	ADM_Access::item_suite()->Size(m_control, p_size.h, p_size.v);
}

inline bool Control_Interface::visible() const
{
	return (ADM_Access::item_suite()->IsVisible(m_control) == (ASBoolean)true);
}

inline void Control_Interface::set_visible(const bool p_show)
{
	ADM_Access::item_suite()->Show(m_control, p_show);
}

inline ASWindowRef Control_Interface::GetWindowRef()
{
	return ADM_Access::item_suite()->GetWindowRef( m_control );
}

inline void	Control_Interface::Enable( bool inEnable )
{
	ADM_Access::item_suite()->Enable( m_control, inEnable );
}

inline bool	Control_Interface::IsEnabled()
{
	return ADM_Access::item_suite()->IsEnabled( m_control ) == 1;
}

inline void	Control_Interface::SetSmallIncrement( float inIncrement )
{
	ADM_Access::item_suite()->SetSmallIncrement( m_control, inIncrement );
}

inline float Control_Interface::GetSmallIncrement()
{
	return ADM_Access::item_suite()->GetSmallIncrement( m_control );
}

inline void	Control_Interface::SetLargeIncrement( float inIncrement )
{
	ADM_Access::item_suite()->SetLargeIncrement( m_control, inIncrement );
}

inline float Control_Interface::GetLargeIncrement()
{
	return ADM_Access::item_suite()->GetLargeIncrement( m_control );
}

inline ADMFont Control_Interface::GetFont()
{
	return ADM_Access::item_suite()->GetFont( m_control );
}

inline void Control_Interface::SetFont( ADMFont font )
{
	ADM_Access::item_suite()->SetFont( m_control, font );
}

inline ADMListRef Control_Interface::GetList()
{
	return ADM_Access::item_suite()->GetList( m_control );
}


inline ADMActionMask Control_Interface::GetMask()
{
	return ADM_Access::item_suite()->GetMask( m_control );
}

inline void Control_Interface::SetMask( ADMActionMask mask )
{
	ADM_Access::item_suite()->SetMask( m_control, mask );
}

inline void Control_Interface::SetPopupDialog( ASInt32 inPopupItemID, ADMDialogRef inDialog )
{
	ADM_Access::item_suite()->SetPopupDialog( m_control, inPopupItemID, inDialog );
}

inline ADMDialogRef Control_Interface::GetPopupDialog()
{
	return ADM_Access::item_suite()->GetPopupDialog( m_control ); 
}


inline void Control_Interface::SetItemStyle( ADMItemStyle inItemStyle )
{
	ADM_Access::item_suite()->SetItemStyle( m_control, inItemStyle );
}


inline ADMItemStyle Control_Interface::GetItemStyle()
{
	return ADM_Access::item_suite()->GetItemStyle( m_control ); 
}

inline void Control_Interface::SetFloatToTextProc( ADMItemFloatToTextProc inProc )
{
	ADM_Access::item_suite()->SetFloatToTextProc( m_control, inProc ); 
}


inline ADMItemFloatToTextProc Control_Interface::GetFloatToTextProc()
{
	return ADM_Access::item_suite()->GetFloatToTextProc( m_control ); 
}


inline void	Control_Interface::SetTextToFloatProc( ADMItemTextToFloatProc inProc )
{
	ADM_Access::item_suite()->SetTextToFloatProc( m_control, inProc ); 
}


inline ADMItemTextToFloatProc Control_Interface::SetTextToFloatProc()
{
	return ADM_Access::item_suite()->GetTextToFloatProc( m_control ); 
}


inline ADMTimerRef Control_Interface::CreateTimer( ASUInt32 inMilliseconds, ADMActionMask inAbortMask, ADMItemTimerProc inTimerProc, ADMItemTimerAbortProc inTimerAbortProc, ASInt32 inOptions )
{
	return ADM_Access::item_suite()->CreateTimer( m_control, inMilliseconds, inAbortMask, inTimerProc, inTimerAbortProc, inOptions ); 
}

inline void Control_Interface::AbortTimer( ADMTimerRef inTimer )
{
	return ADM_Access::item_suite()->AbortTimer( m_control, inTimer ); 
}


#endif

// EOF
