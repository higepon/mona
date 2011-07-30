/*
 *   Copyright (c) 2011  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <monapi.h>
#include <stdlib.h>
#include <string>

#include "./Str.h"
#include "./textlist.h"

#define TOKEN_FILE_PATH "/USER/FB.TOKEN"

extern "C" int ISFacebookOAuthResultUrl(const char* u) {
  std::string url(u);
  if (url.find("http://lovamimi.com/login_monaos.scm?code=")
      == std::string::npos) {
    return 0;
  } else {
    return 1;
  }
}

extern "C" void w3m_exit(int s);

extern "C" void SaveFacebookToken(TextLineListItem* item) {
  FILE* fp = fopen(TOKEN_FILE_PATH, "w");
  if (fp == NULL) {
    monapi_warn("open failed fb.token");
    return;
  }
  for (; item != NULL; item = item->next) {
    fwrite(item->ptr->line->ptr, 1, item->ptr->line->length, fp);
  }
  fclose(fp);
  w3m_exit(0);
}
