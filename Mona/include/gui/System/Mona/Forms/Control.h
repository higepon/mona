// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_MONA_FORMS_CONTROL_H__
#define __SYSTEM_MONA_FORMS_CONTROL_H__

#include <gui/messages.h>
#include <gui/System/Mona/Forms/Events.h>
#include <gui/System/Drawing/Bitmap.h>
#include <gui/System/Drawing/Color.h>
#include <gui/System/Drawing/Font.h>
#include <gui/System/Drawing/Graphics.h>
#include <gui/System/Drawing/Point.h>
#include <gui/System/Drawing/Rectangle.h>
#include <gui/System/Drawing/Size.h>

namespace System { namespace Mona { namespace Forms
{
	class Control : public System::Object
	{
		class ControlCollection : public System::Collections::ArrayList<_P<Control> >
		{
			friend class Control;
		private:
			_P<Control> target;
		
		public:
			ControlCollection() {}
			virtual ~ControlCollection() {}
			
			virtual void Add(_P<Control> control);
		};
		
	private:
		System::Drawing::Rectangle bounds;
		bool visible, capture;
		System::Drawing::Color foreColor;
		System::Drawing::Color backColor;
		_P<Control> parent;
		_P<ControlCollection> controls;
		System::String text;
		System::Drawing::Size clientSize;
		bool foreColorChanged, backColorChanged;
		
	protected:
		_P<System::Drawing::Bitmap> buffer;
		System::Drawing::Point offset;
		System::Drawing::Point clickPoint;
		guiserver_window* _object;
	
	public:
		virtual String get_TypeName() { return "System.Mona.Forms.Control"; }
		Control();
		virtual ~Control();
		
		virtual void Show();
		virtual void Hide();
		virtual void Create();
		virtual void Close();
		virtual void Dispose();
		void Refresh();
		virtual _P<System::Drawing::Graphics> CreateGraphics();
		virtual void WndProc(MessageType type, _P<EventArgs> e);
		inline unsigned int get_Handle() { return this->_object != NULL ? this->_object->Handle : 0; }
		
		System::Drawing::Point PointToClient(System::Drawing::Point p);
		System::Drawing::Point PointToScreen(System::Drawing::Point p);
		_P<Control> get_TopLevelControl();
		_P<Control> FindControl(int x, int y);
	
		inline _P<Control> get_Parent() { return this->parent; }
		inline _P<ControlCollection> get_Controls() { return this->controls; }
		
		inline int get_X() { return this->bounds.X; }
		inline int get_Y() { return this->bounds.Y; }
		inline System::Drawing::Rectangle get_Bounds() { return this->bounds; }
		inline void set_Bounds(System::Drawing::Rectangle r) { this->bounds = r; }
		inline System::Drawing::Point get_Location() { return this->bounds.get_Location(); }
		void set_Location(System::Drawing::Point p);
		inline System::Drawing::Size get_Size() { return this->bounds.get_Size(); }
		inline void set_Size(System::Drawing::Size sz) { this->bounds.Width = sz.Width; this->bounds.Height = sz.Height; }
		inline int get_Width() { return this->bounds.Width; }
		inline void set_Width(int width) { this->bounds.Width = width; }
		inline int get_Height() { return this->bounds.Height; }
		inline void set_Height(int height) { this->bounds.Height = height; }
		
		inline bool get_Visible() { return this->visible; }
		void set_Visible(bool v);
		
		inline bool get_Capture() { return this->capture; }
		void set_Capture(bool v);
		
		System::Drawing::Size get_ClientSize();
		void set_ClientSize(System::Drawing::Size sz);
		
		inline System::String get_Text() { return this->text; }
		void set_Text(System::String text);
		
		inline System::Drawing::Color get_ForeColor() { return this->foreColor; }
		void set_ForeColor(System::Drawing::Color c);
		inline System::Drawing::Color get_BackColor() { return this->backColor; }
		void set_BackColor(System::Drawing::Color c);
		
		static _P<System::Drawing::Font> get_DefaultFont();
	
	protected:
		void RefreshInternal();
		
		virtual void OnPaint() {}
		virtual void OnTextChanged(_P<EventArgs> e);
		virtual void OnForeColorChanged(_P<EventArgs> e);
		virtual void OnBackColorChanged(_P<EventArgs> e);
		virtual void OnMouseMove(_P<MouseEventArgs> e);
		virtual void OnMouseDown(_P<MouseEventArgs> e);
		virtual void OnMouseUp(_P<MouseEventArgs> e);
		virtual void OnClick(_P<EventArgs> e);
		virtual void OnNCMouseDown(_P<MouseEventArgs> e) {};
		virtual void OnNCMouseUp(_P<MouseEventArgs> e) {};
		virtual void OnNCMouseMove(_P<MouseEventArgs> e) {};
		
		DECLARE_EVENT(System::IEventHandler, TextChanged, _P<EventArgs>)
		DECLARE_EVENT(System::IEventHandler, ForeColorChanged, _P<EventArgs>)
		DECLARE_EVENT(System::IEventHandler, BackColorChanged, _P<EventArgs>)
		DECLARE_EVENT(IMouseEventHandler, MouseMove, _P<MouseEventArgs>)
		DECLARE_EVENT(IMouseEventHandler, MouseDown, _P<MouseEventArgs>)
		DECLARE_EVENT(IMouseEventHandler, MouseUp, _P<MouseEventArgs>)
		DECLARE_EVENT(System::IEventHandler, Click, _P<System::EventArgs>)
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_CONTROL_H__
