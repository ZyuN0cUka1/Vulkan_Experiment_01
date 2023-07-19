cmake_minimum_required (VERSION 3.16)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(build_file_dir "${CMAKE_CURRENT_BINARY_DIR}/build")
# First for the generic no-config case (e.g. with mingw)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${build_file_dir} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${build_file_dir} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${build_file_dir} )
set( RUNTIME_OUTPUT_DIRECTORY ${build_file_dir} )
set( LIBRARY_OUTPUT_DIRECTORY ${build_file_dir} )
set( ARCHIVE_OUTPUT_DIRECTORY ${build_file_dir} )
# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${build_file_dir} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${build_file_dir} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${build_file_dir} )
	set( RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${build_file_dir} )
    set( LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${build_file_dir} )
    set( ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${build_file_dir} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

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

#==============================================================================
# COMPILE SHADERS
#
find_program(GLSLC glslc)
set(shader_path ${CMAKE_HOME_DIRECTORY}/assets/shaders/)
file(GLOB shaders RELATIVE ${CMAKE_SOURCE_DIR} "${shader_path}*.vert" "${shader_path}*.frag")
foreach(shader ${shaders})
	set(input_glsl "${CMAKE_HOME_DIRECTORY}/${shader}")
	set(output_spv "${input_glsl}.spv")
	execute_process(COMMAND "glslc" "${input_glsl}" "-o" "${output_spv}")
endforeach()
#
# COMPILE SHADERS END
#==============================================================================

project ("Vulkan_Experiment_01")

# find_package
find_package(Vulkan REQUIRED)
find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# include download files
file(DOWNLOAD
https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
${CMAKE_SOURCE_DIR}/external/stb_image.h)

# include source files
file(GLOB_RECURSE SRCFILES CONFIGURE_DEPENDS src/*.cpp src/*.h src/*.c)
file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS assets/shaders/*.spv)
file(GLOB_RECURSE TEXTURE_FILES CONFIGURE_DEPENDS assets/shaders/)
file(GLOB_RECURSE EXT_FILES CONFIGURE_DEPENDS external/*.h)

add_executable (${PROJECT_NAME} ${SRCFILES} ${SHADER_FILES} ${TEXTURE_FILES} ${EXT_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${build_file_dir}/assets)

# Check if code is debug
set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")
# Indication to the code that this is a debug build
if (${IS_DEBUG_BUILD})
    add_compile_definitions(__DEBUG__)
endif ()

# link libraries
target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		unofficial::vulkan-memory-allocator::vulkan-memory-allocator
		Vulkan::Vulkan
		glm::glm
		glfw
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# set install rules (windows only)
# if(WIN32)
#     if(MSVC)
#         install(TARGETS ${PROJECT_NAME}
#             DESTINATION bin COMPONENT binaries)
#         # install the target file since the ${PROJECT_NAME} target isn't built in this context.
#         install(FILES $<TARGET_FILE:${PROJECT_NAME}>
#             DESTINATION bin COMPONENT binaries)
#         install(FILES data/data.txt
#             DESTINATION data COMPONENT data_files)
#     endif(MSVC)
# endif(WIN32)
# 
# include(CPack)
