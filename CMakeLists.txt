# CMakeList.txt: Melosty 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Melosty")

# 将源代码添加到此项目的可执行文件。
include_directories(
    include
    include/glfw/include
    include/tweeny/include
    module
)

add_subdirectory (include/glfw)

file(GLOB IMGUI_SRCS
    include/imgui/*.cpp
)

file(GLOB_RECURSE MODULE_SRCS
    module/*.cpp
)

add_executable (Melosty
    Melosty.cpp

    glad.c

    ${IMGUI_SRCS}

    ${MODULE_SRCS}
)

find_package(OpenGL REQUIRED)

target_link_libraries(Melosty
    glfw
    OpenGL::GL
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(Melosty winmm)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Melosty PROPERTY CXX_STANDARD 17)
endif()

# TODO: 如有需要，请添加测试并安装目标。
