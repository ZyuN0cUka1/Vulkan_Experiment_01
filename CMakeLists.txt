cmake_minimum_required (VERSION 3.16)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Include vcpkg-2023.06.20
include(FetchContent)
FetchContent_Populate(
	vcpkg
	GIT_REPOSITORY	https://github.com/microsoft/vcpkg.git
	GIT_TAG			2023.06.20
	SOURCE_DIR		"${CMAKE_SOURCE_DIR}/vcpkg"
	QUIET
)
list(APPEND CMAKE_PROJECT_TOP_LEVEL_INCLUDES		"${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake")
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES	CMAKE_PROJECT_TOP_LEVEL_INCLUDES)

# Other CMake library not included in vcpkg
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

project ("Vulkan_Experiment_01")

# find_package
find_package(unofficial-vulkan-memory-allocator REQUIRED)

# include source files
file(GLOB SRCFILES src/*.cpp src/*.h src/*.c)
add_executable (Vulkan_Experiment_01 ${SRCFILES})
target_link_libraries(
	Vulkan_Experiment_01
	PUBLIC
		unofficial::vulkan-memory-allocator::vulkan-memory-allocator
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Vulkan_Experiment_01 PROPERTY CXX_STANDARD 20)
endif()

# set install rules (windows only)
# if(WIN32)
#     if(MSVC)
#         install(TARGETS Vulkan_Experiment_01
#             DESTINATION bin COMPONENT binaries)
#         # install the target file since the Vulkan_Experiment_01 target isn't built in this context.
#         install(FILES $<TARGET_FILE:Vulkan_Experiment_01>
#             DESTINATION bin COMPONENT binaries)
#         install(FILES data/data.txt
#             DESTINATION data COMPONENT data_files)
#     endif(MSVC)
# endif(WIN32)
# 
# include(CPack)
