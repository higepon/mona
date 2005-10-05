#ifndef __SMS_PTR_DICT_H__
#define __SMS_PTR_DICT_H__

struct sms_ptr_dict_memory {
	void* addr;
	int size, type;
	bool mark;
	sms_ptr_dict_memory(void* addr, int size, int type);
};

class sms_ptr_dict_linear {
private:
	sms_ptr_dict_memory* ptrs;
	int length, size;

public:
	sms_ptr_dict_linear();
	~sms_ptr_dict_linear();
	inline int get_length() { return this->length; }
	int get_index(void* addr);
	sms_ptr_dict_memory* get_data(int index);
	sms_ptr_dict_memory* get_data(void* addr);
	void add(void* addr, int size, int type);
	bool remove_at(int index);
	bool remove(void* addr);
	void unmark();
};

class sms_ptr_dict {
private:
	sms_ptr_dict_linear*** ptrs[256];
	int length, it1, it2, it3;

public:
	sms_ptr_dict();
	inline int get_length() { return this->length; }
	sms_ptr_dict_memory* get_data(void* addr);
	void add(void* addr, int size, int type);
	bool remove(void* addr);
	sms_ptr_dict_linear* get_first();
	sms_ptr_dict_linear* get_current();
	sms_ptr_dict_linear* get_next();
};

#ifdef SMS_USE_64K
class sms_ptr_dict_64k {
private:
	sms_ptr_dict_linear** ptrs[256];
	int length, it1, it2;

public:
	sms_ptr_dict_64k();
	inline int get_length() { return this->length; }
	sms_ptr_dict_memory* get_data(void* addr);
	void add(void* addr, int size, int type);
	bool remove(void* addr);
	sms_ptr_dict_linear* get_first();
	sms_ptr_dict_linear* get_current();
	sms_ptr_dict_linear* get_next();
};
#endif

#endif
