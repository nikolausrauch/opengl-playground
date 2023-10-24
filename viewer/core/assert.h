#pragma once

#include "log.h"


#if defined(PLATFOMR_DEBUG)

#define platform_assert(check, ...) { if(!(check)) { platform_log(core::log::level::error, __VA_ARGS__); platform_break(); } }

#else

#define platform_assert(check, ...)

#endif
