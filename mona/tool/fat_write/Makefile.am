bin_PROGRAMS = fat_write
fat_write_CXXFLAGS = -g
fat_write_SOURCES  = VirtualFloppy.cpp VirtualFloppy.h fat.cpp fat.h IStorageDevice.h file.h main.cpp types.h endian_util.cpp endian_util.h fat_template.img

fat_write_datadir = @MONADIR@/share/fat_write
fat_write_data_DATA = fat_template.img

fat_write_includedir = @MONADIR@/include/fat_write
fat_write_include_HEADERS = fat.cpp VirtualFloppy.h fat.h file.h types.h IStorageDevice.h endian_util.h endian_util.cpp config.h
fat_write_dataDATA_INSTALL = ${INSTALL} -p -m 644
fat_write_includeHEADERS_INSTALL = ${INSTALL} -p -m 644
