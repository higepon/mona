package guibuilder;

import java.awt.*;
import java.io.*;

/**
 ファイル保存クラス。
 ここを変えればC++以外にも対応可能。
 */
public class FileMaker {
	/** 保存対象 */
	private PWindow target;
	
	/** コンストラクタ */
	public FileMaker() {
	}
	
	/** Form1.cpp */
	private String getFile() {
		StringBuffer buffer = new StringBuffer();
		//
		// include
		//
		buffer.append("#include <baygui.h>\n\n");
		
		// class 定義
		buffer.append("class " + target.getName() + " : public Window {\n");
		
		//
		// private fields
		//
		buffer.append("private:\n");
		Component[] plist = target.getComponents();
		for (int i = 0; i < target.getComponentCount(); i++) {
			IParts p = (IParts)plist[i];
			buffer.append("\t" + p.getClassName() + " *" + p.getName() + ";\n");
		}
		buffer.append("\n");
		
		//
		// public methods
		//
		buffer.append("public:\n");
		// constructor
		buffer.append("\t" + target.getName() + "() {\n");
		// setRect
		buffer.append("\t\tsetRect(");
		buffer.append(target.getX() + ", ");
		buffer.append(target.getY() + ", ");
		buffer.append(target.getWidth() + ", ");
		buffer.append(target.getHeight() + ");\n");
		// setTitle
		buffer.append("\t\tsetTitle(\"" + target.getCaption() + "\");\n");
		// constructor
		for (int i = 0; i < target.getComponentCount(); i++) {
			IParts p = (IParts)plist[i];
			buffer.append("\t\t" + p.getName() + " = new " + p.getClassName() + "(" + p.getArgs() + ");\n");
		}
		// setRect
		for (int i = 0; i < target.getComponentCount(); i++) {
			IParts p = (IParts)plist[i];
			buffer.append("\t\t" + p.getName() + "->setRect(");
			buffer.append((p.getX() - IParts.INSETS_LEFT) + ", ");
			buffer.append((p.getY() - IParts.INSETS_TOP) + ", ");
			buffer.append(p.getWidth() + ", ");
			buffer.append(p.getHeight() + ");\n");
		}
		// setText
		for (int i = 0; i < target.getComponentCount(); i++) {
			IParts p = (IParts)plist[i];
			if (p instanceof PTextField) {
				buffer.append("\t\t" + p.getName() + "->setText(\"" + p.getCaption() + "\");\n");
			}
		}
		// add
		for (int i = 0; i < target.getComponentCount(); i++) {
			IParts p = (IParts)plist[i];
			buffer.append("\t\tadd(" + p.getName() + ");\n");
		}
		buffer.append("\t}\n\n");
		
		//
		// destructor
		//
		buffer.append("\t~" + target.getName() + "() {\n");
		buffer.append("\t}\n\n");
		
		//
		// onEvent
		//
		buffer.append("\t/** イベントハンドラ */\n");
		buffer.append("\tvirtual void onEvent(Event *e) {\n");
		buffer.append("\t\tswitch(e->type) {\n");
		for (int i = 0; i < IParts.eventName.length; i++) {
			buffer.append("\t\tcase " + IParts.eventName[i] + ":\n");
			buffer.append("\t\t\tif (e->source == this) {\n");
			for (int j = 0; j < target.getComponentCount(); j++) {
				IParts p = (IParts)plist[j];
				if (p.getSupported(i) == true && p.getEvent(i) == true) {
					buffer.append("\t\t\t} else if (e->source == " + p.getName() + ") {\n");
				}
			}
			buffer.append("\t\t\t}\n");
			buffer.append("\t\t\tbreak;\n");
		}
		buffer.append("\t\tdefault:\n");
		buffer.append("\t\t\tbreak;\n");
		buffer.append("\t\t}\n");
		buffer.append("\t}\n\n");
		
		//
		// onPaint
		//
		buffer.append("\t/** 描画ハンドラ */\n");
		buffer.append("\tvirtual void onPaint(Graphics *g) {\n");
		// drawImage
		for (int i = 0; i < target.getComponentCount(); i++) {
			IParts p = (IParts)plist[i];
			if (plist[i] instanceof PImage) {
				buffer.append("\t\tg->drawImage(");
				buffer.append(p.getName() + ", ");
				buffer.append((p.getX() - IParts.INSETS_LEFT) + ", ");
				buffer.append((p.getY() - IParts.INSETS_TOP) + ");\n");
			}
		}
		buffer.append("\t}\n");
		buffer.append("};\n\n");
		
		// main
		buffer.append("/** エントリーポイント */\n");
		buffer.append("int MonaMain(List<char*>* pekoe) {\n");
		buffer.append("\t" + target.getName() + " *window = new " + target.getName() + "();\n");
		buffer.append("\twindow->run();\n");
		buffer.append("\tdelete(window);\n");
		buffer.append("\treturn 0;\n");
		buffer.append("}\n");
		return buffer.toString();
	}

	/** ファイルを生成する */
	public void writeFile(PWindow target) {
		this.target = target;
		new File("baygui").mkdir();
		writeFile("baygui/" + target.getName() + ".cpp", getFile());
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
