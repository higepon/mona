// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_MONA_FORMS_APPLICATION_H__
#define __SYSTEM_MONA_FORMS_APPLICATION_H__

#include <monapi.h>
#include <gui/System/Pointer.h>
#include <gui/System/String.h>
#include <gui/System/Collections/ArrayList.h>

#define SET_MAIN_CLASS(T) int MonaMain(List<char*>* pekoe) { System::Mona::Forms::Application::__main<T>(pekoe); return 0; }

namespace System { namespace Mona { namespace Forms
{
	class Form;
	class Control;
	
	class Application
	{
	private:
		static _P<System::Collections::ArrayList<_P<Form> > > forms;
		static _P<Control> prevControl;
		static _P<Form> mainForm;
		static bool isExited;
		static int mouseButtons;
		
		static void Initialize();
		static void Dispose();
	
	public:
		static unsigned char* defaultFontData;
		
		static void Run();
		static void Run(_P<Form> form);
	    static void Exit();
		
		static void AddForm(_P<Form> f);
		static void RemoveForm(_P<Form> f);
		static _P<Form> FindForm(int x, int y);
		
		template <class T> static void __main(List<char*>* pekoe)
		{
			int len = pekoe->size();
			_A<String> args(len);
			for (int i = 0; i < len; i++) args[i] = pekoe->get(i);
			
			Application::Initialize();
			T::Main(args);
			Application::Dispose();
		}
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_APPLICATION_H__
