cmake_minimum_required(VERSION 3.20)

# Ensure VCPKG_ROOT is defined.
if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT is not defined!")
endif()

# Include VCPKG build system and essential settings.
include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
set(UNLEASHED_RECOMP_THIRDPARTY_ROOT ${CMAKE_SOURCE_DIR}/thirdparty)
set(UNLEASHED_RECOMP_TOOLS_ROOT ${CMAKE_SOURCE_DIR}/tools)
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

# Optional SDL2 support
option(USE_SDL2_DLL "Enable the use of an external SDL2.dll if available" ON)

if(USE_SDL2_DLL)
    # Attempt to find SDL2 dynamically.
    find_package(SDL2 QUIET)
    if(SDL2_FOUND)
        message(STATUS "External SDL2.dll found! Adding support.")
        add_definitions(-DUSE_SDL2_DLL)
        target_include_directories(UnleashedRecomp PRIVATE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(UnleashedRecomp PRIVATE ${SDL2_LIBRARIES})
    else()
        message(WARNING "SDL2.dll not found during build! The executable will attempt to load it dynamically at runtime.")
    endif()
else()
    # Fallback to integrated or static SDL2 library.
    message(STATUS "Using integrated SDL2 library. External SDL2.dll will not be supported.")
    # Uncomment this block if you have integrated SDL2 as a fallback:
    # add_subdirectory(thirdparty/SDL)
    # include_directories(thirdparty/SDL/include)
    # target_link_libraries(UnleashedRecomp PRIVATE SDL2-static)
endif()

# Prevent conflicts between dynamic and static SDL2 usage.
if(USE_SDL2_DLL AND SDL2_FOUND)
    if(TARGET SDL2::SDL2-static)
        message(FATAL_ERROR "Static SDL2 is being linked while dynamic SDL2.dll is enabled. Please ensure SDL2.dll is used at runtime.")
    endif()
endif()

# Enable MSVC Hot Reload support if the compiler and policy support it.
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Use multithreaded runtime for MSVC.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Target Sandy Bridge for all projects.
add_compile_options(
    -march=sandybridge
)

# Add third-party and tools directories.
add_subdirectory(${UNLEASHED_RECOMP_THIRDPARTY_ROOT})
add_subdirectory(${UNLEASHED_RECOMP_TOOLS_ROOT})

# Define the main project
project("UnleashedRecomp-ALL")

# Include sub-projects.
add_subdirectory("UnleashedRecompLib")
add_subdirectory("UnleashedRecomp")
