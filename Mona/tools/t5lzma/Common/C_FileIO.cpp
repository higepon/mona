// Common/C_FileIO.h

#include "C_FileIO.h"

namespace NC {
namespace NFile {
namespace NIO {

CFileBase::~CFileBase() { Close(); }

bool CFileBase::Open(const char *name, const char *access)
{
  Close();
  _handle = ::fopen(name, access);
  return _handle != 0;
}

bool CFileBase::Close()
{
  if(_handle == 0)
    return true;
  if (fclose(_handle) != 0)
    return false;
  _handle = 0;
  return true;
}

bool CFileBase::GetPosition(UInt64 &position) const
{
  long posTemp = ftell(_handle);
  if (posTemp == -1)
    return false;
  position = posTemp;
  return true;
}

bool CFileBase::GetLength(UInt64 &length) const
{
  UInt64 curPos;
  if (!GetPosition(curPos))
    return false;;
  bool result = false;
  if (Seek(0, SEEK_END))
  {
    result = GetPosition(length);
  }
  Seek(curPos, SEEK_SET);
  return result;
}

bool CFileBase::Seek(Int64 distanceToMove, int moveMethod) const
{
  if (fseek(_handle, (long)distanceToMove, moveMethod) != 0)
    return false;
  return true;
}

/////////////////////////
// CInFile

bool CInFile::Open(const char *name)
{
  return CFileBase::Open(name, "rb");
}

bool CInFile::Read(void *data, UInt32 size, UInt32 &processedSize)
{
  processedSize = fread(data, 1, size, _handle);
  return ferror(_handle) == 0;
}

/////////////////////////
// COutFile

bool COutFile::Open(const char *name)
{
  return CFileBase::Open(name, "wb");
}

bool COutFile::Write(const void *data, UInt32 size, UInt32 &processedSize)
{
  processedSize = fwrite(data, 1, size, _handle);
  return ferror(_handle) == 0;
}

}}}
