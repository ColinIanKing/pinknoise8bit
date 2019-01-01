/*
 * Copyright (C) 2019 Colin Ian King, colin.i.king@gmail.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
 *  A tiny 8 bit pink noise generator
 */
#define _GNU_SOURCE         /* See feature_test_macros(7) */

#define PINK_MAX_ROWS   (26)
#define PINK_BITS       (17)
#define PINK_SHIFT      ((sizeof(uint64_t) * 8) - PINK_BITS)
#define PINK_BIAS	(128.0)

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>

static uint32_t w, z;

static uint32_t mwc32(void)
{
        z = 36969 * (z & 65535) + (z >> 16);
        w = 18000 * (w & 65535) + (w >> 16);
        return (z << 16) + w;
}

static uint64_t mwc64(void)
{
        return (((uint64_t)mwc32()) << 32) | mwc32();
}

/*
 * output 8 bit pink noise using
 * the Gardner method with the McCartney
 * selection tree optimization
 */
int main(void)
{
	size_t i;
	size_t idx = 0;
	const size_t mask = (1 << PINK_MAX_ROWS) - 1;
	uint64_t sum = 0;
	const uint64_t max = (PINK_MAX_ROWS + 1) * (1 << (PINK_BITS - 1));
	static uint64_t rows[PINK_MAX_ROWS];
	const float scalar = 256.0 / max;
	uint8_t buffer[1024];

	w = syscall(__NR_getpid);
	z = syscall(__NR_time, NULL);

	do {
		for (i =0; i < sizeof(buffer); i++) {
			int64_t rnd;

			idx = (idx + 1) & mask;
			if (idx) {
				size_t j = __builtin_ctz(idx);
	
				sum -= rows[j];
				rnd = (int64_t)mwc64() >> PINK_SHIFT;
				sum += rnd;
				rows[j] = rnd;
			}
			rnd = (int64_t)mwc64() >> PINK_SHIFT;
			buffer[i] = (int)((scalar * ((int64_t)sum + rnd)) + PINK_BIAS);
		}
	} while (syscall(__NR_write, 1, buffer, sizeof(buffer)) == sizeof(buffer));
	return 0;
}
