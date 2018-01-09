/* connector for ftruncate */

#include <reent.h>
#include <sys/stat.h>
#include <unistd.h>

int
_DEFUN (ftruncate, (fd, pstat),
     int fd _AND
     off_t length)
{
  return _ftruncate_r (_REENT, fd, length);
}
