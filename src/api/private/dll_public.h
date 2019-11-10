#pragma once

// https://gcc.gnu.org/wiki/Visibility

#if defined(__BUILDING_LIB__)
#define DLL_PUBLIC
#define DLL_LOCAL
#else
#if defined(_MSC_VER) || defined(__CYGWIN__)
#ifdef __BUILDING_DLL__
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllexport))
#else
#define DLL_PUBLIC __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllimport))
#else
#define DLL_PUBLIC __declspec(dllimport)
#endif
#endif
#define DLL_LOCAL
#else
#if __GNUC__ >= 4
#define DLL_PUBLIC __attribute__ ((visibility ("default")))
#define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define DLL_PUBLIC
#define DLL_LOCAL
#endif
#endif
#endif

#if !defined(EXTERN_C)
#if defined(__cplusplus)
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif
#endif

#if !defined(STDCALL)
#if defined(_WIN32)
#define STDCALL __stdcall
#else
#define STDCALL
#endif
#endif

#if defined(_MSC_VER)
#define VGA_C_API_DECL(ret_type) EXTERN_C DLL_PUBLIC ret_type STDCALL
#define VGA_CXX_API_DECL(ret_type) DLL_PUBLIC ret_type STDCALL
#else
#define VGA_C_API_DECL(ret_type) EXTERN_C DLL_PUBLIC STDCALL ret_type
#define VGA_CXX_API_DECL(ret_type) DLL_PUBLIC STDCALL ret_type
#endif
#define VGA_API_IMPL(ret_type) ret_type STDCALL
