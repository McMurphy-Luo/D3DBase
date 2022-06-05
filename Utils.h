#ifndef D3D_BASE_UTILS_H_
#define D3D_BASE_UTILS_H_

#include "Define.h"
#include <sstream>
#include "Utf8String.h"

template <typename Func, typename Obj>
auto bind_front(Func f, Obj* obj) {
  return [=](auto&&... args) {
    return (obj->*f)(std::forward<decltype(args)>(args)...);
  };
}

#ifdef D3D_BASE_DEBUG
#define DEBUG_LOG(x) \
  do { \
    std::basic_ostringstream<Utf8String::value_type> debug_stream; \
    debug_stream << x << std::endl; \
    OutputDebugStringW(Utf8StringToWindowsString(debug_stream.str()).c_str()); \
  } while(0)
#else //D3D_BASE_DEBUG
#define DEBUG_LOG(x)
#endif //D3D_BASE_DEBUG

#endif // D3D_BASE_UTILS_H_
