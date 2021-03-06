/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * Copyright (c) 2020, Florida State University. Contributions from
 * the Computer Architecture and Systems Research Laboratory (CASTL)
 * at the Department of Computer Science.
 *
 * LLNL-CODE-805021. All rights reserved.
 * 
 * This is the license for Direct-FUSE.
 * For details, see https://github.com/llnl/direct-fuse
 * Please read https://github.com/llnl/direct-fuse/LICENSE for full license text.
 */



/*
 *    This Cplant(TM) source code is the property of Sandia National
 *    Laboratories.
 *
 *    This Cplant(TM) source code is copyrighted by Sandia National
 *    Laboratories.
 *
 *    The redistribution of this Cplant(TM) source code is subject to the
 *    terms of the GNU Lesser General Public License
 *    (see cit/LGPL or http://www.gnu.org/licenses/lgpl.html)
 *
 *    Cplant(TM) Copyright 1998-2003 Sandia Corporation. 
 *    Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
 *    license for use of this work by or on behalf of the US Government.
 *    Export of this program may require a license from the United States
 *    Government.
 */

/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Questions or comments about this library should be sent to:
 *
 * Lee Ward
 * Sandia National Laboratories, New Mexico
 * P.O. Box 5800
 * Albuquerque, NM 87185-1110
 *
 * lee@sandia.gov
 */

#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/queue.h>

#include "sysio.h"
#include "inode.h"
#include "file.h"
#include "sysio-symbols.h"

static int
_do_chown(struct pnode *pno, struct inode *ino, uid_t owner, gid_t group)
{
	int	err;
	struct intnl_stat stbuf;
	unsigned mask;

	(void )memset(&stbuf, 0, sizeof(struct intnl_stat));
	mask = 0;
	if (owner != (uid_t )-1) {
		stbuf.st_uid = owner;
		mask |= SETATTR_UID;
	}
	if (group != (gid_t )-1) {
		stbuf.st_gid = group;
		mask |= SETATTR_GID;
	}
	err = _sysio_setattr(pno, ino, mask, &stbuf);
	return err;
}

int
SYSIO_INTERFACE_NAME(chown)(const char *path, uid_t owner, gid_t group)
{
	int	err;
	struct pnode *pno;
	SYSIO_INTERFACE_DISPLAY_BLOCK;

	SYSIO_INTERFACE_ENTER;
	err = _sysio_namei(_sysio_cwd, path, 0, NULL, &pno);
	if (err)
		goto out;

	err = _do_chown(pno, pno->p_base->pb_ino, owner, group);
	P_RELE(pno);
out:
	SYSIO_INTERFACE_RETURN(err ? -1 : 0, err);
}

#ifdef REDSTORM
#undef __chown
sysio_sym_weak_alias(SYSIO_INTERFACE_NAME(chown), 
		     PREPEND(__, SYSIO_INTERFACE_NAME(chown)))
#endif

int
SYSIO_INTERFACE_NAME(fchown)(int fd, uid_t owner, gid_t group)
{
	int	err;
	struct file *fil;
	SYSIO_INTERFACE_DISPLAY_BLOCK;

	SYSIO_INTERFACE_ENTER;
	err = 0;
	fil = _sysio_fd_find(fd);
	if (!fil) {
		err = -EBADF;
		goto out;
	}

	err = _do_chown(NULL, fil->f_ino, owner, group);
out:
	SYSIO_INTERFACE_RETURN(err ? -1 : 0, err);
}

#ifdef REDSTORM
#undef __fchown
sysio_sym_weak_alias(SYSIO_INTERFACE_NAME(fchown), 
		     PREPEND(__, SYSIO_INTERFACE_NAME(fchown)))
#endif

