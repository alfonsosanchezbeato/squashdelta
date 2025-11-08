/**
 * SquashFS delta tools
 * (c) Canonical 2025
 * Released under the terms of the 2-clause BSD license
 */

#pragma once
#ifndef SDT_DELTA_HXX
#define SDT_DELTA_HXX 1

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <cstdlib>

extern "C"
{
#ifdef HAVE_STDINT_H
#	include <stdint.h>
#endif
}


class XDelta
{
public:
	int delta(const char *path1, const char *path2, int out_fd);
};

#endif /*!SDT_DELTA_HXX*/
