// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <bzlib.h>
#include "GUIServer.h"
#include "bzip2.h"
#include "file.h"

int64_t GetBZ2DecompressedSize(MemoryInfo* mi)
{
	static char buf[256];
	
	bz_stream bzs;
	bzs.bzalloc = NULL;
	bzs.bzfree  = NULL;
	int result = BZ2_bzDecompressInit(&bzs, 0, 0);
	if (result != BZ_OK) return -1;
	
	int64_t ret = 0;
	bzs.next_in  = (char*)mi->Data;
	bzs.avail_in = mi->Size;
	while (result == BZ_OK)
	{
		bzs.next_out  = buf;
		bzs.avail_out = sizeof(buf);
		bzs.total_out_hi32 = bzs.total_out_lo32 = 0;
		result = BZ2_bzDecompress(&bzs);
		ret += bzs.total_out_lo32;
		ret += ((int64_t)bzs.total_out_hi32) << 32;
	}
	BZ2_bzDecompressEnd(&bzs);
	if (result != BZ_STREAM_END) return -1;
	
	return ret;
}

MemoryInfo* BZ2Decompress(MemoryInfo* mi)
{
	int64_t size = GetBZ2DecompressedSize(mi);
	
	// if size >= 4GB abort...
	if ((size >> 32) > 0) return NULL;
	
	MemoryInfo* ret = new MemoryInfo();
	if (!ret->Create((dword)size))
	{
		delete ret;
		return NULL;
	}
	
	bz_stream bzs;
	bzs.bzalloc = NULL;
	bzs.bzfree  = NULL;
	if (BZ2_bzDecompressInit(&bzs, 0, 0) != BZ_OK) return NULL;
	
	bzs.next_in   = (char*)mi->Data;
	bzs.avail_in  = mi->Size;
	bzs.next_out  = (char*)ret->Data;
	bzs.avail_out = ret->Size;
	int result = BZ2_bzDecompress(&bzs);
	BZ2_bzDecompressEnd(&bzs);
	if (result != BZ_STREAM_END)
	{
		ret->Dispose();
		delete ret;
		return NULL;
	}
	
	return ret;
}

MemoryInfo* BZ2DecompressFile(const char* file, bool prompt /*= false*/)
{
	MemoryInfo* mi = ReadFile(file, prompt), * ret = NULL;
	if (mi == NULL) return ret;
	
	if (prompt) printf("%s: Decompressing %s....", SVR, file);
	ret = BZ2Decompress(mi);
	if (prompt) printf(ret != NULL ? "OK\n" : "ERROR\n");
	
	mi->Dispose();
	delete mi;
	return ret;
}
