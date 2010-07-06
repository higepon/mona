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

#include <monapi.h>
#include <monapi/messages.h>

#include "messages.h"

#define IMESERVER_NAME "IME.EX5"

/** かな→漢字変換 */
void getKanji(char *str) {
    dword imesvrID = MonAPI::Message::lookupMainThread(IMESERVER_NAME);
    if (imesvrID != THREAD_UNKNOWN) {
        MessageInfo info;
        printf("IMETEST: connected %s\n", IMESERVER_NAME);
        MonAPI::Message::sendReceive(&info, imesvrID, MSG_IMESERVER_GETKANJI, 0, 0, 0, str);
        if (info.arg2 > 0) {
            bool flag = true;
            while (flag) {
                if (!MonAPI::Message::receive(&info)) {
                    switch(info.header) {
                    case MSG_IMESERVER_STARTKANJI:
                        printf("MSG_IMESERVER_START_GETKANJI\n");
                        MonAPI::Message::reply(&info);
                        break;
                    case MSG_IMESERVER_KANJI:
                        printf("%d: %s\n", info.arg2, info.str);
                        MonAPI::Message::reply(&info);
                        break;
                    case MSG_IMESERVER_ENDKANJI:
                        printf("MSG_IMESERVER_END_GETKANJI\n");
                        MonAPI::Message::reply(&info);
                        flag = false;
                        break;
                    }
                }
            }
        }
    }
}

/** 漢字→かな逆変換 */
void getYomi(char *str) {
    dword imesvrID = MonAPI::Message::lookupMainThread(IMESERVER_NAME);
    if (imesvrID != THREAD_UNKNOWN) {
        MessageInfo info;
        printf("IMETEST: connected %s\n", IMESERVER_NAME);
        MonAPI::Message::sendReceive(&info, imesvrID, MSG_IMESERVER_GETYOMI, 0, 0, 0, str);
        if (info.arg2 > 0) {
            printf("MSG_IMESERVER_GETYOMI: %s -> %s\n", str, info.str);
        } else {
            printf("MSG_IMESERVER_GETYOMI: NG\n");
        }
    }
}

/** ローマ字→かな変換 */
void getKana(char *str) {
    dword imesvrID = MonAPI::Message::lookupMainThread(IMESERVER_NAME);
    if (imesvrID != THREAD_UNKNOWN) {
        MessageInfo info;
        printf("IMETEST: connected %s\n", IMESERVER_NAME);
        MonAPI::Message::sendReceive(&info, imesvrID, MSG_IMESERVER_GETKANA, 0, 0, 0, str);
        if (info.arg2 > 0) {
            printf("MSG_IMESERVER_GETKANA: %s -> %s\n", str, info.str);
        } else {
            printf("MSG_IMESERVER_GETKANA: NG\n");
        }
    }
}

/** かな→ローマ字変換 */
void getRoman(char *str) {
    dword imesvrID = MonAPI::Message::lookupMainThread(IMESERVER_NAME);
    if (imesvrID != THREAD_UNKNOWN) {
        MessageInfo info;
        printf("IMETEST: connected %s\n", IMESERVER_NAME);
        MonAPI::Message::sendReceive(&info, imesvrID, MSG_IMESERVER_GETROMAN, 0, 0, 0, str);
        if (info.arg2 > 0) {
            printf("MSG_IMESERVER_GETROMAN: %s -> %s\n", str, info.str);
        } else {
            printf("MSG_IMESERVER_GETROMAN: NG\n");
        }
    }
}

int MonaMain(List<char*>* pekoe)
{
    getKanji("あ");
    getKanji("わんりょく");
    getYomi("逢");
    getYomi("腕力");
    getKana("!");
    getKana("zzyu");
    getRoman("！");
    getRoman("っじゅ");

    return 0;
}
