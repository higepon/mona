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
IN NO KEYEVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "baygui.h"

CheckboxGroup::CheckboxGroup()
{
	this->checkboxList = new LinkedList();
}

CheckboxGroup::~CheckboxGroup()
{
	delete(this->checkboxList);
}

void CheckboxGroup::add(Checkbox *check)
{
	// NULLチェック
	if (check == NULL) return;
	
	check->setCheckboxGroup(this);
	this->checkboxList->add(check);
}

Checkbox *CheckboxGroup::getSelectedCheckbox()
{
	for (int i = 0; i < this->checkboxList->getLength(); i++) {
		Checkbox *chk = (Checkbox *)this->checkboxList->get(i);
		if (chk->getChecked() == true) return chk;
	}
	return NULL;
}

void CheckboxGroup::onEvent(Event *e)
{
	// 選択されているチェックボックス以外を未選択にする
	for (int i = 0; i < this->checkboxList->getLength(); i++) {
		Checkbox *chk = (Checkbox *)this->checkboxList->get(i);
		if (e->getSource() != chk) {
			chk->setChecked(false);
		}
	}
}
