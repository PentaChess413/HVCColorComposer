/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "gmp.h"
#include "gmp-impl.h"
#include <string.h>

/* Special realloc that zeroes the old memory before freeing it. */

void *_mp_allocate(size_t new_size)
{
	return new char[new_size];
}

/* Special realloc that zeroes the old memory before freeing it. */

void *_mp_reallocate(void *ptr, size_t old_size, size_t new_size)
{
	void	*p = _mp_allocate(new_size);
	int		s = old_size;

	if (old_size > new_size)
		s = new_size;

	memcpy(p, ptr, s);
	_mp_free(ptr, old_size);

	return p;
}

/* Special free that zeroes the memory before freeing it. */

void _mp_free(void *ptr, size_t size)
{
	memset(ptr, 0, size);
	delete[] reinterpret_cast<char*> (ptr);
}
