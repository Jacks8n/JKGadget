#pragma once

#ifdef RFLITE_DISABLE
#define RFLITE_DISABLE_STATIC
#endif

#include "rflite/internal/refl_dynamic.h"

#ifndef RFLITE_DISABLE
#define RFLITE_VERSION 0x00000000u

#undef RFLITE_NS
#undef RFLITE_IMPL_NS
#endif