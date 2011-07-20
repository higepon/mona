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

#include <monagui.h>

#include "./link_button.h"
#include "./facebook_service.h"

namespace facebook {

LinkButton::LinkButton() : facebook::Button("") {
}

void LinkButton::set_url(const std::string& url) {
    url_ = url;
    FontMetrics* fm = getFontMetrics();
    int url_width = fm->getWidth(url.c_str());
    int width = getWidth();
    if (url_width < width) {
      setLabelNoRepaint(url.c_str());
    } else {
      for (size_t i = url.size() - 1; i > 0; i--) {
        std::string sub_url = url.substr(0, i);
        int url_width = fm->getWidth(sub_url.c_str());
        if (url_width < width) {
          setLabelNoRepaint(sub_url.c_str());
          break;
        }
      }
    }
}

void LinkButton::processEvent(Event* event) {
  if (event->getSource() == this &&
      event->getType() == MouseEvent::MOUSE_RELEASED) {
    OpenBrowser(url_);
  }
}

void LinkButton::OpenBrowser(const std::string& url) {
  std::string command = "/APPS/W3M.APP/W3M.EX5 ";
  command += "\"";
  command += url;
  command += "\"";
  bool waits = false;
  FacebookService::ExecuteCommand(command, waits);
}
}
