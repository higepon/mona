bin_PROGRAMS = fat_write
fat_write_CXXFLAGS = -g
fat_write_SOURCES  = VirtualFloppy.cpp VirtualFloppy.h fat.cpp fat.h IStorageDevice.h file.h main.cpp types.h endian_util.cpp endian_util.h

fat_write_datadir = $(datadir)/fat_write
fat_write_data_DATA = fat_template.img

mona_includedir = @MONADIR@/include
mona_include_HEADERS = fat.cpp VirtualFloppy.h fat.h file.h types.h
