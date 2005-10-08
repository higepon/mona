#include "sms_ptr_dict.h"
#include <stdlib.h>
#include <string.h>


sms_ptr_dict_memory::sms_ptr_dict_memory(void* addr, int size, int type)
	: addr(addr), size(size), type(type), mark(false) {
}


sms_ptr_dict_linear::sms_ptr_dict_linear() {
	this->length = 0;
	this->size = 16;
	this->ptrs = (sms_ptr_dict_memory*)malloc(this->size * sizeof(sms_ptr_dict_memory));
}

sms_ptr_dict_linear::~sms_ptr_dict_linear() {
	free(this->ptrs);
}

int sms_ptr_dict_linear::get_index(void* addr) {
	sms_ptr_dict_memory* p = this->ptrs;
	for (int i = 0; i < this->length; i++, p++)
		if (p->addr == addr) return i;
	return -1;
}

sms_ptr_dict_memory* sms_ptr_dict_linear::get_data(int index) {
	if (index < 0 || index >= this->length) return NULL;
	return &this->ptrs[index];
}

sms_ptr_dict_memory* sms_ptr_dict_linear::get_data(void* addr) {
	return this->get_data(this->get_index(addr));
}

void sms_ptr_dict_linear::add(void* addr, int size, int type) {
	if (this->length == this->size) {
		this->size *= 2;
		this->ptrs = (sms_ptr_dict_memory*)realloc(this->ptrs, this->size * sizeof(sms_ptr_dict_memory));
	}
	this->ptrs[this->length++] = sms_ptr_dict_memory(addr, size, type);
}

bool sms_ptr_dict_linear::remove_at(int index) {
	if (index < 0 || index >= this->length) return false;
	sms_ptr_dict_memory* p = &this->ptrs[index];
	this->length--;
	memcpy(p, p + 1, (this->length - index) * sizeof(sms_ptr_dict_memory));
	return true;
}

bool sms_ptr_dict_linear::remove(void* addr) {
	return this->remove_at(this->get_index(addr));
}

void sms_ptr_dict_linear::unmark() {
	sms_ptr_dict_memory* p = this->ptrs;
	for (int i = 0; i < this->length; i++, p++) p->mark = false;
}


sms_ptr_dict::sms_ptr_dict() : length(0), it1(0), it2(0), it3(0) {
	memset(this->ptrs, 0, 256 * sizeof(sms_ptr_dict_linear***));
}

sms_ptr_dict_memory* sms_ptr_dict::get_data(void* addr) {
	unsigned int a1 =  ((unsigned int)addr) >> 24;
	unsigned int a2 = (((unsigned int)addr) >> 16) & 0xff;
	unsigned int a3 = (((unsigned int)addr) >>  8) & 0xff;
	sms_ptr_dict_linear*** p1 = this->ptrs[a1];
	if (p1 == NULL) return NULL;
	sms_ptr_dict_linear** p2 = p1[a2];
	if (p2 == NULL) return NULL;
	sms_ptr_dict_linear* p3 = p2[a3];
	if (p3 == NULL) return NULL;
	return p3->get_data(addr);
}

void sms_ptr_dict::add(void* addr, int size, int type) {
	unsigned int a1 =  ((unsigned int)addr) >> 24;
	unsigned int a2 = (((unsigned int)addr) >> 16) & 0xff;
	unsigned int a3 = (((unsigned int)addr) >>  8) & 0xff;
	sms_ptr_dict_linear*** p1 = this->ptrs[a1];
	if (p1 == NULL) p1 = this->ptrs[a1] = (sms_ptr_dict_linear***)calloc(256, sizeof(sms_ptr_dict_linear**));
	sms_ptr_dict_linear** p2 = p1[a2];
	if (p2 == NULL) p2 = p1[a2] = (sms_ptr_dict_linear**)calloc(256, sizeof(sms_ptr_dict_linear*));
	sms_ptr_dict_linear* p3 = p2[a3];
	if (p3 == NULL) p3 = p2[a3] = new sms_ptr_dict_linear();
	p3->add(addr, size, type);
	this->length++;
}

bool sms_ptr_dict::remove(void* addr) {
	unsigned int a1 =  ((unsigned int)addr) >> 24;
	unsigned int a2 = (((unsigned int)addr) >> 16) & 0xff;
	unsigned int a3 = (((unsigned int)addr) >>  8) & 0xff;
	sms_ptr_dict_linear*** p1 = this->ptrs[a1];
	if (p1 == NULL) return false;
	sms_ptr_dict_linear** p2 = p1[a2];
	if (p2 == NULL) return false;
	sms_ptr_dict_linear* p3 = p2[a3];
	if (p3 == NULL || !p3->remove(addr)) return false;
	if (p3->get_length() == 0) {
		p2[a3] = NULL;
		delete p3;
	}
	this->length--;
	return true;
}

sms_ptr_dict_linear* sms_ptr_dict::get_first() {
	for (this->it1 = 0; this->it1 < 256; this->it1++) {
		sms_ptr_dict_linear*** p1 = this->ptrs[this->it1];
		if (p1 == NULL) continue;
		for (this->it2 = 0; this->it2 < 256; this->it2++) {
			sms_ptr_dict_linear** p2 = p1[this->it2];
			if (p2 == NULL) continue;
			for (this->it3 = 0; this->it3 < 256; this->it3++) {
				sms_ptr_dict_linear* p3 = p2[this->it3];
				if (p3 != NULL) return p3;
			}
		}
	}
	return NULL;
}

sms_ptr_dict_linear* sms_ptr_dict::get_current() {
	if (this->it1 == 256) return NULL;
	sms_ptr_dict_linear*** p1 = this->ptrs[this->it1];
	if (p1 == NULL) return NULL;
	sms_ptr_dict_linear** p2 = p1[this->it2];
	if (p2 == NULL) return NULL;
	return p2[this->it3];
}

sms_ptr_dict_linear* sms_ptr_dict::get_next() {
	if (this->it1 == 256) return NULL;
	sms_ptr_dict_linear*** p1 = this->ptrs[this->it1];
	sms_ptr_dict_linear** p2 = p1[this->it2];
	sms_ptr_dict_linear* p3 = NULL;
	while (p3 == NULL) {
		this->it3++;
		if (this->it3 == 256) {
			this->it3 = 0;
			p2 = NULL;
			while (p2 == NULL) {
				this->it2++;
				if (this->it2 == 256) {
					this->it2 = 0;
					p1 = NULL;
					while (p1 == NULL) {
						this->it1++;
						if (this->it1 == 256) return NULL;
						p1 = this->ptrs[this->it1];
					}
				}
				p2 = p1[this->it2];
			}
		}
		p3 = p2[this->it3];
	}
	return p3;
}


#ifdef SMS_USE_64K
sms_ptr_dict_64k::sms_ptr_dict_64k() : length(0), it1(0), it2(0) {
	memset(this->ptrs, 0, 256 * sizeof(sms_ptr_dict_linear**));
}

sms_ptr_dict_memory* sms_ptr_dict_64k::get_data(void* addr) {
	unsigned int a1 =  ((unsigned int)addr) >> 24;
	unsigned int a2 = (((unsigned int)addr) >> 16) & 0xff;
	sms_ptr_dict_linear** p1 = this->ptrs[a1];
	if (p1 == NULL) return NULL;
	sms_ptr_dict_linear* p2 = p1[a2];
	if (p2 == NULL) return NULL;
	return p2->get_data(addr);
}

void sms_ptr_dict_64k::add(void* addr, int size, int type) {
	unsigned int a1 =  ((unsigned int)addr) >> 24;
	unsigned int a2 = (((unsigned int)addr) >> 16) & 0xff;
	sms_ptr_dict_linear** p1 = this->ptrs[a1];
	if (p1 == NULL) p1 = this->ptrs[a1] = (sms_ptr_dict_linear**)calloc(256, sizeof(sms_ptr_dict_linear*));
	sms_ptr_dict_linear* p2 = p1[a2];
	if (p2 == NULL) p2 = p1[a2] = new sms_ptr_dict_linear();
	p2->add(addr, size, type);
	this->length++;
}

bool sms_ptr_dict_64k::remove(void* addr) {
	unsigned int a1 =  ((unsigned int)addr) >> 24;
	unsigned int a2 = (((unsigned int)addr) >> 16) & 0xff;
	sms_ptr_dict_linear** p1 = this->ptrs[a1];
	if (p1 == NULL) return false;
	sms_ptr_dict_linear* p2 = p1[a2];
	if (p2 == NULL || !p2->remove(addr)) return false;
	if (p2->get_length() == 0) {
		p1[a2] = NULL;
		delete p2;
	}
	this->length--;
	return true;
}

sms_ptr_dict_linear* sms_ptr_dict_64k::get_first() {
	for (this->it1 = 0; this->it1 < 256; this->it1++) {
		sms_ptr_dict_linear** p1 = this->ptrs[this->it1];
		if (p1 == NULL) continue;
		for (this->it2 = 0; this->it2 < 256; this->it2++) {
			sms_ptr_dict_linear* p2 = p1[this->it2];
			if (p2 != NULL) return p2;
		}
	}
	return NULL;
}

sms_ptr_dict_linear* sms_ptr_dict_64k::get_current() {
	if (this->it1 == 256) return NULL;
	sms_ptr_dict_linear** p1 = this->ptrs[this->it1];
	if (p1 == NULL) return NULL;
	return p1[this->it2];
}

sms_ptr_dict_linear* sms_ptr_dict_64k::get_next() {
	if (this->it1 == 256) return NULL;
	sms_ptr_dict_linear** p1 = this->ptrs[this->it1];
	sms_ptr_dict_linear* p2 = NULL;
	while (p2 == NULL) {
		this->it2++;
		if (this->it2 == 256) {
			this->it2 = 0;
			p1 = NULL;
			while (p1 == NULL) {
				this->it1++;
				if (this->it1 == 256) return NULL;
				p1 = this->ptrs[this->it1];
			}
		}
		p2 = p1[this->it2];
	}
	return p2;
}
#endif
