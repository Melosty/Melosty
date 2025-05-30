// Melosty.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include <thread>
#include <chrono>

#include "glad/glad.h"
#include "glfw/include/GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <algorithm> // for std::min and std::max
#include <Windows.h>
#include <Mmsystem.h>
#endif

// --- MODULE ---

// TODO: 在此处引用程序需要的其他标头。
