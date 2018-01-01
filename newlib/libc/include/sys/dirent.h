#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H

#ifdef __cplusplus
extern "C" {
#endif

#define dirent SceIoDirent
typedef struct {
	SceIoDirent ent; //< updated and returned by readdir
	SceUID fd;       //< the directory file descriptor
} DIR;

int __lastdir;
#define opendir(path)  ((__lastdir=sceIoDopen(path)) < 0 ? NULL : &(DIR){.fd=__lastdir})
#define readdir(dirp)  (sceIoDread(dirp->fd, &dirp->ent) <= 0 ? NULL : &dirp->ent)
#define closedir(dirp) (sceIoDclose(dirp->fd))

#ifdef __cplusplus
}
#endif

#endif
