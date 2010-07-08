/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <servers/ime.h>

#include "ImeServer.h"
#include "rule_roma.h"

ImeServer::ImeServer()
{
    basicDic = NULL;
    basicDicSize = 0;
}

ImeServer::~ImeServer()
{
    free(basicDic);
}

/** 辞書ロード */
bool ImeServer::loadDictionary()
{
    monapi_cmemoryinfo* mi = NULL;

    // 辞書を開く
//    mi = monapi_call_file_decompress_st5_file(BASICDIC_NAME, MONAPI_FALSE);
    mi = monapi_file_read_all(BASICDIC_NAME);
    basicDicSize = mi->Size;
    if (mi != NULL && basicDicSize > 0) {
        basicDic = (char *)malloc(basicDicSize);
        memcpy(basicDic, mi->Data, basicDicSize);
        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
        //printf("IME: Basic dictionary loaded (%dbyte)\n", basicDicSize);
        return true;
    } else {
        printf("IME: Basic dictionary not found\n");
        return false;
    }
}

/**
 よみ→漢字変換
*/
int ImeServer::getKanji(char *yomi, HList<MonAPI::CString>* result)
{
    int i, j, srcPtr, dstPtr;
    char src[MAX_TEXT_LEN], dst[MAX_TEXT_LEN];
    bool srcRead = false;

    // 読みと漢字を得る
    srcPtr = dstPtr = 0;
    memset(src, 0, MAX_TEXT_LEN);
    memset(dst, 0, MAX_TEXT_LEN);
    for (i = 0; i < basicDicSize; i++) {
        if (basicDic[i] != '\t' && srcRead == false) {
            src[srcPtr++] = basicDic[i];
        } else if (basicDic[i] == '\t') {
            dstPtr = 0;
            memset(dst, 0, MAX_TEXT_LEN);
            srcRead = true;
        } else if (basicDic[i] != '\n' && srcRead == true) {
            dst[dstPtr++] = basicDic[i];
        } else if (basicDic[i] == '\n') {
            if (strncmp(src, yomi, strlen(src)) == 0) {
                // 漢字を追加
                int j0 = strlen(dst);
                for (j = 0; j < (int)strlen(yomi) - (int)strlen(src); j++) {
                    dst[j + j0] = yomi[j + strlen(src)];
                }
                result->add(dst);
            }
            srcPtr = 0;
            memset(src, 0, MAX_TEXT_LEN);
            srcRead = false;
        }
    }
    // 最後によみを追加する
    result->add(yomi);
    return result->size();
}

/**
 漢字→よみ逆変換
*/
bool ImeServer::getYomi(char *kanji, char *result)
{
    int i, srcPtr, dstPtr;
    char src[MAX_TEXT_LEN], dst[MAX_TEXT_LEN];
    bool srcRead = false;

    // 読みと漢字を得る
    srcPtr = dstPtr = 0;
    memset(src, 0, MAX_TEXT_LEN);
    memset(dst, 0, MAX_TEXT_LEN);
    for (i = 0; i < basicDicSize; i++) {
        if (basicDic[i] != '\t' && srcRead == false) {
            src[srcPtr++] = basicDic[i];
        } else if (basicDic[i] == '\t') {
            dstPtr = 0;
            memset(dst, 0, MAX_TEXT_LEN);
            srcRead = true;
        } else if (basicDic[i] != '\n' && srcRead == true) {
            dst[dstPtr++] = basicDic[i];
        } else if (basicDic[i] == '\n') {
            if (strcmp(dst, kanji) == 0) {
                strncpy(result, src, MAX_TEXT_LEN);
                return true;
            }
            srcPtr = 0;
            memset(src, 0, MAX_TEXT_LEN);
            srcRead = false;
        }
    }
    return false;
}

/**
 入力文字→かな変換
 @return マッチしたら「かな」、マッチしなかったら「NULL」
 */
bool ImeServer::getKana(char *inputString, char *result)
{
    // デフォルトはローマ字入力
    for (int i = 0; i < MAX_RULEROMA_LEN; i++) {
        if (strcmp(ruleRomaSrc[i], inputString) == 0) {
            strncpy(result, ruleRomaDst[i], MAX_TEXT_LEN);
            return true;
        }
    }
    return false;
}

/**
 かな→入力文字逆変換
 @return マッチしたら「入力文字」、マッチしなかったら「NULL」
 */
bool ImeServer::getRoman(char *kana, char *result)
{
    // デフォルトはローマ字入力
    for (int i = 0; i < MAX_RULEROMA_LEN; i++) {
        if (strcmp(ruleRomaDst[i], kana) == 0) {
            strncpy(result, ruleRomaSrc[i], MAX_TEXT_LEN);
            return true;
        }
    }
    return false;
}

/** メインスレッド */
void ImeServer::service()
{
    uintptr_t targetID = MonAPI::Message::lookupMainThread("MONITOR.BIN");
    if (targetID == THREAD_UNKNOWN) {
        printf("IME: Monitor server not found\n");
        exit(1);
    }

    if (MonAPI::Message::send(targetID, MSG_SERVER_START_OK, 0, 0, 0, NULL)) {
        printf("IME: Server start failed\n");
        exit(1);
    }

    // 辞書が読めなかったときはかな漢字変換はすべて失敗を返す
    bool dicLoaded = loadDictionary();

    MessageInfo info;
    while (1) {
        if (!MonAPI::Message::receive(&info)) {
            switch(info.header) {
            case MSG_IMESERVER_GETKANJI:
                {
                    HList<MonAPI::CString> result;
                    int hit = getKanji(info.str, &result);

                    // MSG_IMESERVER_GETKANJI
                    // arg2: ヒット数
                    if (dicLoaded == true) {
                        MonAPI::Message::reply(&info, hit, 0, NULL);
                    } else {
                        MonAPI::Message::reply(&info, 0, 0, NULL);
                    }

                    if (dicLoaded == true && hit > 0) {
                        // MSG_IMESERVER_STARTKANJI
                        // arg1: ヒット数
                        MonAPI::Message::sendReceive(&info, info.from, MSG_IMESERVER_STARTKANJI, hit, 0, 0, NULL);

                        for (int i = 0; i < hit; i++) {
                            // MSG_IMESERVER_KANJI
                            // arg1: ヒット数
                            // arg2: カウンタ
                            // str : よみ→漢字
                            MonAPI::Message::sendReceive(&info, info.from, MSG_IMESERVER_KANJI, hit, i, 0, (const char*)result.get(i));
                            //printf("%d: %s\n", i, (const char*)result.get(i));
                        }

                        // MSG_IMESERVER_ENDKANJI
                        // arg1: ヒット数
                        MonAPI::Message::sendReceive(&info, info.from, MSG_IMESERVER_ENDKANJI, hit, 0, 0, NULL);
                    }
                }
                break;
            case MSG_IMESERVER_GETYOMI:
                {
                    // MSG_IMESERVER_GETYOMI
                    // arg2: 0-失敗 1-成功
                    // str : 漢字→よみ
                    char result[MAX_TEXT_LEN];
                    if (dicLoaded == true && getYomi(info.str, result) == true) {
                        strncpy(info.str, result, MAX_TEXT_LEN);
                        MonAPI::Message::reply(&info, 1, 0, result);
                    } else {
                        strncpy(info.str, "", MAX_TEXT_LEN);
                        MonAPI::Message::reply(&info, 0, 0, "");
                    }
                }
                break;
            case MSG_IMESERVER_GETKANA:
                {
                    // MSG_IMESERVER_GETKANA
                    // arg2: 0-失敗 1-成功
                    // str : 入力文字→かな
                    char result[MAX_TEXT_LEN];
                    if (getKana(info.str, result) == true) {
                        strncpy(info.str, result, MAX_TEXT_LEN);
                        MonAPI::Message::reply(&info, 1, 0, result);
                    } else {
                        strncpy(info.str, "", MAX_TEXT_LEN);
                        MonAPI::Message::reply(&info, 0, 0, "");
                    }
                }
                break;
            case MSG_IMESERVER_GETROMAN:
                {
                    // MSG_IMESERVER_GETROMAN
                    // arg2: 0-失敗 1-成功
                    // str : かな→入力文字
                    char result[MAX_TEXT_LEN];
                    if (getRoman(info.str, result) == true) {
                        strncpy(info.str, result, MAX_TEXT_LEN);
                        MonAPI::Message::reply(&info, 1, 0, result);
                    } else {
                        strncpy(info.str, "", MAX_TEXT_LEN);
                        MonAPI::Message::reply(&info, 0, 0, "");
                    }
                }
                break;
            }
        }
    }
}
