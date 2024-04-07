/****************************************************************************
 * include/fs/fs.h
 *
 *   Copyright (C) 2007-2009, 2011-2013, 2015-2018 Gregory Nutt. All rights
 *     reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __INCLUDE_FS_FS_H
#define __INCLUDE_FS_FS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "vfs_config.h"
#include "compiler.h"

#include "poll.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"

#include "stdarg.h"
#include "stdint.h"

#include "sys/vfs.h"
#include "los_vm_map.h"
#include "los_atomic.h"
#include "semaphore.h"
#include "los_spinlock.h"
#include "mount.h"

#ifndef CONFIG_DISABLE_MQUEUE
#include "mqueue.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* POSIX-like OS return values: */

#ifndef  VFS_ERROR
#define VFS_ERROR -1
#endif

#undef  OK
#define OK 0

#define MAX_DIRENT_NUM 14 // 14 means 4096 length buffer can store 14 dirent, see struct DIR
#define MS_RDONLY 1
#define MS_NOSYNC 2
#define PROCFS_MOUNT_POINT  "/proc"
#define PROCFS_MOUNT_POINT_SIZE (sizeof(PROCFS_MOUNT_POINT) - 1)

#define RAMFS_MOUNT_POINT   "/ramfs"
#define RAMFS_MOUNT_POINT_SIZE  (sizeof(RAMFS_MOUNT_POINT) - 1)

#define DEFAULT_DIR_MODE        0777
#define DEFAULT_FILE_MODE       0666
#define USER_MODE_SHIFT         6
#define GROUP_MODE_SHIFT        3
#define DEFAULT_MOUNT_DIR_MODE  0755
/* Format options (3rd argument of format) */
#define FMT_FAT      0x01
#define FMT_FAT32    0x02
#define FMT_ANY      0x07
#define FMT_ERASE    0x08

/* system time flag for FAT */
#define FAT_SYSTEM_TIME_ENABLE   0x01
#define FAT_SYSTEM_TIME_DISABLE  0x00

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
/* Stream flags for the fs_flags field of in struct file_struct */

#define __FS_FLAG_EOF   (1 << 0) /* EOF detected by a read operation */
#define __FS_FLAG_ERROR (1 << 1) /* Error detected by any operation */
#define __FS_FLAG_LBF   (1 << 2) /* Line buffered */
#define __FS_FLAG_UBF   (1 << 3) /* Buffer allocated by caller of setvbuf */

#define FALLOC_FL_KEEP_SIZE 1 /* extend size */

/* The struct file_operations open(0) normally returns zero on success and
 * a negated errno value on failure.  There is one case, however, where
 * the open method will redirect to another driver and return a file
 * descriptor instead.
 *
 * This case is when SUSv1 pseudo-terminals are used (CONFIG_PSEUDOTERM_SUSV1=y).
 * In this case, the output is encoded and decoded using these macros in
 * order to support (a) returning file descriptor 0 (which really should
 * not happen), and (b) avoiding confusion if some other open method returns
 * a positive, non-zero value which is not a file descriptor.
 *
 *   OPEN_ISFD(r) tests if the return value from the open method is
 *     really a file descriptor.
 *   OPEN_SETFD(f) is used by an implementation of the open() method
 *     in order to encode a file descriptor in the return value.
 *   OPEN_GETFD(r) is use by the upper level open() logic to decode
 *     the file descriptor encoded in the return value.
 *
 * REVISIT: This only works for file descriptors in the in range 0-255.
 */

#define OPEN_MAGIC      0x4200
#define OPEN_MASK       0x00ff
#define OPEN_MAXFD      0x00ff

#define OPEN_ISFD(r)    (((r) & ~OPEN_MASK) == OPEN_MAGIC)
#define OPEN_SETFD(f)   ((f) | OPEN_MAGIC)
#define OPEN_GETFD(r)   ((r) & OPEN_MASK)

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

#define AT_REMOVEDIR 0x200

/* Attribute flags. */
#define CHG_MODE 1
#define CHG_UID 2
#define CHG_GID 4
#define CHG_SIZE 8
#define CHG_ATIME 16
#define CHG_MTIME 32
#define CHG_CTIME 64

struct IATTR
{
  /* This structure is used for record vnode attr. */
  unsigned int attr_chg_valid;
  unsigned int attr_chg_flags;
  unsigned attr_chg_mode;
  unsigned attr_chg_uid;
  unsigned attr_chg_gid;
  unsigned attr_chg_size;
  unsigned attr_chg_atime;
  unsigned attr_chg_mtime;
  unsigned attr_chg_ctime;
};

/* Forward references */

struct file;
struct Vnode;
struct stat;
struct statfs;
struct pollfd;
struct fs_dirent_s;

/* This structure is provided by devices when they are registered with the
 * system.  It is used to call back to perform device specific operations.
 */

struct file_operations_vfs
{
  /* The device driver open method differs from the mountpoint open method */

  int     (*open)(struct file *filep);

  /* The following methods must be identical in signature and position because
   * the struct file_operations and struct mountp_operations are treated like
   * unions.
   */

  int     (*close)(struct file *filep);
  ssize_t (*read)(struct file *filep, char *buffer, size_t buflen);
  ssize_t (*write)(struct file *filep, const char *buffer, size_t buflen);
  off_t   (*seek)(struct file *filep, off_t offset, int whence);
  int     (*ioctl)(struct file *filep, int cmd, unsigned long arg);
  int     (*mmap)(struct file* filep, struct VmMapRegion *region);
  /* The two structures need not be common after this point */

#ifndef CONFIG_DISABLE_POLL
  int     (*poll)(struct file *filep, poll_table *fds);
#endif
  int     (*stat)(struct file *filep, struct stat* st);
  int     (*fallocate)(struct file* filep, int mode, off_t offset, off_t len);
  int     (*fallocate64)(struct file *filep, int mode, off64_t offset, off64_t len);
  int     (*fsync)(struct file *filep);
  ssize_t (*readpage)(struct file *filep, char *buffer, size_t buflen);
  int     (*unlink)(struct Vnode *vnode);
};

/* This structure provides information about the state of a block driver */

#ifndef CONFIG_DISABLE_MOUNTPOINT
struct geometry
{
  bool               geo_available;    /* true: The device is available */
  bool               geo_mediachanged; /* true: The media has changed since last query */
  bool               geo_writeenabled; /* true: It is okay to write to this device */
  unsigned long long geo_nsectors;     /* Number of sectors on the device */
  size_t             geo_sectorsize;   /* Size of one sector */
};

/* This structure is provided by block devices when they register with the
 * system.  It is used by file systems to perform filesystem transfers.  It
 * differs from the normal driver vtable in several ways -- most notably in
 * that it deals in struct Vnode vs. struct filep.
 */

struct Vnode;
struct block_operations
{
  int     (*open)(struct Vnode *vnode);
  int     (*close)(struct Vnode *vnode);
  ssize_t (*read)(struct Vnode *vnode, unsigned char *buffer,
            unsigned long long start_sector, unsigned int nsectors);
  ssize_t (*write)(struct Vnode *vnode, const unsigned char *buffer,
            unsigned long long start_sector, unsigned int nsectors);
  int     (*geometry)(struct Vnode *vnode, struct geometry *geometry);
  int     (*ioctl)(struct Vnode *vnode, int cmd, unsigned long arg);
  int     (*unlink)(struct Vnode *vnode);
};

struct mountpt_operations2
{
  /* The mountpoint open method differs from the driver open method
   * because it receives (1) the vnode that contains the mountpoint
   * private data, (2) the relative path into the mountpoint, and (3)
   * information to manage privileges.
   */

  int     (*open)(struct file *filep, const char *relpath,
            int oflags, mode_t mode);

  /* The following methods must be identical in signature and position
   * because the struct file_operations and struct mountpt_operations are
   * treated like unions.
   */

  int     (*close)(struct file *filep);
  ssize_t (*read)(struct file *filep, char *buffer, size_t buflen);
  ssize_t (*write)(struct file *filep, const char *buffer,
            size_t buflen);
  off_t   (*seek)(struct file *filep, off_t offset, int whence);
  int     (*ioctl)(struct file *filep, int cmd, unsigned long arg);
  int     (*mmap)(struct file* filep, LosVmMapRegion *region);
  /* The two structures need not be common after this point. The following
   * are extended methods needed to deal with the unique needs of mounted
   * file systems.
   *
   * Additional open-file-specific mountpoint operations:
   */

  int     (*sync)(struct file *filep);
  int     (*dup)(const struct file *oldp, struct file *newp);
  int     (*fstat)(const struct file *filep, struct stat *buf);
  int     (*truncate)(struct file *filep, off_t length);

  /* Directory operations */

  int     (*opendir)(struct Vnode *mountpt, const char *relpath,
            struct fs_dirent_s *dir);
  int     (*closedir)(struct Vnode *mountpt,
            struct fs_dirent_s *dir);
  int     (*readdir)(struct Vnode *mountpt,
            struct fs_dirent_s *dir);
  int     (*rewinddir)(struct Vnode *mountpt,
            struct fs_dirent_s *dir);

  /* General volume-related mountpoint operations: */

  int     (*bind)(struct Vnode *blkdriver, const void *data, void **handle, const char *realpath);
  int     (*unbind)(void *handle, struct Vnode **blkdriver);
  int     (*statfs)(struct Vnode *mountpt, struct statfs *buf);
  int     (*virstatfs)(struct Vnode *mountpt, const char* relpath, struct statfs *buf);

  /* Operations on paths */

  int     (*unlink)(struct Vnode *mountpt, const char *relpath);
  int     (*mkdir)(struct Vnode *mountpt, const char *relpath,
            mode_t mode);
  int     (*rmdir)(struct Vnode *mountpt, const char *relpath);
  int     (*rename)(struct Vnode *mountpt, const char *oldrelpath,
            const char *newrelpath);
  int     (*stat)(struct Vnode *mountpt, const char *relpath,
            struct stat *buf);
  int     (*utime)(struct Vnode *mountpt, const char *relpath,
            const struct tm *times);
  int     (*chattr)(struct Vnode *mountpt, const char *relpath,
            struct IATTR *attr);
  loff_t   (*seek64)(struct file *filep, loff_t offset, int whence);
  int     (*getlabel)(void *handle, char* label);
  int     (*fallocate)(struct file *filep, int mode, off_t offset, off_t len);
  int     (*fallocate64)(struct file *filep, int mode, off64_t offset, off64_t len);
  int     (*truncate64)(struct file *filep, off64_t length);
  int     (*fscheck)(struct Vnode *mountpt, const char *relpath,
            struct fs_dirent_s *dir);
  int     (*map_pages)(LosVmMapRegion *region, LosVmPgFault *pgFault);
  ssize_t (*writepage)(struct file *filep, const char *buffer, size_t buflen);
  /* NOTE:  More operations will be needed here to support:  disk usage
   * stats file stat(), file attributes, file truncation, etc.
   */
};

struct drv_data
{
  const void *ops;
  mode_t mode;
  void *priv;
};

/* This structure is provided by a filesystem to describe a mount point.
 * Note that this structure differs from file_operations ONLY in the form of
 * the open method.  Once the file is opened, it can be accessed either as a
 * struct file_operations or struct mountpt_operations
 */

/* file mapped in VMM pages */
struct page_mapping {
  LOS_DL_LIST                           page_list;    /* all pages */
  SPIN_LOCK_S                           list_lock;    /* lock protecting it */
  LosMux                                mux_lock;     /* mutex lock */
  unsigned long                         nrpages;      /* number of total pages */
  unsigned long                         flags;
  Atomic                                ref;          /* reference counting */
  struct file                           *host;        /* owner of this mapping */
};

/* map: full_path(owner) <-> mapping */
struct file_map {
  LOS_DL_LIST                           head;
  LosMux                                lock;         /* lock to protect this mapping */
  struct page_mapping                   mapping;
  int                                   name_len;
  char                                  *rename;
  char                                  owner[0];     /* owner: full path of file */
};
#endif /* CONFIG_DISABLE_MOUNTPOINT */

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
#define _MAX_ENTRYLENGTH  16                                       /* MAX virtual partition name length */
#define _MAX_VIRVOLUMES   5                                        /* MAX virtual partition number */
typedef struct virtual_partition_info
{
  char *devpartpath;                                             /* need set virtual partition, e.g. /dev/mmcblk0p0 */
  int  virpartnum;                                               /* virtual partition numbers, MAX number is 5 */
  double virpartpercent[_MAX_VIRVOLUMES];                        /* every virtual partition percent,e.g 0.6,0.3,0.1 */
  char virpartname[_MAX_VIRVOLUMES][_MAX_ENTRYLENGTH + 1];       /* every virtual partition name, MAX length is 16 */
} virpartinfo;
#endif

struct fsmap_t
{
  const char                      *fs_filesystemtype;
  const struct                    MountOps *fs_mops;
  const BOOL                      is_mtd_support;
  const BOOL                      is_bdfs;
};

#define FSMAP_ENTRY(_l, _name, _mop, _is_mtd_support, _is_bdfs)                      \
struct fsmap_t _l LOS_HAL_TABLE_ENTRY(fsmap) =  \
{                                                       \
  _name,                                              \
  &_mop,                                               \
  _is_mtd_support,                                    \
  _is_bdfs                                                \
}

/* Named OS resources are also maintained by the VFS.  This includes:
 *
 *   - Named semaphores:     sem_open(), sem_close(), and sem_unlink()
 *   - POSIX Message Queues: mq_open() and mq_close()
 *   - Shared memory:        shm_open() and shm_unlink();
 *
 * These are a special case in that they do not follow quite the same
 * pattern as the other file system types in that they have operations.
 */

/* These are the various kinds of operations that can be associated with
 * an vnode.
 */

/* This structure represents one vnode in the NuttX pseudo-file system */

typedef enum mount_status
{
  STAT_UNMOUNTED = 0,
  STAT_MOUNTED,
} MOUNT_STATE;


#define FSNODE_SIZE(n) (sizeof(struct Vnode) + (n))

/* This is the underlying representation of an open file.  A file
 * descriptor is an index into an array of such types. The type associates
 * the file descriptor to the file state and to a set of vnode operations.
 */

struct file
{
  unsigned int         f_magicnum;  /* file magic number */
  int                  f_oflags;    /* Open mode flags */
  struct Vnode         *f_vnode;    /* Driver interface */
  loff_t               f_pos;       /* File position */
  unsigned long        f_refcount;  /* reference count */
  char                 *f_path;     /* File fullpath */
  void                 *f_priv;     /* Per file driver private data */
  const char           *f_relpath;  /* realpath */
  struct page_mapping  *f_mapping;  /* mapping file to memory */
  void                 *f_dir;      /* DIR struct for iterate the directory if open a directory */
  const struct file_operations_vfs *ops;
  int fd;
};

/* This defines a list of files indexed by the file descriptor */

#if CONFIG_NFILE_DESCRIPTORS > 0
struct filelist
{
  sem_t   fl_sem;               /* Manage access to the file list */
  struct file fl_files[CONFIG_NFILE_DESCRIPTORS];
};

extern struct filelist tg_filelist;
#endif

/* The following structure defines the list of files used for standard C I/O.
 * Note that NuttX can support the standard C APIs with or without buffering
 *
 * When buffering is used, the following describes the usage of the I/O buffer.
 * The buffer can be used for reading or writing -- but not both at the same time.
 * An fflush is implied between each change in direction of access.
 *
 * The field fs_bufread determines whether the buffer is being used for reading or
 * for writing as follows:
 *
 *              BUFFER
 *     +----------------------+ <- fs_bufstart Points to the beginning of the buffer.
 *     | WR: Buffered data    |                WR: Start of buffered write data.
 *     | RD: Already read     |                RD: Start of already read data.
 *     +----------------------+
 *     | WR: Available buffer | <- fs_bufpos   Points to next byte:
 *     | RD: Read-ahead data  |                WR: End+1 of buffered write data.
 *     |                      |                RD: Points to next char to return
 *     +----------------------+
 *     | WR: Available        | <- fs_bufread  Top+1 of buffered read data
 *     | RD: Available        |                WR: bufstart buffer used for writing.
 *     |                      |                RD: Pointer to last buffered read char+1
 *     +----------------------+
 *                              <- fs_bufend   Points to the end of the buffer+1
 */

#if CONFIG_NFILE_STREAMS > 0
struct file_struct
{
  int                fs_fd;        /* File descriptor associated with stream */
#if CONFIG_STDIO_BUFFER_SIZE > 0
  sem_t              fs_sem;       /* For thread safety */
  pid_t              fs_holder;    /* Holder of sem */
  int                fs_counts;    /* Number of times sem is held */
  unsigned char *fs_bufstart;  /* Pointer to start of buffer */
  unsigned char *fs_bufend;    /* Pointer to 1 past end of buffer */
  unsigned char *fs_bufpos;    /* Current position in buffer */
  unsigned char *fs_bufread;   /* Pointer to 1 past last buffered read char. */
#endif
  uint16_t           fs_oflags;    /* Open mode flags */
  uint8_t            fs_flags;     /* Stream flags */
#if CONFIG_NUNGET_CHARS > 0
  uint8_t            fs_nungotten; /* The number of characters buffered for ungetc */
  unsigned char      fs_ungotten[CONFIG_NUNGET_CHARS];
#endif
};

struct streamlist
{
  sem_t               sl_sem;   /* For thread safety */
  struct file_struct sl_streams[CONFIG_NFILE_STREAMS];
};

extern struct streamlist tg_streamlist;
#endif /* CONFIG_NFILE_STREAMS */

/****************************************************************************
 * Name: fs_initialize
 *
 * Description:
 *   This is called from the OS initialization logic to configure the file
 *   system.
 *
 ****************************************************************************/

void fs_initialize(void);

/****************************************************************************
 * Name: register_driver
 *
 * Description:
 *   Register a character driver vnode the pseudo file system.
 *
 * Input Parameters:
 *   path - The path to the vnode to create
 *   fops - The file operations structure
 *   mode - Access privileges (not used)
 *   priv - Private, user data that will be associated with the vnode.
 *
 * Returned Value:
 *   Zero on success (with the vnode point in 'vnode'); A negated errno
 *   value is returned on a failure (all error values returned by
 *   vnode_reserve):
 *
 *   EINVAL - 'path' is invalid for this operation
 *   EEXIST - An vnode already exists at 'path'
 *   ENOMEM - Failed to allocate in-memory resources for the operation
 *
 * Attention:
 *   This function should be called after los_vfs_init has been called.
 *   The parameter path must point a valid string, which end with the terminating null byte.
 *   The total length of parameter path must less than the value defined by PATH_MAX.
 *   The prefix of the parameter path must be /dev/.
 *   The fops must pointed the right functions, otherwise the system will crash when the device is being operated.
 *
 ****************************************************************************/

int register_driver(const char *path,
                    const struct file_operations_vfs *fops, mode_t mode,
                    void *priv);

/****************************************************************************
 * Name: register_blockdriver
 *
 * Description:
 *   Register a block driver vnode the pseudo file system.
 *
 * Attention:
 *   This function should be called after los_vfs_init has been called.
 *   The parameter path must point a valid string, which end with the terminating null byte.
 *   The length of parameter path must be less than the value defined by PATH_MAX.
 *   The prefix of the parameter path must be '/dev/'.
 *   The bops must pointed the right functions, otherwise the system will crash when the device is being operated.
 *
 * Input Parameters:
 *   path - The path to the vnode to create
 *   bops - The block driver operations structure
 *   mode - Access privileges (not used)
 *   priv - Private, user data that will be associated with the vnode.
 *
 * Returned Value:
 *   Zero on success (with the vnode point in 'vnode'); A negated errno
 *   value is returned on a failure (all error values returned by
 *   vnode_reserve):
 *
 *   EINVAL - 'path' is invalid for this operation
 *   EEXIST - An vnode already exists at 'path'
 *   ENOMEM - Failed to allocate in-memory resources for the operation
 *
 ****************************************************************************/

#ifndef CONFIG_DISABLE_MOUNTPOINT
int register_blockdriver(const char *path,
                         const struct block_operations *bops,
                         mode_t mode, void *priv);
#endif

/****************************************************************************
 * Name: unregister_driver
 *
 * Description:
 *   Remove the character driver vnode at 'path' from the pseudo-file system
 *
 * Returned Value:
 *   Zero on success (with the vnode point in 'vnode'); A negated errno
 *   value is returned on a failure (all error values returned by
 *   vnode_reserve):
 *
 *   EBUSY - Resource is busy ,not permit for this operation.
 *   ENOENT - 'path' is invalid for this operation.
 *
 * Attention:
 *   This function should be called after register_blockdriver has been called.
 *   The parameter path must point a valid string, which end with the terminating null byte.
 *   The total length of parameter path must less than the value defined by PATH_MAX.
 *   The block device node referred by parameter path must be really exist.
 ****************************************************************************/

int unregister_driver(const char *path);

/****************************************************************************
 * Name: unregister_blockdriver
 *
 * Description:
 *   Remove the block driver vnode at 'path' from the pseudo-file system
 *
 * Input Parameters:
 *   path - The path that the vnode to be destroyed.
 *
 * Returned Value:
 *   Zero on success (with the vnode point in 'vnode'); A negated errno
 *   value is returned on a failure (all error values returned by
 *   vnode_reserve):
 *
 *   EBUSY - Resource is busy ,not permit for this operation.
 *   ENOENT - 'path' is invalid for this operation.
 *
 * Attention:
 *   This function should be called after register_blockdriver has been called.
 *   The parameter path must point a valid string, which end with the terminating null byte.
 *   The total length of parameter path must less than the value defined by PATH_MAX.
 *   The block device node referred by parameter path must be really exist.
 *
 ****************************************************************************/

int unregister_blockdriver(const char *path);

/****************************************************************************
 * Name: files_initlist
 *
 * Description:
 *   Initializes the list of files for a new task
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
void files_initlist(struct filelist *list);
#endif

/****************************************************************************
 * Name: files_releaselist
 *
 * Description:
 *   Release a reference to the file list
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
void files_releaselist(struct filelist *list);
#endif

/****************************************************************************
 * Name: file_dup2
 *
 * Description:
 *   Assign an vnode to a specific files structure.  This is the heart of
 *   dup2.
 *
 *   Equivalent to the non-standard fs_dupfd2() function except that it
 *   accepts struct file instances instead of file descriptors and it does
 *   not set the errno variable.
 *
 * Returned Value:
 *   Zero (OK) is returned on success; a negated errno value is return on
 *   any failure.
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
int file_dup2(struct file *filep1, struct file *filep2);
#endif

/****************************************************************************
 * Name: fs_dupfd OR dup
 *
 * Description:
 *   Clone a file descriptor 'fd' to an arbitrary descriptor number (any value
 *   greater than or equal to 'minfd'). If socket descriptors are
 *   implemented, then this is called by dup() for the case of file
 *   descriptors.  If socket descriptors are not implemented, then this
 *   function IS dup().
 *
 *   This alternative naming is used when dup could operate on both file and
 *   socket descriptors to avoid drawing unused socket support into the link.
 *
 * Returned Value:
 *   fs_dupfd is sometimes an OS internal function and sometimes is a direct
 *   substitute for dup().  So it must return an errno value as though it
 *   were dup().
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
int fs_dupfd(int fd, int minfd);
#endif

/****************************************************************************
 * Name: file_dup
 *
 * Description:
 *   Equivalent to the non-standard fs_dupfd() function except that it
 *   accepts a struct file instance instead of a file descriptor and does
 *   not set the errno variable.
 *
 * Returned Value:
 *   Zero (OK) is returned on success; a negated errno value is returned on
 *   any failure.
 *
 ****************************************************************************/

int file_dup(struct file *filep, int minfd);

/****************************************************************************
 * Name: fs_dupfd2 OR dup2
 *
 * Description:
 *   Clone a file descriptor to a specific descriptor number. If socket
 *   descriptors are implemented, then this is called by dup2() for the
 *   case of file descriptors.  If socket descriptors are not implemented,
 *   then this function IS dup2().
 *
 *   This alternative naming is used when dup2 could operate on both file and
 *   socket descriptors to avoid drawing unused socket support into the link.
 *
 * Returned Value:
 *   fs_dupfd2 is sometimes an OS internal function and sometimes is a direct
 *   substitute for dup2().  So it must return an errno value as though it
 *   were dup2().
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
int fs_dupfd2(int fd1, int fd2);
#endif


/****************************************************************************
 * Name: open_blockdriver
 *
 * Description:
 *   Return the vnode of the block driver specified by 'pathname'
 *
 * Input Parameters:
 *   pathname - the full path to the block driver to be opened
 *   mountflags - if MS_RDONLY is not set, then driver must support write
 *     operations (see include/sys/mount.h)
 *   ppvnode - address of the location to return the vnode reference
 *
 * Returned Value:
 *   Returns zero on success or a negated errno on failure:
 *
 *   EINVAL  - pathname or pvnode is NULL
 *   ENOENT  - No block driver of this name is registered
 *   ENOTBLK - The vnode associated with the pathname is not a block driver
 *   EACCESS - The MS_RDONLY option was not set but this driver does not
 *     support write access
 *
 * Aattention:
 *   The parameter path must point a valid string, which end with the terminating null byte.
 *   The total length of parameter path must less than the value defined by PATH_MAX.
 *   The parameter ppvnode must point a valid memory, which size must be enough for storing struct Vnode.

 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
int open_blockdriver(const char *pathname, int mountflags,
                     struct Vnode **ppvnode);
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: find_blockdriver
 *
 * Description:
 *   Return the inode of the block driver specified by 'pathname'
 *
 * Input Parameters:
 *   pathname   - The full path to the block driver to be located
 *   mountflags - If MS_RDONLY is not set, then driver must support write
 *                operations (see include/sys/mount.h)
 *   ppinode    - Address of the location to return the inode reference
 *
 * Returned Value:
 *   Returns zero on success or a negated errno on failure:
 *
 *   ENOENT  - No block driver of this name is registered
 *   ENOTBLK - The inode associated with the pathname is not a block driver
 *   EACCESS - The MS_RDONLY option was not set but this driver does not
 *             support write access
 *
 ****************************************************************************/

int find_blockdriver(const char *pathname, int mountflags,
                     struct Vnode **vpp);


/****************************************************************************
 * Name: close_blockdriver
 *
 * Description:
 *   Call the close method and release the vnode
 *
 * Input Parameters:
 *   vnode - reference to the vnode of a block driver opened by open_blockdriver
 *
 * Returned Value:
 *   Returns zero on success or a negated errno on failure:
 *
 *   EINVAL  - vnode is NULL
 *   ENOTBLK - The vnode is not a block driver
 *
 * Attention:
 *   This function should be called after open_blockdriver has been called.
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
int close_blockdriver(struct Vnode *vnode);
#endif

/****************************************************************************
 * Name: fs_ioctl
 *
 * Description:
 *   Perform device specific operations.
 *
 * Input Parameters:
 *   fd       File/socket descriptor of device
 *   req      The ioctl command
 *   arg      The argument of the ioctl cmd
 *
 * Returned Value:
 *   >=0 on success (positive non-zero values are cmd-specific)
 *   -1 on failure with errno set properly:
 *
 *   EBADF
 *     'fd' is not a valid descriptor.
 *   EFAULT
 *     'arg' references an inaccessible memory area.
 *   EINVAL
 *     'cmd' or 'arg' is not valid.
 *   ENOTTY
 *     'fd' is not associated with a character special device.
 *   ENOTTY
 *      The specified request does not apply to the kind of object that the
 *      descriptor 'fd' references.
 *
 ****************************************************************************/

#ifdef CONFIG_LIBC_IOCTL_VARIADIC
int fs_ioctl(int fd, int req, unsigned long arg);
#endif

/****************************************************************************
 * Name: fs_fdopen
 *
 * Description:
 *   This function does the core operations for fopen and fdopen.  It is
 *   used by the OS to clone stdin, stdout, stderr
 *
 ****************************************************************************/

#if CONFIG_NFILE_STREAMS > 0
struct tcb_s; /* Forward reference */
struct file_struct *fs_fdopen(int fd, int oflags);
#endif

/****************************************************************************
 * Name: lib_flushall
 *
 * Description:
 *   Called either (1) by the OS when a task exits, or (2) from fflush()
 *   when a NULL stream argument is provided.
 *
 ****************************************************************************/

#if CONFIG_NFILE_STREAMS > 0
int lib_flushall(struct streamlist *list);
#endif

/****************************************************************************
 * Name: lib_sendfile
 *
 * Description:
 *   Transfer a file
 *
 ****************************************************************************/

#ifdef CONFIG_NET_SENDFILE
ssize_t lib_sendfile(int outfd, int infd, off_t *offset, size_t count);
#endif

/****************************************************************************
 * Name: fs_getfilep
 *
 * Description:
 *   Given a file descriptor, return the corresponding instance of struct
 *   file.  NOTE that this function will currently fail if it is provided
 *   with a socket descriptor.
 *
 * Input Parameters:
 *   fd    - The file descriptor
 *   filep - The location to return the struct file instance
 *
 * Returned Value:
 *   Zero (OK) is returned on success; a negated errno value is returned on
 *   any failure.
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
int fs_getfilep(int fd, struct file **filep);
#endif

/****************************************************************************
 * Name: file_read
 *
 * Description:
 *   file_read() is an internal OS interface.  It is functionally similar to
 *   the standard read() interface except:
 *
 *    - It does not modify the errno variable,
 *    - It is not a cancellation point,
 *    - It does not handle socket descriptors, and
 *    - It accepts a file structure instance instead of file descriptor.
 *
 * Input Parameters:
 *   filep  - File structure instance
 *   buf    - User-provided to save the data
 *   nbytes - The maximum size of the user-provided buffer
 *
 * Returned Value:
 *   The positive non-zero number of bytes read on success, 0 on if an
 *   end-of-file condition, or a negated errno value on any failure.
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
ssize_t file_read(struct file *filep, void *buf, size_t nbytes);
#endif

/****************************************************************************
 * Name: file_write
 *
 * Description:
 *   Equivalent to the standard write() function except that is accepts a
 *   struct file instance instead of a file descriptor.  Currently used
 *   only by aio_write();
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
ssize_t file_write(struct file *filep, const void *buf, size_t nbytes);
#endif

/****************************************************************************
 * Name: file_pread
 *
 * Description:
 *   Equivalent to the standard pread function except that is accepts a
 *   struct file instance instead of a file descriptor.  Currently used
 *   only by aio_read();
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
ssize_t file_pread(struct file *filep, void *buf, size_t nbytes,
                   off_t offset);
#endif

/****************************************************************************
 * Name: file_pwrite
 *
 * Description:
 *   Equivalent to the standard pwrite function except that is accepts a
 *   struct file instance instead of a file descriptor.  Currently used
 *   only by aio_write();
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
ssize_t file_pwrite(struct file *filep, const void *buf,
                    size_t nbytes, off_t offset);
#endif

/****************************************************************************
 * Name: file_seek
 *
 * Description:
 *   Equivalent to the standard lseek() function except that is accepts a
 *   struct file instance instead of a file descriptor.  Currently used
 *   only by net_sendfile()
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
off_t file_seek(struct file *filep, off_t offset, int whence);
#endif

/****************************************************************************
 * Name: file_fsync
 *
 * Description:
 *   Equivalent to the standard fsync() function except that is accepts a
 *   struct file instance instead of a file descriptor and it does not set
 *   the errno variable.
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
int file_fsync(struct file *filep);
#endif

/****************************************************************************
 * Name: file_vfcntl
 *
 * Description:
 *   Similar to the standard vfcntl function except that is accepts a struct
 *   struct file instance instead of a file descriptor.
 *
 * Input Parameters:
 *   filep - Instance for struct file for the opened file.
 *   cmd   - Indentifies the operation to be performed.
 *   ap    - Variable argument following the command.
 *
 * Returned Value:
 *   The nature of the return value depends on the command.  Non-negative
 *   values indicate success.  Failures are reported as negated errno
 *   values.
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
int file_vfcntl(struct file *filep, int cmd, va_list ap);
#endif

/****************************************************************************
 * Name: file_seek64
 *
 * Description:
 *   Equivalent to the standard lseek64() function except that is accepts a
 *   struct file instance instead of a file descriptor.  Currently used
 *   only by net_sendfile()
 *
 ****************************************************************************/

#if CONFIG_NFILE_DESCRIPTORS > 0
off64_t file_seek64(struct file *filep, off64_t offset, int whence);
#endif

/****************************************************************************
 * Name: files_allocate
 *
 * Description:
 *   Allocate a struct files instance and associate it with an vnode instance.
 *   Returns the file descriptor == index into the files array.
 *
 ****************************************************************************/

int files_allocate(struct Vnode *vnode, int oflags, off_t pos,void *priv, int minfd);

/****************************************************************************
 * Name: files_close
 *
 * Description:
 *   Close an vnode (if open)
 *
 * Assumuptions:
 *   Caller holds the list semaphore because the file descriptor will be freed.
 *
 ****************************************************************************/

int files_close(int fd);

/****************************************************************************
 * Name: files_release
 *
 * Assumuptions:
 *   Similar to files_close().  Called only from open() logic on error
 *   conditions.
 *
 ****************************************************************************/

void files_release(int fd);

/****************************************************************************
 * Name: files_initialize
 *
 * Description:
 *   This is called from the FS initialization logic to configure the files.
 *
 ****************************************************************************/

void weak_function files_initialize(void);

int vfs_normalize_path(const char *directory, const char *filename, char **pathname);
int vfs_normalize_pathat(int fd, const char *filename, char **pathname);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* __INCLUDE_FS_FS_H */
