#ifndef __RAMDISK_H__
#define __RAMDISK_H__

#include <monalibc.h>
#include "FileSystem.h"
#include "VnodeManager.h"
#include <map>

int ramdisk_debug_times = 0;

namespace RamDisk {


    typedef std::vector<char*> Chunks;
    struct FileInfo;
    typedef std::map<std::string, FileInfo*> FileMap;
    enum
      {
          CHUNK_SIZE = 4096,
      };


    struct FileInfo {
        std::string name;
        uint32_t size;
        // int pos;
        Chunks chunks;
        ~FileInfo()
          {
              for(Chunks::iterator it = chunks.begin(); it != chunks.end(); it++)
                {
                    char* ptr = *it;
                    delete(ptr);
                }
              chunks.clear();
          }

        inline Chunks::iterator findChunk(int offset)
          {
              int chunkNum = offset/CHUNK_SIZE;

              Chunks::iterator it = chunks.begin();
              for(int i = 0; i < chunkNum; i++)
                {
                    it++;
                    ASSERT(it != chunks.end());
                }
              return it;
          }

        inline int chunkRemainLen(int offsetInChunk, int len)
          {
              if(offsetInChunk + len > CHUNK_SIZE)
                return CHUNK_SIZE - offsetInChunk;
              return len;
          }

        inline Chunks::iterator extendOneChunk()
          {
              char* ptr = new char[CHUNK_SIZE];
              ASSERT(ptr);
              chunks.push_back(ptr);
              Chunks::iterator it = chunks.end();
              it--;
              return it;
          }

        inline void extendToOffset(int offset)
          {
              int chunkNum = offset/CHUNK_SIZE+1;
              int addNum = chunkNum- chunks.size();
              if(addNum <=0)
                return;
              for(int i = 0; i < addNum; i++)
                {
                    extendOneChunk();
                }
          }

        inline void writeChunks(int offset, int len, uint8_t *src)
          {
              int rest = len;
              int chunkOffset = offset%CHUNK_SIZE;

              extendToOffset(offset);
              Chunks::iterator it = findChunk(offset);
              ASSERT(it != chunks.end());

              while(rest > 0) {
                  int copySize = chunkRemainLen(chunkOffset, rest);
                  ASSERT(copySize > 0);

                  char *ptr = *it;
                  memcpy(ptr + chunkOffset, src, copySize);

                  src += copySize;
                  chunkOffset = 0; // only none-zero at first chunk.
                  rest -= copySize;
                  it++;
                  if(it == chunks.end())
                        it = extendOneChunk();
              }
          }

        inline void readChunks(int offset, int len, uint8_t* dest)
          {
              int rest = len;
              int chunkOffset = offset%CHUNK_SIZE;

              Chunks::iterator it = findChunk(offset);
              while(rest > 0) {
                  int copySize = chunkRemainLen(chunkOffset, rest);
                  ASSERT(copySize > 0);

                  char *ptr = *it;
                  memcpy(dest, ptr+chunkOffset, copySize);

                  dest += copySize;
                  chunkOffset = 0; // only none-zero at first chunk.
                  rest -= copySize;
                  it++;
                  ASSERT(it != chunks.end());
              }
          }


    };

    // do not support subfolder.
    class RamDiskFileSystem : public FileSystem
      {
        public:

          RamDiskFileSystem(VnodeManager* vmanager) {
              vmanager_ = vmanager;

              root_ = vmanager_->alloc();
              // root_->fnode  = rootDir;
              root_->fs     = this;
              root_->type = Vnode::DIRECTORY;
          }
          virtual ~RamDiskFileSystem() {
              // should already removed at VnodeCacher's destructor
              // We had better also destruct correctly here, but now I just confirm my understanding by assert.
              ASSERT(root_ == NULL);
              ASSERT(files_.size() == 0);
          }


        public:
          virtual int initialize(){ return MONA_SUCCESS; }
          virtual int lookup(Vnode* diretory, const std::string& file, Vnode** found, int type)
            {
                ramdisk_debug_times++;

                Vnode* v = vmanager_->cacher()->lookup(diretory, file);
                if (v != NULL && v->type == type)
                  {
                      *found = v;
                      return MONA_SUCCESS;
                  }


                FileMap::iterator it = files_.find(file);
                if(it == files_.end())
                  return MONA_ERROR_ENTRY_NOT_FOUND;



                Vnode* newVnode = vmanager_->alloc(); //never return NULL?
                newVnode->fnode = (*it).second;
                newVnode->type = type;
                newVnode->fs = this;
                vmanager_->cacher()->add(diretory, file, newVnode);
                *found = newVnode;

                return MONA_SUCCESS;
            }

          virtual int open(Vnode* file, int mode)
            {
                return MONA_SUCCESS;
            }
          virtual int create(Vnode* dir, const std::string& file)
            {
                FileMap::iterator it = files_.find(file);
                if(it != files_.end())
                {
                    // overwrite case. remove previous file.
                    FileInfo* fi  = (*it).second;
                    vmanager_->cacher()->remove(root_, fi->name);
                    delete fi;
                    files_.erase(it);
                }
                FileInfo* finfo = new FileInfo();
                finfo->name = file;
                finfo->size = 0;
                files_.insert(std::pair<std::string, FileInfo*>(file, finfo));
                return MONA_SUCCESS;
            }


          virtual int read(Vnode* file, struct io::Context* context)
            {
                if (file->type != Vnode::REGULAR) return MONA_FAILURE;
                FileInfo* f = (FileInfo*)file->fnode;
                uint32_t offset = context->offset;
                uint32_t readSize = context->size;
                uint32_t rest = f->size - offset;

                if (rest < readSize)
                  {
                      readSize = rest;
                  }

                context->memory = monapi_cmemoryinfo_new();
                if(readSize == 0)
                  return MONA_SUCCESS;

                if (monapi_cmemoryinfo_create(context->memory, readSize, MONAPI_FALSE) != M_OK)
                  {
                      monapi_cmemoryinfo_delete(context->memory);
                      return MONA_ERROR_MEMORY_NOT_ENOUGH;
                  }
                f->readChunks(offset, readSize, context->memory->Data);

                context->resultSize = readSize;
                context->offset += readSize;
                return MONA_SUCCESS;

            }



          virtual int write(Vnode* file, struct io::Context* context)
            {
                if (file->type != Vnode::REGULAR) return MONA_FAILURE;
                FileInfo* f = (FileInfo*)file->fnode;
                monapi_cmemoryinfo* memory = context->memory;

                uint32_t offset = context->offset;
                uint32_t writeSize = context->size;
                f->writeChunks(offset, writeSize, memory->Data);
                f->size = f->size > offset+writeSize ? f->size : offset+writeSize;
                return MONA_SUCCESS;
            }
          virtual int seek(Vnode* file, uint32_t offset, uint32_t origin) { return MONA_SUCCESS; }

          virtual int readdir(Vnode* dir, monapi_cmemoryinfo** entries)
            {
                typedef std::vector<monapi_directoryinfo*> Files;
                Files files;
                monapi_directoryinfo di;
                currentIt_ = files_.begin();


                while (readdirInternal(di.name, &di.size, &di.attr) == MONA_SUCCESS)
                  {
                      files.push_back(new monapi_directoryinfo(di));
                  }

                monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();

                int size = files.size();
                if (monapi_cmemoryinfo_create(ret, sizeof(int) + size * sizeof(monapi_directoryinfo), MONAPI_FALSE) != M_OK)
                  {
                      monapi_cmemoryinfo_delete(ret);
                      for (Files::const_iterator it = files.begin(); it != files.end(); ++it)
                        {
                            delete (*it);
                        }
                      return MONA_ERROR_MEMORY_NOT_ENOUGH;
                  }

                memcpy(ret->Data, &size, sizeof(int));
                monapi_directoryinfo* p = (monapi_directoryinfo*)&ret->Data[sizeof(int)];

                for (Files::const_iterator it = files.begin(); it != files.end(); ++it)
                  {
                      memcpy(p, (*it), sizeof(monapi_directoryinfo));
                      delete (*it);
                      p++;
                  }
                *entries = ret;
                return MONA_SUCCESS;
            }
          virtual int close(Vnode* file) { return MONA_SUCCESS; }
          virtual int delete_file(Vnode* file)
            {
                if(file == root_)
                  return MONA_FAILURE; // root is undeletable

                destroyVnode(file);
                return MONA_SUCCESS;
            }
          virtual int stat(Vnode* file, Stat* st)
            {
                FileInfo* f = (FileInfo*)file->fnode;
                st->size = f->size;
                return MONA_SUCCESS;
            }
          virtual Vnode* getRoot() const
            {
                return root_;
            }
          virtual void destroyVnode(Vnode* vnode)
            {
                if (vnode->type == Vnode::DIRECTORY)
                  {
                      // vnode must be root
                      ASSERT(vnode == root_);
                      delete vnode;
                      root_ = NULL;
                      return;
                  }
                FileInfo* finfo = (FileInfo*)vnode->fnode;
                std::string name = finfo->name;
                vmanager_->cacher()->remove(root_, name);
                FileMap::iterator it = files_.find(name);
                ASSERT(it != files_.end());
                delete finfo;
                files_.erase(it);
                delete vnode;
            }

        protected:
          virtual int deviceOn(){ return MONA_SUCCESS; }
          virtual int deviceOff(){ return MONA_SUCCESS; }
          virtual int readdirInternal(char* name, int* size, int* attribute)
            {
                *attribute = 0;
                if(currentIt_ == files_.end())
                  return MONA_FAILURE;

                strcpy(name, (*currentIt_).first.c_str());
                *size = (*currentIt_).second->size;

                currentIt_++;
                return MONA_SUCCESS;
            }

          VnodeManager* vmanager_;
          Vnode* root_;
          FileMap files_;
          FileMap::iterator currentIt_;
      };


}

#endif // __RAMDISK_H__
