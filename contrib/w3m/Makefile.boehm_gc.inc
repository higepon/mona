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

gc_cpp.o: $(BOEHM_GC_DIR)/gc_cpp.cpp
	cp $(BOEHM_GC_DIR)/gc_cpp.o gc_cpp.o

alloc.o: $(BOEHM_GC_DIR)/alloc.c
	cp $(BOEHM_GC_DIR)/alloc.o alloc.o

backgraph.o: $(BOEHM_GC_DIR)/backgraph.c
	cp $(BOEHM_GC_DIR)/backgraph.o backgraph.o

blacklst.o: $(BOEHM_GC_DIR)/blacklst.c
	cp $(BOEHM_GC_DIR)/blacklst.o blacklst.o

dbg_mlc.o: $(BOEHM_GC_DIR)/dbg_mlc.c
	cp $(BOEHM_GC_DIR)/dbg_mlc.o dbg_mlc.o

dyn_load.o: $(BOEHM_GC_DIR)/dyn_load.c
	cp $(BOEHM_GC_DIR)/dyn_load.o dyn_load.o

finalize.o: $(BOEHM_GC_DIR)/finalize.c
	cp $(BOEHM_GC_DIR)/finalize.o finalize.o

headers.o: $(BOEHM_GC_DIR)/headers.c
	cp $(BOEHM_GC_DIR)/headers.o headers.o

mach_dep.o: $(BOEHM_GC_DIR)/mach_dep.c
	cp $(BOEHM_GC_DIR)/mach_dep.o mach_dep.o

malloc.o: $(BOEHM_GC_DIR)/malloc.c
	cp $(BOEHM_GC_DIR)/malloc.o malloc.o

mallocx.o: $(BOEHM_GC_DIR)/mallocx.c
	cp $(BOEHM_GC_DIR)/mallocx.o mallocx.o

mark.o: $(BOEHM_GC_DIR)/mark.c
	cp $(BOEHM_GC_DIR)/mark.o mark.o

mark_rts.o: $(BOEHM_GC_DIR)/mark_rts.c
	cp $(BOEHM_GC_DIR)/mark_rts.o mark_rts.o

misc.o: $(BOEHM_GC_DIR)/misc.c
	cp $(BOEHM_GC_DIR)/misc.o misc.o

new_hblk.o: $(BOEHM_GC_DIR)/new_hblk.c
	cp $(BOEHM_GC_DIR)/new_hblk.o new_hblk.o

obj_map.o: $(BOEHM_GC_DIR)/obj_map.c
	cp $(BOEHM_GC_DIR)/obj_map.o obj_map.o

os_dep.o: $(BOEHM_GC_DIR)/os_dep.c
	cp $(BOEHM_GC_DIR)/os_dep.o os_dep.o

reclaim.o: $(BOEHM_GC_DIR)/reclaim.c
	cp $(BOEHM_GC_DIR)/reclaim.o reclaim.o

allchblk.o: $(BOEHM_GC_DIR)/allchblk.c
	cp $(BOEHM_GC_DIR)/allchblk.o allchblk.o

