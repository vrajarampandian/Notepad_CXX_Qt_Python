#pragma once

// ── MSVC + Python 3.12 Debug Build Fix ───────────────────────────────────
//
// Problem: Python debug headers look for python312_d.dll which doesn't
//          exist in standard Python installs.
//          Naive fix (#undef _DEBUG) changes the C++ runtime library and
//          causes LNK2038 mismatches.
//
// Correct fix: Define Py_NO_ENABLE_SHARED and link explicitly to the
//              release Python lib regardless of build config.
//              This keeps the C++ runtime consistent (_MDd in debug)
//              while using the release Python DLL.
// ─────────────────────────────────────────────────────────────────────────

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4100 4127 4244 4267 4512 4800 4996)

// Tell Python headers: do not try to auto-link debug variant
// This must come BEFORE any Python or pybind11 include
#  ifdef _DEBUG
#    define PYBIND11_NOINLINE_OVERRIDE
     // Redirect Python's debug auto-link to release lib
#    pragma comment(lib, "python312.lib")
#  endif
#endif

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#ifdef _MSC_VER
#  pragma warning(pop)
#endif
