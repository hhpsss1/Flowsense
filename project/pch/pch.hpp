#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#ifndef PHNT_VERSION
#define PHNT_VERSION PHNT_THRESHOLD
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmicrosoft-enum-forward-reference"
#endif

#include <external/phnt/phnt_windows.h>
#include <external/phnt/phnt.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <external/inline-syscall/inline_syscall.hpp>

#include <cstdint>
#include <string>
#include <cmath>
#include <numbers>
#include <format>
#include <fstream>
#include <functional>
#include <chrono>
#include <variant>
#include <optional>
#include <random>

#include <array>
#include <span>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>

#include <memory>

#include <numeric>
#include <ranges>
#include <algorithm>

#include <atomic>
#include <mutex>
#include <shared_mutex>

#include <external/xdraw/xui/xui.hpp>
#include <external/zydis/zydis.h>
#include <external/lz4/lz4.h>

#define JM_XORSTR_DISABLE_AVX_INTRINSICS
#include <external/xorstr.hpp>

#include <external/poly2d.hpp>
#include <external/bc7.hpp>

#include <utilities/animation.hpp>
#include <utilities/cstypes.hpp>
#include <utilities/fnv1a.hpp>
#include <utilities/addresses/addresses.hpp>

extern "C" int __stdcall _CRT_INIT( HMODULE, DWORD, LPVOID );
