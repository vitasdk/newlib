/* connector for ftruncate */

#include <reent.h>
#include <sys/stat.h>
#include <unistd.h>

int
ftruncate (int fd,
     off_t length)
{
  return _ftruncate_r (_REENT, fd, length);
}
