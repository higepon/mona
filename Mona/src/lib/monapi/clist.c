#include <monapi/clist.h>

#ifdef MONA
#include <monapi/syscall.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif

#define DEFAULT_CAPACITY 16
#define ASSERT(cond) if (!cond) { printf("%s:%d: null pointer exception!\n", __FILE__, __LINE__); exit(1); }

static void monapi_clist_initialize(monapi_clist* this)
{
    this->pointer  = malloc(DEFAULT_CAPACITY * sizeof(void*));
    ASSERT(this->pointer);
    this->count    = 0;
    this->capacity = DEFAULT_CAPACITY;
}

monapi_clist* monapi_clist_new()
{
    monapi_clist* this = (monapi_clist*)malloc(sizeof(monapi_clist));
    ASSERT(this);
    monapi_clist_initialize(this);
    return this;
}

void monapi_clist_delete(monapi_clist* this)
{
    if (this->pointer != NULL) free(this->pointer);
    free(this);
}

void monapi_clist_clear(monapi_clist* this)
{
    free(this->pointer);
    monapi_clist_initialize(this);
}

void monapi_clist_add(monapi_clist* this, void* item)
{
    if (this->count == this->capacity)
    {
        int i;
        void** ptr;

        if (this->count < DEFAULT_CAPACITY)
        {
            this->capacity = DEFAULT_CAPACITY;
        }
        else
        {
            this->capacity <<= 1;
        }
        ptr = malloc(this->capacity * sizeof(void*));
        ASSERT(ptr);
        if (this->pointer != NULL)
        {
            for (i = 0; i < this->count; i++) ptr[i] = this->pointer[i];
            free(this->pointer);
        }
        this->pointer = ptr;
    }
    this->pointer[this->count] = item;
    this->count++;
}

void* monapi_clist_remove_at(monapi_clist* this, int index)
{
    int i;
    void* ret = monapi_clist_get_item(this, index);

    this->count--;
    for (i = index; i < this->count; i++)
    {
        this->pointer[i] = this->pointer[i + 1];
    }
    return ret;
}

void* monapi_clist_remove(monapi_clist* this, void* item)
{
    int idx = monapi_clist_index_of(this, item);
    if (idx < 0) return NULL;

    return monapi_clist_remove_at(this, idx);
}

int monapi_clist_index_of(monapi_clist* this, void* item)
{
    int i;

    for (i = 0; i < this->count; i++)
    {
        if (this->pointer[i] == item) return i;
    }
    return -1;
}

void* monapi_clist_get_item(monapi_clist* this, int index)
{
    ASSERT(!(index < 0 || this->count - 1 < index));
    return this->pointer[index];
}
