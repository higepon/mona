// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <monapi.h>
#include <gui/System/Array.h>
#include <bzlib.h>

using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
	if (pekoe->size() < 1)
	{
		printf("usage: BZCAT FILENAME.BZ2\n");
		return 1;
	}
	
	const char* file = pekoe->get(0);
	FileInputStream fis(file);
	if (fis.open() != 0)
	{
		printf("ERROR: can not open: %s\n", file);
		return 1;
	}
	
	int size = fis.getFileSize();
	_A<char> buf1(size);
	fis.read((byte*)buf1.get(), size);
	fis.close();
	
	bz_stream bzs;
	bzs.bzalloc = NULL;
	bzs.bzfree  = NULL;
	int result = BZ2_bzDecompressInit(&bzs, 0, 0);
	if (result != BZ_OK)
	{
		printf("ERROR: can not initialize bzip2\n");
		return 1;
	}
	
	bzs.next_in = buf1.get();
	bzs.avail_in = size;
	_A<char> buf2(256);
	while (result == BZ_OK)
	{
		bzs.next_out  = buf2.get();
		bzs.avail_out = buf2.get_Length() - 1;
		bzs.total_out_hi32 = bzs.total_out_lo32 = 0;
		result = BZ2_bzDecompress(&bzs);
		if (result == BZ_OK || result == BZ_STREAM_END)
		{
			buf2[bzs.total_out_lo32] = '\0';
			unsigned int len = strlen(buf2.get());
			if (len != bzs.total_out_lo32) result = BZ_STREAM_END;
			printf(buf2.get());
		}
		else if (result == BZ_MEM_ERROR)
		{
			printf("ERROR: out of memory\n");
		}
		else
		{
			printf("ERROR: can not decompress data\n");
		}
	}
	BZ2_bzDecompressEnd(&bzs);
	if (result != BZ_STREAM_END) return 1;
	
	return 0;
}
