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

#BOEHM_GC_DIR  = ./BoehmGC/gc-7.0
BOEHM_GC_DIR  = ../../mona/core/scheme/BoehmGC/gc-7.0
GC_CFLAGS     = $(BOEHM_GC_CFLAGS) -DMONASH_DONT_FREE_MEMORY -I $(BOEHM_GC_DIR)/include -DUSE_BOEHM_GC -DGC_OPERATOR_NEW_ARRAY -DGC_NOT_DLL #-DMONA_GC_DEBUG


GC_CXX_OBJECTS=gc_cpp.o

GC_C_OBJECTS = alloc.o \
backgraph.o              \
blacklst.o               \
dbg_mlc.o                \
dyn_load.o               \
finalize.o               \
headers.o                \
mach_dep.o               \
malloc.o                 \
mallocx.o                \
mark.o                   \
mark_rts.o               \
misc.o                   \
new_hblk.o               \
obj_map.o                \
os_dep.o                 \
reclaim.o                \
allchblk.o

GC_OBJECTS = $(GC_C_OBJECTS) $(GC_CXX_OBJECTS)

