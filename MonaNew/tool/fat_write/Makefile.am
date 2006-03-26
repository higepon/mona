bin_PROGRAMS = fat_write
fat_write_CXXFLAGS = -g
fat_write_SOURCES  = VirtualFloppy.cpp VirtualFloppy.h fat.cpp fat.h file.h main.cpp types.h

fat_write_datadir = $(datadir)/fat_write
fat_write_data_DATA = fat_template.img

mona_includedir = @MONADIR@/include
mona_include_HEADERS = fat.cpp VirtualFloppy.h fat.h file.h types.h

#if LINUX
#fat_write_SOURCES = VirtualFloppy.cpp VirtualFloppy.h fat.cpp fat.h file.h main.cpp types.h
#else
#fat_write_SOURCES = VirtualFloppy.cpp VirtualFloppy.h fat.cpp fat.h file.h main.cpp types.h
#endif
