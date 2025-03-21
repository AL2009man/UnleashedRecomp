cmake_minimum_required (VERSION 3.20)

if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT is not defined!")
endif()

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
set(UNLEASHED_RECOMP_THIRDPARTY_ROOT ${CMAKE_SOURCE_DIR}/thirdparty)
set(UNLEASHED_RECOMP_TOOLS_ROOT ${CMAKE_SOURCE_DIR}/tools)
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

# Optional SDL2 support
option(USE_SDL2_DLL "Enable the use of an external SDL2.dll if available" ON)

if(USE_SDL2_DLL)
    # Attempt to find SDL2 dynamically
    find_package(SDL2 QUIET)
    if(SDL2_FOUND)
        message(STATUS "External SDL2.dll found! Adding support.")
        add_definitions(-DUSE_SDL2_DLL) # Define preprocessor macro for SDL2 DLL usage
        target_include_directories(UnleashedRecomp PRIVATE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(UnleashedRecomp PRIVATE ${SDL2_LIBRARIES})
    else()
        message(WARNING "SDL2.dll not found! Proceeding without optional external support.")
    endif()
else()
    # Fallback to integrated SDL2 (static or submodule)
    message(STATUS "Using integrated SDL2 library.")
    # Example integration code (commented out since specifics depend on your setup):
    # add_subdirectory(thirdparty/SDL)
    # include_directories(thirdparty/SDL/include)
    # target_link_libraries(UnleashedRecomp SDL2-static)
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Target Sandy Bridge for all projects
add_compile_options(
    -march=sandybridge
)

add_subdirectory(${UNLEASHED_RECOMP_THIRDPARTY_ROOT})
add_subdirectory(${UNLEASHED_RECOMP_TOOLS_ROOT})

project("UnleashedRecomp-ALL")

# Include sub-projects.
add_subdirectory("UnleashedRecompLib")
add_subdirectory("UnleashedRecomp")
