// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <monapi/messages.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Button.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Collections/ArrayList.h>
#include <monapi/CString.h>

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;
using namespace System::Mona::Forms;

extern _P<MonAPI::Screen> GetDefaultScreen();

static bool ExistsProcess(const MonAPI::CString& self)
{
	syscall_set_ps_dump();
	PsInfo info;

	bool ret = false;
	dword tid = syscall_get_tid();

	while (syscall_read_ps_dump(&info) == 0)
	{
		if (!ret && self == info.name && tid != info.tid) ret = true;
	}
	if (ret) printf("%s: already has executed!\n", (const char*)self);

	return ret;
}

static int ProcessStart(const String& file)
{
	int len = file.get_Length();
	_A<char> elf(len + 1);
	for (int i = 0; i < len; i++)
	{
		wchar ch = file[i];
		elf[i] = ch < 128 ? ch : '?';
	}
	elf[len] = '\0';
	return monapi_call_elf_execute_file(elf.get(), 0);
}

class Form1 : public Form
{
private:
	static _P<ArrayList<String> > elfs;
	_A<_P<Button> > buttons;
	
public:
	Form1()
	{
		this->InitializeComponent();
		
		int len = Form1::elfs->get_Count();
		this->buttons.Alloc(len);
		this->set_ClientSize(Size(128, len * 20));
		this->set_Location(Point(::GetDefaultScreen()->getWidth() - this->get_Width(), 0));
		for (int i = 0; i < len; i++)
		{
			this->buttons[i] = new Button();
			this->buttons[i]->set_Bounds(Rectangle(0, i * 20, 128, 20));
			this->buttons[i]->set_Text(Form1::elfs->get_Item(i));
			this->buttons[i]->add_Click(new EventHandler<Form1>(this, &Form1::button_Click));
			this->get_Controls()->Add(this->buttons[i].get());
		}
	}
	
private:
	void InitializeComponent()
	{
		this->set_Text("Mona Launcher");
	}
	
	void button_Click(_P<Object> sender, _P<EventArgs> e)
	{
		_P<Button> button = (Button*)sender.get();
		String file = "/APPS/" + button->get_Text();
		if (file.EndsWith(".APP"))
		{
			file += '/';
			int len = button->get_Text().get_Length();
			file += button->get_Text().Substring(0, len - 4);
			file += ".EL2";
		}
		::ProcessStart(file);
	}
	

public:
	static void Main(_A<String> args)
	{
		if (ExistsProcess("LAUNCHER.EL2")) return;

		if (syscall_cd("/APPS"))
		{
			printf("ERROR: directory /APPS not found\n");
			return;
		}
		if (syscall_dir_open())
		{
			printf("ERROR: can not open directory\n");
			return;
		}
		Form1::elfs = new ArrayList<String>;
		char name[16];
		int size;
		int attr;
		while (syscall_dir_read(name, &size, &attr) == 0)
		{
			String n = name;
			if (n.EndsWith(".ELF") || n.EndsWith(".EL2") || n.EndsWith(".APP"))
			{
				Form1::elfs->Add(name);
			}
		}
		syscall_dir_close();
		syscall_cd("/");
		
		Application::Run(new Form1());
	}
};
_P<ArrayList<String> > Form1::elfs;

SET_MAIN_CLASS(Form1)
