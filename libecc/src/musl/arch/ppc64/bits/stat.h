/* copied from kernel definition, but with padding replaced
 * by the corresponding correctly-sized userspace types. */

struct stat
{
	dev_t st_dev;
	ino_t st_ino;
#if defined(__ppc64__)
	nlink_t st_nlink;
	mode_t st_mode;
#else
	mode_t st_mode;
	nlink_t st_nlink;
#endif
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	off_t st_size;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
        unsigned long pad1[2];
#if defined(__ppc64__)
        unsigned long pad2;
#endif
};

#ifdef _BSD_SOURCE
#define st_atimespec st_atim
#define st_mtimespec st_mtim
#define st_ctimespec st_ctim
#endif
