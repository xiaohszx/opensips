/*
 * Copyright (C) 2014-2019 OpenSIPS Solutions
 *
 * This file is part of opensips, a free SIP server.
 *
 * opensips is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * opensips is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef mem_common_h
#define mem_common_h

#include "meminfo.h"

#if !defined(F_MALLOC) && !defined(QM_MALLOC) && !defined(HP_MALLOC)
#error "no memory allocator selected"
#endif

extern int mem_warming_enabled;
extern char *mem_warming_pattern_file;
extern int mem_warming_percentage;
extern enum osips_mm mem_allocator;

enum osips_mm {
	MM_NONE,
	MM_F_MALLOC,
	MM_QM_MALLOC,
	MM_HP_MALLOC,
#ifdef DBG_MALLOC
	MM_F_MALLOC_DBG,
	MM_QM_MALLOC_DBG,
	MM_HP_MALLOC_DBG,
#endif
};

extern void *mem_block;
extern void *shm_block;

#ifdef DBG_MALLOC
typedef void *(*osips_malloc_f) (void *block, unsigned long size,
                      const char *file, const char *func, unsigned int line);
typedef void *(*osips_realloc_f) (void *block, void *ptr, unsigned long size,
                      const char *file, const char *func, unsigned int line);
typedef void (*osips_free_f) (void *block, void *ptr,
                      const char *file, const char *func, unsigned int line);
#else
typedef void *(*osips_malloc_f) (void *block, unsigned long size);
typedef void *(*osips_realloc_f) (void *block, void *ptr, unsigned long size);
typedef void (*osips_free_f) (void *block, void *ptr);
#endif

typedef void (*osips_mem_info_f) (void *block, struct mem_info *i);
typedef void (*osips_mem_status_f) (void *block);
typedef unsigned long (*osips_get_mmstat_f) (void *block);

#if defined F_MALLOC
#include "f_malloc.h"
#endif

#if defined QM_MALLOC
#include "q_malloc.h"
#endif

#if defined HP_MALLOC
#include "hp_malloc.h"
#endif

#define oom_errorf \
	"not enough free %s memory (%lu bytes left, need %lu), " \
	"please increase the \"-%s\" command line parameter!\n"

#define oom_nostats_errorf \
	"not enough free %s memory (need %lu), please increase the \"-%s\" " \
	"command line parameter!\n"

/* if exactly an allocator was selected, let's inline it! */
#if ((!defined QM_MALLOC && !defined HP_MALLOC) || \
	 (!defined F_MALLOC && !defined HP_MALLOC) || \
	 (!defined F_MALLOC && !defined QM_MALLOC))
#define INLINE_ALLOC
#endif

#ifdef DBG_MALLOC
#define check_double_free(ptr, frag, block) \
	do { \
		if (frag_is_free(frag)) { \
			LM_CRIT("freeing already freed %s pointer (%p), first free: " \
			        "%s: %s(%ld) - aborting!\n", (block)->name, ptr, \
			        (frag)->file, (frag)->func, (frag)->line); \
			abort(); \
		} \
	} while (0)
#else
#define check_double_free(ptr, frag, block)
#endif

#endif
