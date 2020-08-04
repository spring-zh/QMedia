/*
 * copyright (c) 2004 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file bitstream.h
 * bitstream api header.
 */

#ifndef FS_IO_H
#define FS_IO_H

#define av_always_inline inline
#define attribute_deprecated

#include <inttypes.h>
#include <stdlib.h>

#ifdef ROCKBOX_BIG_ENDIAN
#define WORDS_BIGENDIAN
#endif

#ifdef __cplusplus
extern "C"{
#endif

typedef struct FSIOContext {
	void* handle;      //          
	int   iomode;      //io类型 1文件/0内存

	uint32_t curr_pos; //io当前指向位置
	uint32_t buff_size;//当前已使用缓存大小
	uint32_t max_size;

	int writeable;        //1可写 0不可写
} FSIOContext;

extern int fsio_init_context(FSIOContext *s, void* input, uint32_t insize, int mode);

extern uint32_t fsio_write(FSIOContext *s, const uint8_t *buf, uint32_t size);

extern void fsio_w8(FSIOContext *s, int b);
extern void fsio_wl64(FSIOContext *s, uint64_t val);
extern void fsio_wb64(FSIOContext *s, uint64_t val);
extern void fsio_wl16(FSIOContext *s, unsigned int val);
extern void fsio_wb16(FSIOContext *s, unsigned int val);
extern void fsio_wl32(FSIOContext *s, unsigned int val);
extern void fsio_wb32(FSIOContext *s, unsigned int val);
extern void fsio_wl24(FSIOContext *s, unsigned int val);
extern void fsio_wb24(FSIOContext *s, unsigned int val);

extern int fsio_put_str(FSIOContext *s, const char *str);

extern int32_t fsio_seek(FSIOContext *s, int32_t offset, int whence);
extern int32_t fsio_skip(FSIOContext *s, int32_t offset);
extern int32_t fsio_tell(FSIOContext *s);

extern uint32_t fsio_read(FSIOContext *s, const uint8_t *buf, uint32_t size);

extern int fsio_r8(FSIOContext *s);
extern unsigned int fsio_rl16(FSIOContext *s);
extern unsigned int fsio_rl24(FSIOContext *s);
extern unsigned int fsio_rl32(FSIOContext *s);
extern uint64_t fsio_rl64(FSIOContext *s);
extern unsigned int fsio_rb16(FSIOContext *s);
extern unsigned int fsio_rb24(FSIOContext *s);
extern unsigned int fsio_rb32(FSIOContext *s);
extern uint64_t fsio_rb64(FSIOContext *s);

#ifdef __cplusplus
}
#endif

#endif /* FS_IO_H */
