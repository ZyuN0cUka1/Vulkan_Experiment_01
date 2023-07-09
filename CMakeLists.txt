# CMakeList.txt: Vulkan_Experiment_01 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.17)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

include(FetchContent)

FetchContent_Populate(
  vcpkg
  GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
  GIT_TAG        703a8113f42e794534df5dfc5cece5dabcb949d0
  SOURCE_DIR     "${CMAKE_SOURCE_DIR}/vcpkg"
  QUIET
)

list(APPEND CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake")
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES CMAKE_PROJECT_TOP_LEVEL_INCLUDES)

project ("Vulkan_Experiment_01")

# 将源代码添加到此项目的可执行文件。
add_executable (Vulkan_Experiment_01 "src/Vulkan_Experiment_01.cpp" "src/Vulkan_Experiment_01.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Vulkan_Experiment_01 PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
 