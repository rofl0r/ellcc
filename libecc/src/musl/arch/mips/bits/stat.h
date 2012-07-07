/* copied from kernel definition, but with padding replaced
 * by the corresponding correctly-sized userspace types. */

struct stat
{
	dev_t st_dev;
	long __st_padding1[3];
	ino_t st_ino;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	long __st_padding2[2];
	long st_size;
        long __st_padding3;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
        long st_padding4[14];
};

#ifdef _BSD_SOURCE
#define st_atimespec st_atim
#define st_mtimespec st_mtim
#define st_ctimespec st_ctim
#endif
