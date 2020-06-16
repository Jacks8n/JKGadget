#pragma once

// Options:
//  -   RFLITE_PROJECT_NAME: Modify the macro if the default name conflicts
//  -   RFLITE_PREPROCESS_ONLY: Prevent including stl headers, may be helpful when inspecting
//      expanded macros
//  -   RFLITE_DYNAMIC_DISABLE: Define the macro to disable dynamic reflection

#define RFLITE_PROJECT_NAME rflite

#define RFLITE_NS      namespace RFLITE_PROJECT_NAME
#define RFLITE_IMPL_NS namespace RFLITE_PROJECT_NAME::impl

#define RFLITE      ::RFLITE_PROJECT_NAME::
#define RFLITE_IMPL ::RFLITE_PROJECT_NAME::impl::

//#define RFLITE_PREPROCESS_ONLY
//#define RFLITE_DYNAMIC_DISABLE