/* connector for truncate */

#include <reent.h>
#include <sys/stat.h>
#include <unistd.h>

int
truncate (const char * path,
     _off_t length)
{
  return _truncate_r (_REENT, path, length);
}
