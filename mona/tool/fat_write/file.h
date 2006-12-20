//*****************************************************************************
// FILE.h :
// 2004/01/30 by Gaku :
// Licence : see http://gaku.s12.xrea.com/wiki/main.cgi?c=g&p=Gaku%2FLicence
//*****************************************************************************

//-----------------------------------------------------------------------------
#ifndef _FILE_H_
#define _FILE_H_

//-----------------------------------------------------------------------------
#ifdef MONA
#include <sys/types.h>
#else
#include "types.h"
#endif

namespace FatFS
{

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// File
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class File
{
public:
	virtual ~File() {}

	virtual uint32_t read (uint8_t *bf, uint32_t sz) = 0;
	virtual uint32_t write (uint8_t *bf, uint32_t sz) = 0;
	virtual bool seek (int pt, int flag) = 0;
	virtual bool flush () = 0;
	virtual bool resize (uint32_t sz) = 0;
	virtual uint32_t position () = 0;
	virtual uint32_t size () = 0;
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Directory
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class Directory
{
public:
	virtual ~Directory() {}

	virtual int searchEntry (uint8_t *bf) = 0;
	virtual int getEntryName (int entry, uint8_t *bf) = 0;
	virtual int getHeadEntry () = 0;
	virtual int getNextEntry (int entry) = 0;
	virtual bool deleteEntry (int entry) = 0;
	virtual int newDirectory (uint8_t *bf) = 0;
	virtual int newFile (uint8_t *bf, uint32_t sz) = 0;
	virtual Directory* getDirectory (int entry) = 0;
	virtual File* getFile (int entry) = 0;
	virtual bool isDirectory (int entry) = 0;
	virtual bool isFile (int entry) = 0;
	virtual uint32_t getIdentifer () = 0;
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Storage
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class Storage
{
public:
	virtual ~Storage () {}

	virtual Directory* getRootDirectory () = 0;
};
}
//-----------------------------------------------------------------------------
#endif // _FILE_H_ //
