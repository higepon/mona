#ifndef __MONAPI_CLIST_H__
#define __MONAPI_CLIST_H__

typedef struct
{
    void** pointer;
    int count, capacity;
} monapi_clist;

#ifdef __cplusplus
extern "C"
{
#endif
extern monapi_clist* monapi_clist_new();
extern void monapi_clist_delete(monapi_clist*);
extern void monapi_clist_clear(monapi_clist*);
extern void monapi_clist_add(monapi_clist*, void*);
extern void* monapi_clist_remove_at(monapi_clist*, int);
extern void* monapi_clist_remove(monapi_clist*, void*);
extern int monapi_clist_index_of(monapi_clist*, void*);
extern void* monapi_clist_get_item(monapi_clist*, int);
#ifdef __cplusplus
}
#endif

#endif  // __MONAPI_CLIST_H__
