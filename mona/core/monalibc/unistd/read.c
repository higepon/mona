#include <monalibc/stdio.h>

ssize_t read(int fd, void *buf, size_t count) {
  _logprintf("[WARN] libc read not implemented");
  return -1;
}
