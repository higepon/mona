// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_MONA_FORMS_APPLICATION_H__
#define __SYSTEM_MONA_FORMS_APPLICATION_H__

#ifdef MONA
#include <monapi.h>
#define MAIN_ARGS int argc, char* argv[]
#define SET_MAIN_CLASS(T) int main(MAIN_ARGS) { \
	System::Mona::Forms::Application::__main<T>(argc, argv); return 0; }
#else
#define MAIN_ARGS int argc, char* argv[]
#define SET_MAIN_CLASS(T) int main(MAIN_ARGS) { \
	System::Mona::Forms::Application::__main<T>(argc, argv); return 0; }
#endif

#include <gui/System/Pointer.h>
#include <gui/System/String.h>
#include <gui/System/Collections/ArrayList.h>
#include <gui/System/Mona/Forms/IMessageFilter.h>

namespace System { namespace Mona { namespace Forms
{
	class Form;
	class Control;
	
	class Application
	{
	private:
		static _P<System::Collections::ArrayList<_P<Form> > > forms;
		static _P<Form> mainForm;
		static _P<System::Collections::ArrayList<IMessageFilter*> > messageFilters;
		static bool isExited;
		static int mouseButtons;
		
	public:
		static unsigned char* defaultFontData;
		
		static void Initialize();
		static void Dispose();
		
		static void Run();
		static void Run(_P<Form> form);
		static void DoEvents();
		static void ProcessEvent(Message* m);
		static void Exit();
		static void AddMessageFilter(IMessageFilter* value);
		static void RemoveMessageFilter(IMessageFilter* value);
		
		static void AddForm(_P<Form> f);
		static void RemoveForm(_P<Form> f);
		
		template <class T> static void __main(MAIN_ARGS)
		{
#ifdef MONA
			int len = argc - 1;
			_A<String> args(len);
			for (int i = 0; i < len; i++) args[i] = argv[i + 1];
#else
			_A<String> args(argc);
			for (int i = 0; i < argc; i++) args[i] = argv[i];
#endif
			
			Application::Initialize();
			T::Main(args);
			Application::Dispose();
		}
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_APPLICATION_H__
