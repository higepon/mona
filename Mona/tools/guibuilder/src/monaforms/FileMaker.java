package guibuilder;

import java.awt.*;
import java.io.*;

/**
 ファイル保存クラス。
 ここを変えればC++以外にも対応可能。
 */
public class FileMaker {
	private PWindow target;
	
	public FileMaker() {
	}
	
	/** Form1.cpp */
	private String getFile() {
		StringBuffer buffer = new StringBuffer();
		//
		// include
		//
		buffer.append("#include <monapi.h>\n");
		buffer.append("#include <gui/System/Mona/Forms/Application.h>\n");
		buffer.append("#include <gui/System/Mona/Forms/Form.h>\n");
		buffer.append("#include <gui/System/Mona/Forms/Label.h>\n");
		buffer.append("#include <gui/System/Mona/Forms/Button.h>\n\n");
		
		//
		// namespace
		//
		buffer.append("using namespace System;\n");
		buffer.append("using namespace System::Drawing;\n");
		buffer.append("using namespace System::Mona::Forms;\n\n");
		
		// class 定義
		buffer.append("class " + target.getName() + " : public Form {\n");
		
		//
		// private fields
		//
		buffer.append("private:\n");
		Component[] plist = target.getComponents();
		for (int i = 0; i < target.getComponentCount(); i++) {
			IParts p = (IParts)plist[i];
			buffer.append("\t_P<" + p.getClassName() + "> " + p.getName() + ";\n");
		}
		buffer.append("\n");
		
		//
		// public methods
		//
		buffer.append("public:\n");
		// constructor
		buffer.append("\t" + target.getName() + "() {\n");
		buffer.append("\t\tthis->InitializeComponent();\n");
		buffer.append("\t}\n\n");
		
		// destructor
		buffer.append("\t~" + target.getName() + "() {\n");
		buffer.append("\t}\n\n");
		
		//
		// private methods
		//
		buffer.append("private:\n");
		//
		// InitializeComponent
		//
		buffer.append("\tvoid InitializeComponent() {\n");
		// setBounds
		buffer.append("\t\tthis->set_Bounds(Rectangle(");
		buffer.append(target.getX() + ", ");
		buffer.append(target.getY() + ", ");
		buffer.append(target.getWidth() + ", ");
		buffer.append(target.getHeight() + "));\n");
		// setText
		buffer.append("\t\tthis->set_Text(\"" + target.getCaption() + "\");\n");
		// constructor
		for (int i = 0; i < target.getComponentCount(); i++) {
			IParts p = (IParts)plist[i];
			buffer.append("\t\t" + p.getName() + " = new " + p.getClassName() + "(" + p.getArgs() + ");\n");
		}
		// setText
		for (int i = 0; i < target.getComponentCount(); i++) {
			IParts p = (IParts)plist[i];
			if ((p instanceof PButton) || (p instanceof PLabel)) {
				buffer.append("\t\tthis->" + p.getName() + "->set_Text(\"");
				buffer.append(p.getCaption() + "\");\n");
			}
		}
		// setBounds
		for (int i = 0; i < target.getComponentCount(); i++) {
			IParts p = (IParts)plist[i];
			buffer.append("\t\tthis->" + p.getName() + "->set_Bounds(Rectangle(");
			buffer.append((p.getX() - IParts.INSETS_LEFT) + ", ");
			buffer.append((p.getY() - IParts.INSETS_TOP) + ", ");
			buffer.append(p.getWidth() + ", ");
			buffer.append(p.getHeight() + "));\n");
		}
		// DECLARE_EVENT
		for (int i = 0; i < IParts.eventName.length; i++) {
			if (target.getSupported(i) == true && target.getEvent(i) == true) {
				buffer.append("\t\tthis->add_" + IParts.eventName[i] + 
				"(new EventHandler<" + target.getName() + ">(this, &" + target.getName() + 
					"::On_" + IParts.eventName[i] + "));\n");
			}
		}
		// DECLARE_EVENT
		for (int j = 0; j < target.getComponentCount(); j++) {
			IParts p = (IParts)plist[j];
			for (int i = 0; i < IParts.eventName.length; i++) {
				if (p.getSupported(i) == true && p.getEvent(i) == true) {
					buffer.append("\t\tthis->" + p.getName() + "->add_" + IParts.eventName[i] + 
					"(new EventHandler<" + target.getName() + ">(this, &" + target.getName() + 
						"::On_" + IParts.eventName[i] + "));\n");
				}
			}
		}
		// Add
		for (int i = 0; i < target.getComponentCount(); i++) {
			IParts p = (IParts)plist[i];
			buffer.append("\t\tthis->get_Controls()->Add(this->" + p.getName() + ".get());\n");
		}
		buffer.append("\t}\n\n");
		
		//
		// event handlers
		//
		for (int i = 0; i < IParts.eventName.length; i++) {
			buffer.append("\t/** イベントハンドラ */\n");
			buffer.append("\tvoid On_" + IParts.eventName[i] + "(_P<Object> sender, _P<EventArgs> e) {\n");
			buffer.append("\t}\n\n");
		}
		
		//
		// protected methods
		//
		buffer.append("protected:\n");
		// OnPaint
		buffer.append("\t/** 描画ハンドラ */\n");
		buffer.append("\tvirtual void OnPaint() {\n");
		buffer.append("\t\tForm::OnPaint();\n");
		buffer.append("\t\t_P<Graphics> g = this->CreateGraphics();\n");
		for (int i = 0; i < target.getComponentCount(); i++) {
			IParts p = (IParts)plist[i];
			if (plist[i] instanceof PImage) {
				buffer.append("\t\tg->DrawImage(");
				buffer.append(p.getName() + ",");
				buffer.append((p.getX() - IParts.INSETS_LEFT) + ",");
				buffer.append((p.getY() - IParts.INSETS_TOP) + ");\n");
			}
		}
		buffer.append("\t\tg->Dispose();\n");
		buffer.append("\t}\n\n");
		
		// main
		buffer.append("public:\n");
		buffer.append("\tstatic void Main(_A<String> args) {\n");
		buffer.append("\t\tApplication::Run(new Form1());\n");
		buffer.append("\t}\n");
		buffer.append("};\n");
		buffer.append("\n");
		buffer.append("SET_MAIN_CLASS(Form1)\n");
		return buffer.toString();
	}

	/** ファイルを生成する */
	public void writeFile(PWindow target) {
		this.target = target;
		new File("monaforms").mkdir();
		writeFile("monaforms/" + target.getName() + ".cpp", getFile());
	}
	
	/** ファイルを生成する */
	private void writeFile(String name, String data) {
		FileOutputStream fos = null;
		try {
			fos = new FileOutputStream(name);
			fos.write(data.getBytes("UTF-8"));
			fos.flush();
		} catch (FileNotFoundException e) {
			System.out.println("Error: cant open " + name);
		} catch (UnsupportedEncodingException e) {
			System.out.println("Error: UTF-8 encoding not supported");
		} catch (IOException e) {
			System.out.println("Error: fatal I/O error occured ");
			e.printStackTrace();
		} finally {
			try {
				if (fos != null) {
					fos.close();
				}
			} catch (IOException ioe) {
				fos = null;
			}
		}
	}
}
