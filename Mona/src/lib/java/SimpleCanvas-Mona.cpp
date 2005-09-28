#include <gcj/javaprims.h>
#include "SimpleCanvas.h"
#include <gcj/cni.h>
#include <monapi.h>

static MonAPI::Screen screen;
static jint* buffer = NULL;
static int width = 0, height = 0;

static char* get_string(jstring s) {
	int len = s->length();
	char* cstr = new char[len + 1];
	jchar* ss = JvGetStringChars(s);
	for (int i = 0; i < len; i++)
		cstr[i] = (char)ss[i];
	cstr[len] = '\0';
	return cstr;
}

jboolean SimpleCanvas::create(jintArray buf, jstring title, jint w, jint h) {
	buffer = elements(buf);
	width  = w;
	height = h;
	//char* t = get_string(title);
	//delete [] t;
	return true;
}

jboolean SimpleCanvas::doEvents() {
	//for(int y = 0; y < height; y++) {
	//	for(int x = 0; x < width; x++) {
	//		screen.putPixel16(x, y, buffer[y * width + x]);
	//	}
	//}
	byte* vram = screen.getVRAM();
	int bpp = screen.getBpp();
	int sw  = screen.getWidth();
	int bypp = bpp >> 3;
	
	for (int y = 0; y < height; y++) {
		jint* pBuff = &buffer[y * width];
		byte* pVram = &vram[y * sw * bypp];
		for (int x = 0; x < width; x++, pBuff++, pVram += bypp) {
			byte* p = (byte*)pBuff;
			switch (bpp) {
				case 8: // broken
					*pVram = (p[0] + p[1] + p[2]) / 3;
					break;
				case 16: // 565
					*(unsigned short*)pVram = MonAPI::Color::bpp24to565(p);
					break;
				case 24:
					pVram[0] = p[0];
					pVram[1] = p[1];
					pVram[2] = p[2];
					break;
				case 32:
					*(jint*)pVram = *pBuff;
					break;
			}
		}
	}
	return true;
}

void SimpleCanvas::invalidate() {
}
