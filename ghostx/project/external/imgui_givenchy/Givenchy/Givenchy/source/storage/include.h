#pragma once

#include <windows.h> 
#include <tlhelp32.h> 
#include <string> 
#include <iostream> 
#include <fstream>
#include <vector>
#include <thread>
#include <algorithm>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

template<typename T>
inline const T& min(const T& a, const T& b) {
    return std::min(a, b);
}

template<typename T>
inline const T& max(const T& a, const T& b) {
    return std::max(a, b);
}

