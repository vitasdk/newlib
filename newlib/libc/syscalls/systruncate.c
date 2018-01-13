/* connector for truncate */

#include <reent.h>
#include <sys/stat.h>
#include <unistd.h>

int
_DEFUN (truncate, (path, pstat),
     const char * path _AND
     off_t length)
{
  return _truncate_r (_REENT, path, length);
}
