#ifdef __cplusplus
extern "C" {
#endif

int __syscall_0(dword func);
int __syscall_1(dword func, dword arg1);
int __syscall_2(dword func, dword arg1, dword arg2);
int __syscall_3(dword func, dword arg1, dword arg2, dword arg3);

// キャストで 型チェックしなくなる点にちうい
#define	syscall_mthread_create(f)								\
			(__syscall_1(SYSTEM_CALL_MTHREAD_CREATE, (dword)(f)))
#define	syscall_mthread_join(i)									\
   			(__syscall_1(SYSTEM_CALL_MTHREAD_JOIN, (dword)(i)))
#define	syscall_mthread_yeild_message()							\
			(__syscall_0(SYSTEM_CALL_MTHREAD_YIELD_MESSAGE))
#define	syscall_sleep(t)										\
			(__syscall_1(SYSTEM_CALL_MTHREAD_SLEEP, (dword)(t)))
#define syscall_get_tick()										\
			((dword)__syscall_0(SYSTEM_CALL_GET_TICK))
#define syscall_get_kernel_version(b, s)						\
			(__syscall_2(SYSTEM_CALL_SEND, (dword)(b), (s)))

#define	syscall_print(m)										\
			(__syscall_1(SYSTEM_CALL_PRINT, (dword)(m)))

#define	syscall_load_process(p, n, l)							\
			(__syscall_3(SYSTEM_CALL_LOAD_PROCESS,				\
									(dword)(p), (dword)(n), (dword)(l)))
#define	syscall_load_process_image(i)							\
			(__syscall_1(SYSTEM_CALL_LOAD_PROCESS_IMAGE, (dword)(i)))
#define	syscall_kill()											\
			(__syscall_0(SYSTEM_CALL_KILL))
#define	syscall_kill_thread(i)									\
			(__syscall_1(SYSTEM_CALL_KILL_THREAD, (i)))

#define	syscall_send(i, m)										\
			(__syscall_2(SYSTEM_CALL_SEND, (dword)(i), (dword)(m)))
#define	syscall_receive(m)										\
			(__syscall_1(SYSTEM_CALL_RECEIVE, (dword)(m)))
#define	syscall_exist_message()									\
			(__syscall_0(SYSTEM_CALL_EXIST_MESSAGE))

#define	syscall_mutex_create()									\
			(__syscall_0(SYSTEM_CALL_MUTEX_CREATE))
#define	syscall_mutex_trylock(i)								\
			(__syscall_1(SYSTEM_CALL_MUTEX_TRYLOCK, (dword)(i)))
#define	syscall_mutex_lock(i)									\
			(__syscall_1(SYSTEM_CALL_MUTEX_LOCK, (dword)(i)))
#define	syscall_mutex_unlock(i)									\
			(__syscall_1(SYSTEM_CALL_MUTEX_UNLOCK, (dword)(i)))
#define	syscall_mutex_destroy(i)								\
			(__syscall_1(SYSTEM_CALL_MUTEX_DESTROY, (dword)(i)))

#define	syscall_lookup(n)										\
			((dword)__syscall_1(SYSTEM_CALL_LOOKUP, (dword)(n)))
#define	syscall_lookup_main_thread(n)							\
			(__syscall_1(SYSTEM_CALL_LOOKUP_MAIN_THREAD, (dword)(n)))

#define	syscall_get_vram_info(i)								\
			(__syscall_1(SYSTEM_CALL_GET_VRAM_INFO, (dword)(i)))
#define	syscall_get_cursor(x, y)								\
			(__syscall_2(SYSTEM_CALL_GET_CURSOR, (dword)(x), (dword)(y)))
#define	syscall_set_cursor(x, y)								\
			(__syscall_2(SYSTEM_CALL_SET_CURSOR, (dword)(x), (dword)(y)))

#define	syscall_file_open(p, m, s)								\
			(__syscall_3(SYSTEM_CALL_FILE_OPEN,					\
									(dword)(p), (dword)(m), (dword)(s)))
#define	syscall_file_create(p)									\
			(__syscall_1(SYSTEM_CALL_FILE_CREATE, (dword)(p)))
#define	syscall_file_read(p, s, r)								\
			(__syscall_3(SYSTEM_CALL_FILE_READ,					\
									(dword)(p), (dword)(s), (dword)(r)))
#define	syscall_file_write(p, s, w)								\
			(__syscall_3(SYSTEM_CALL_FILE_WRITE,				\
									(dword)(p), (dword)(s), (dword)(w)))
#define	syscall_file_seek(p, f)									\
			((dword)__syscall_2(SYSTEM_CALL_FILE_SEEK,			\
									(dword)(p), (dword)(f)))
#define	syscall_file_position()									\
			((dword)__syscall_0(SYSTEM_CALL_FILE_POSITION))
#define	syscall_file_close()									\
			(__syscall_0(SYSTEM_CALL_FILE_CLOSE))

#define	syscall_fdc_open()										\
			(__syscall_0(SYSTEM_CALL_FDC_OPEN))
#define	syscall_fdc_read(l, p, b)								\
			(__syscall_3(SYSTEM_CALL_FDC_READ,					\
									(dword)(l), (dword)(p), (dword)(b)))
#define	syscall_fdc_write(l, p, b)								\
			(__syscall_3(SYSTEM_CALL_FDC_WRITE,					\
									(dword)(l), (dword)(p), (dword)(b)))
#define	syscall_fdc_close()										\
			(__syscall_0(SYSTEM_CALL_FDC_CLOSE))
#define	syscall_fdc_disk_changed()								\
			(__syscall_0(SYSTEM_CALL_FDC_DISK_CHANGED))

#define	syscall_get_pid()										\
			((dword)__syscall_0(SYSTEM_CALL_GET_PID))
#define	syscall_get_tid()										\
			((dword)__syscall_0(SYSTEM_CALL_GET_TID))

#define	syscall_get_arg_count()									\
			(__syscall_0(SYSTEM_CALL_ARGUMENTS_NUM))
#define	syscall_get_arg(p, n)									\
			(__syscall_2(SYSTEM_CALL_GET_ARGUMENTS, (dword)(p), (dword)(n)))

#define	syscall_get_date(d)										\
			(__syscall_1(SYSTEM_CALL_DATE, (dword)(d)))

#define	syscall_get_io()										\
			(__syscall_0(SYSTEM_CALL_GET_IO))

#define	syscall_memory_map_create(s)							\
			((dword)__syscall_1(SYSTEM_CALL_MEMORY_MAP_CREATE, (dword)(s)))
#define	syscall_memory_map_get_size(i)							\
			((dword)__syscall_1(SYSTEM_CALL_MEMORY_MAP_GET_SIZE, (dword)(i)))
#define syscall_memory_map_map(i, a)							\
			(__syscall_2(SYSTEM_CALL_MEMORY_MAP_MAP,			\
									(dword)(i), (dword)(a)))
#define	syscall_memory_map_unmap(i)								\
			(__syscall_1(SYSTEM_CALL_MEMORY_MAP_UNMAP, (dword)(i)))

#define	syscall_dir_open()										\
			(__syscall_0(SYSTEM_CALL_DIR_OPEN))
#define syscall_dir_read(n, s)									\
			(__syscall_2(SYSTEM_CALL_DIR_READ, (dword)(n), (dword)(s)))
#define	syscall_dir_close()										\
			(__syscall_0(SYSTEM_CALL_DIR_CLOSE))
#define	syscall_cd(p)											\
			(__syscall_1(SYSTEM_CALL_CD, (dword)(p)))

#define	syscall_set_ps_dump()									\
			(__syscall_0(SYSTEM_CALL_PS_DUMP_SET))
#define	syscall_read_ps_dump(i)									\
			(__syscall_1(SYSTEM_CALL_PS_DUMP_READ, (dword)(i)))

#ifdef __cplusplus
}
#endif
