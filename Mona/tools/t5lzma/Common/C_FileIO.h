// Common/C_FileIO.h

#ifndef __COMMON_C_FILEIO_H
#define __COMMON_C_FILEIO_H

#include <stdio.h>
#include "Types.h"
#include "MyWindows.h"

namespace NC {
namespace NFile {
namespace NIO {

class CFileBase
{
protected:
  FILE *_handle;
  bool Open(const char *name, const char *access);
public:
  CFileBase(): _handle(0){};
  virtual ~CFileBase();
  virtual bool Close();

  bool GetPosition(UInt64 &position) const;
  bool GetLength(UInt64 &length) const;
  bool Seek(Int64 distanceToMove, int moveMethod) const;  
};

class CInFile: public CFileBase
{
public:
  bool Open(const char *name);
  bool Read(void *data, UInt32 size, UInt32 &processedSize);
};

class COutFile: public CFileBase
{
public:
  bool Open(const char *name);
  bool Write(const void *data, UInt32 size, UInt32 &processedSize);
};

}}}

#endif
