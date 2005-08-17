#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>

using namespace MonAPI;

int OpenSlide(List<CString>* list, int i)
{
	if (list->get(i).endsWith(".jpg")) {
		// 画像を開く
		CString cstr = "/APPS/JPEGDEMO.EX2 ";
		cstr += System::getBundlePath();
		cstr += "/";
		cstr += list->get(i);
		syscall_set_cursor(0,0);
		return monapi_call_process_execute_file((const char*)cstr, MONAPI_FALSE);
	} else if (list->get(i).endsWith(".mpg")) {
		// 動画を開く
		CString cstr = "/APPS/MONAPEG.EX2 ";
		cstr += System::getBundlePath();
		cstr += "/";
		cstr += list->get(i);
		syscall_set_cursor(0,0);
		return monapi_call_process_execute_file((const char*)cstr, MONAPI_FALSE);
	}
}

int MonaMain(List<char*>* argv)
{
	// 設定ファイル名
	CString filepath = System::getBundlePath();
	filepath += "/MONAPPT.INI";
	
	// ファイル読み込み
	printf("loading %s...\n", (const char*)filepath);
	monapi_cmemoryinfo* mi = monapi_call_file_read_data((const char*)filepath, MONAPI_FALSE);
	if (mi == NULL || mi->Size == 0) {
		printf("file read error\n");
		return(-1);
	}
	unsigned int filesize = mi->Size;
	unsigned char* filebuff = (unsigned char*)malloc(mi->Size);
	if (filebuff == NULL) {
		printf("memory allocate error\n");
		monapi_cmemoryinfo_dispose(mi);
		monapi_cmemoryinfo_delete(mi);
		return(-1);
	}
	memcpy(filebuff, mi->Data, mi->Size);
	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);
	
	// ファイル解析
	HList<CString> list;
	unsigned char temp[20];
	memset(temp, '\0', sizeof(temp));
	for (int i = 0; i < (int)filesize; i++) {
		if (filebuff[i] == '\r') {
			// do nothing
		} else if (filebuff[i] == '\n' || i == (int)filesize - 1) {
			list.add((const char*)temp);
			memset(temp, '\0', sizeof(temp));
		} else {
			temp[strlen((const char*)temp)] = filebuff[i];
		}
	}
	
	// キーサーバーに登録する
	syscall_clear_screen();
	syscall_set_cursor(0,0);
	if (!monapi_register_to_server(ID_KEYBOARD_SERVER, MONAPI_TRUE)) exit(1);
	
	// １枚目のスライド
	int slideno = 0;
	OpenSlide(&list, slideno);
	
	// メッセージループ
	for (MessageInfo info;;)
	{
		if (MonAPI::Message::receive(&info) != 0) continue;
		
		if (info.header == MSG_KEY_VIRTUAL_CODE) {
			int keycode  = info.arg1;
			int modcode  = info.arg2;
			int charcode = info.arg3;
			if ((modcode & KEY_MODIFIER_DOWN) == KEY_MODIFIER_DOWN) {
				if (keycode == Keys::Escape) {
					break;
				} else if (keycode == Keys::Enter) {
					// 次のスライド
					if (slideno == list.size() - 1) {
						OpenSlide(&list, list.size() - 1);
					} else {
						OpenSlide(&list, ++slideno);
					}
				} else if (keycode == Keys::Back) {
					// 前のスライド
					if (slideno == 0) {
						OpenSlide(&list, 0);
					} else {
						OpenSlide(&list, --slideno);
					}
				}
			}
		}
	}
	
	// キーサーバーから登録解除する
	monapi_register_to_server(ID_KEYBOARD_SERVER, MONAPI_FALSE);
	syscall_clear_screen();
	syscall_set_cursor(0,0);
	
	free(filebuff);
	return(0);
}
