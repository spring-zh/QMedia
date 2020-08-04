
#include "fs_io.h"
#include <stdio.h>
#include <string.h>

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

int fsio_init_context(FSIOContext *s, void* input, uint32_t insize, int mode)
{
	s->buff_size = s->curr_pos = 0;
	s->iomode = mode;
	s->writeable = 1;
	s->handle = input;
	if (mode == 0)
	{
		s->max_size = insize;
	}
	else
	{
		s->curr_pos = ftell(s->handle);
		fseek(s->handle, 0, SEEK_END);
		s->buff_size = s->max_size = ftell(s->handle);
		fseek(s->handle, s->curr_pos, SEEK_SET);
	}

	return 0;
}

uint32_t fsio_write(FSIOContext *s, const uint8_t *buf, uint32_t size)
{
	uint8_t* curr_prt;
	uint32_t ret;
	if (s->iomode == 0) {
		if (s->curr_pos + size > s->max_size)
			return 0;

		curr_prt = (uint8_t*)s->handle + s->curr_pos;
		memcpy(curr_prt, buf, size);
		ret = size;
	}
	else if (s->iomode == 1)
		ret = fwrite(buf, size, 1, (FILE*)s->handle);

	s->curr_pos += size;
	s->buff_size = MAX(s->curr_pos, s->buff_size);
	return ret;
}
void fsio_w8(FSIOContext *s, int b)
{
	uint8_t* curr_prt;
	if (s->iomode == 0)
	{
		curr_prt = (uint8_t*)(s->handle) + s->curr_pos;
		*curr_prt = b;
	}
	else if (s->iomode == 1)
	{
		fputc(b, (FILE*)s->handle);
	}
	s->curr_pos++;
	s->buff_size = MAX(s->curr_pos, s->buff_size);
}

void fsio_wl64(FSIOContext *s, uint64_t val)
{
	fsio_wl32(s, (uint32_t)(val & 0xffffffff));
	fsio_wl32(s, (uint32_t)(val >> 32));
}

void fsio_wb64(FSIOContext *s, uint64_t val)
{
	fsio_wb32(s, (uint32_t)(val >> 32));
	fsio_wb32(s, (uint32_t)(val & 0xffffffff));
}

void fsio_wl16(FSIOContext *s, unsigned int val)
{
	fsio_w8(s, (uint8_t)val);
	fsio_w8(s, (int)val >> 8);
}

void fsio_wb16(FSIOContext *s, unsigned int val)
{
	fsio_w8(s, (int)val >> 8);
	fsio_w8(s, (uint8_t)val);
}

void fsio_wl32(FSIOContext *s, unsigned int val)
{
	fsio_w8(s, (uint8_t)val);
	fsio_w8(s, (uint8_t)(val >> 8));
	fsio_w8(s, (uint8_t)(val >> 16));
	fsio_w8(s, val >> 24);
}

void fsio_wb32(FSIOContext *s, unsigned int val)
{
	fsio_w8(s, val >> 24);
	fsio_w8(s, (uint8_t)(val >> 16));
	fsio_w8(s, (uint8_t)(val >> 8));
	fsio_w8(s, (uint8_t)val);
}

void fsio_wl24(FSIOContext *s, unsigned int val)
{
	fsio_wl16(s, val & 0xffff);
	fsio_w8(s, (int)val >> 16);
}

void fsio_wb24(FSIOContext *s, unsigned int val)
{
	fsio_wb16(s, (int)val >> 8);
	fsio_w8(s, (uint8_t)val);
}

int fsio_put_str(FSIOContext *s, const char *str)
{
	return fsio_write(s, (const unsigned char *)str, strlen(str));
}

int32_t fsio_seek(FSIOContext *s, int32_t offset, int whence)
{

	switch (whence)
	{
		case SEEK_SET:
			s->curr_pos = offset;
			break;
		case SEEK_CUR:
			s->curr_pos += offset;
			break;
		case SEEK_END:
			s->curr_pos = s->buff_size;
			break;
	}

	s->buff_size = MAX(s->curr_pos, s->buff_size);
	if (s->iomode == 1)
		return fseek((FILE*)s->handle, offset, whence);

	return s->curr_pos;
}
int32_t fsio_skip(FSIOContext *s, int32_t offset)
{
	return fsio_seek(s, offset, SEEK_CUR);
}
int32_t fsio_tell(FSIOContext *s)
{
	if (s->iomode == 0)
		return s->curr_pos;
	else if (s->iomode == 1)
		return ftell((FILE*)s->handle);

	return -1;
}

int fsio_r8(FSIOContext *s)
{
	int r8;
	uint8_t* curr_prt;
	if (s->curr_pos >= s->max_size)
		return 0;
	if (s->iomode == 0)
	{
		r8 = *((uint8_t*)(s->handle) + s->curr_pos);
	}
	else if (s->iomode == 1)
	{
		r8 = fgetc((FILE*)s->handle);
	}
	s->curr_pos++;
	return r8;
}

unsigned int fsio_rl16(FSIOContext *s)
{
	unsigned int val;
	val = fsio_r8(s);
	val |= fsio_r8(s) << 8;
	return val;
}

unsigned int fsio_rl24(FSIOContext *s)
{
	unsigned int val;
	val = fsio_rl16(s);
	val |= fsio_r8(s) << 16;
	return val;
}

unsigned int fsio_rl32(FSIOContext *s)
{
	unsigned int val;
	val = fsio_rl16(s);
	val |= fsio_rl16(s) << 16;
	return val;
}

uint64_t fsio_rl64(FSIOContext *s)
{
	uint64_t val;
	val = (uint64_t)fsio_rl32(s);
	val |= (uint64_t)fsio_rl32(s) << 32;
	return val;
}

unsigned int fsio_rb16(FSIOContext *s)
{
	unsigned int val;
	val = fsio_r8(s) << 8;
	val |= fsio_r8(s);
	return val;
}

unsigned int fsio_rb24(FSIOContext *s)
{
	unsigned int val;
	val = fsio_rb16(s) << 8;
	val |= fsio_r8(s);
	return val;
}
unsigned int fsio_rb32(FSIOContext *s)
{
	unsigned int val;
	val = fsio_rb16(s) << 16;
	val |= fsio_rb16(s);
	return val;
}

uint64_t fsio_rb64(FSIOContext *s)
{
	uint64_t val;
	val = (uint64_t)fsio_rb32(s) << 32;
	val |= (uint64_t)fsio_rb32(s);
	return val;
}

uint32_t fsio_read(FSIOContext *s, const uint8_t *buf, uint32_t size)
{
	uint8_t* curr_prt;
	uint32_t ret;
	uint32_t len_to_read = size;
	if (s->iomode == 0) {
		if (s->curr_pos + len_to_read > s->max_size)
			len_to_read = s->max_size - s->curr_pos;

		curr_prt = (uint8_t*)s->handle + s->curr_pos;
		memcpy(buf, curr_prt, len_to_read);
		ret = len_to_read;
	}
	else if (s->iomode == 1){
		ret = fread(buf, 1, size, (FILE*)s->handle);
		len_to_read = ret;
	}

	s->curr_pos += len_to_read;
	return ret;
}