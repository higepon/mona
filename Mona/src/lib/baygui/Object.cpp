/*
Copyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "baygui.h"

/** コンストラクタ */
Object::Object() {
	threadID = THREAD_UNKNOWN;
}

/** デストラクタ */
Object::~Object() {
}

/** スレッドIDを設定する */
void Object::setThreadID(dword threadID)
{
	this->threadID = threadID;
}

/** スレッドIDを得る */
dword Object::getThreadID()
{
	return threadID;
}

/** ファイルを開きバイト列を得る */
unsigned char *Object::getByteArray(char *path)
{
#if defined(PEKOE)
	int file1, filesize1, readsize1;
	unsigned char *fp1;
	
	if ((file1 = open(path, O_RDONLY, 0)) >= 0) {
		filesize1 = lseek(file1, 0, 2);
		lseek(file1, 0, 0); // 先頭に戻す
		fp1 = (unsigned char *)malloc(filesize1);
		readsize1 = read(file1, (char *)fp1, filesize1);
		close(file1);
		return fp1;
	} else {
		return NULL;
	}
#elif defined(MONA)
	// bzip2 または tek5
	if (path[strlen(path) - 1] == '2' || path[strlen(path) - 1] == '5') {
		monapi_cmemoryinfo* mi = NULL;
		if (path[strlen(path) - 1] == '2') {
			mi = monapi_call_file_decompress_bz2_file(path, false);
		} else {
			mi = monapi_call_file_decompress_st5_file(path, false);
		}
		if (mi != NULL && mi->Size > 0) {
			unsigned char *data = (unsigned char *)malloc(mi->Size);
			memcpy(data, mi->Data, mi->Size);
			monapi_cmemoryinfo_dispose(mi);
			monapi_cmemoryinfo_delete(mi);
			return data;
		} else {
			return NULL;
		}
	} else {
		dword filesize1 = 0, readsize1 = 0;
		unsigned char *fp1;
		if (syscall_file_open(path, 0, &filesize1) == 0 && filesize1 > 0) {
			fp1 = (unsigned char *)malloc(filesize1);
			if (syscall_file_read((char*)fp1, filesize1, &readsize1) == 0) {
				syscall_file_close();
				return fp1;
			} else {
				return NULL;
			}
		} else {
			return NULL;
		}
	}
#endif
}

/** 文字列をコピーする */
void Object::copyString(char *dst, char *src)
{
	memset(dst, 0, strlen(dst));
	strcpy(dst, src);
	//int i;
	//for (i = 0; i < (int)strlen(src); i++) {
	//	dst[i] = src[i];
	//}
	//dst[i] = '\0';
}

/** 10進数を文字列に変換する */
void Object::toDecimalString(int n, char *s)
{
	int k, i = 0, j;
	char tmp;
	
	k = 0;
	
	if (n < 0) {
		n = -1 * n;
		i = 1;
	}
	if (n == 0) {
		s[k++] = 0 + '0';
	}
	while (n != 0) {
		s[k++] = n%10 + '0';
		n = n/10;
	}
	if (i == 1) s[k++] = '-';
	s[k] = '\0';

	i = j = 0;
	while (s[j]) ++j;
	while (i < --j) {
		tmp = s[i];
		s[i++] = s[j];
		s[j] = tmp;
	}
}
