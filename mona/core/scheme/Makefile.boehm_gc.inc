BOEHM_GC_CFLAGS = -DPACKAGE_NAME=\"gc\"    \
-DPACKAGE_TARNAME=\"gc\"                   \
-DPACKAGE_VERSION=\"7.0\"                  \
-DPACKAGE_STRING=\"gc\ 7.0\"               \
-DPACKAGE_BUGREPORT=\"Hans.Boehm@hp.com\"  \
-DGC_VERSION_MAJOR=7                       \
-DGC_VERSION_MINOR=0                       \
-DPACKAGE=\"gc\"                           \
-DVERSION=\"7.0\"                          \
-DNO_EXECUTE_PERMISSION=1                  \
-DALL_INTERIOR_POINTERS=1                  \
-DATOMIC_UNCOLLECTABLE=1                   \
-DNO_CLOCK                                 \
-DGC_PRINT_STATS                           \
-UMSWIN32                                  

BOEHM_GC_DIR  = ./BoehmGC/gc-7.0
GC_CFLAGS     = $(BOEHM_GC_CFLAGS) -DMONASH_DONT_FREE_MEMORY -I $(BOEHM_GC_DIR)/include -DUSE_BOEHM_GC -DGC_OPERATOR_NEW_ARRAY -DGC_NOT_DLL #-DMONA_GC_DEBUG

GC_CXX_SOURCES = $(BOEHM_GC_DIR)/gc_cpp.cpp
GC_C_SOURCES   = $(BOEHM_GC_DIR)/alloc.c \
$(BOEHM_GC_DIR)/backgraph.c              \
$(BOEHM_GC_DIR)/blacklst.c               \
$(BOEHM_GC_DIR)/dbg_mlc.c                \
$(BOEHM_GC_DIR)/dyn_load.c               \
$(BOEHM_GC_DIR)/finalize.c               \
$(BOEHM_GC_DIR)/headers.c                \
$(BOEHM_GC_DIR)/mach_dep.c               \
$(BOEHM_GC_DIR)/malloc.c                 \
$(BOEHM_GC_DIR)/mallocx.c                \
$(BOEHM_GC_DIR)/mark.c                   \
$(BOEHM_GC_DIR)/mark_rts.c               \
$(BOEHM_GC_DIR)/misc.o                   \
$(BOEHM_GC_DIR)/new_hblk.c               \
$(BOEHM_GC_DIR)/obj_map.c                \
$(BOEHM_GC_DIR)/os_dep.c                 \
$(BOEHM_GC_DIR)/reclaim.c                \
$(BOEHM_GC_DIR)/allchblk.c

GC_CXX_OBJECTS = $(GC_CXX_SOURCES:.cpp=.o)
GC_C_OBJECTS   = $(GC_C_SOURCES:.c=.o)
GC_OBJECTS     = $(GC_C_OBJECTS) $(GC_CXX_OBJECTS)
