#include "stdafx.h"
#include "sdl_dynamic_api.h"
#include <os/logger.h>
#include <vector>
#include <algorithm>

namespace UnleashedRecomp::SDL
{
    // SDL2-compat is typically much smaller than the actual SDL2 file (less than ~500KB vs 2MB+)
    constexpr size_t SDL2_COMPAT_MAX_SIZE = 500000;
    
    // Platform-specific SDL2 library names for auto-detection
#if defined(_WIN32)
    constexpr const char* SDL2_LIBRARY_NAME = "SDL2.dll";
#elif defined(__APPLE__)
    constexpr const char* SDL2_LIBRARY_NAME = "libSDL2.dylib";
#else
    constexpr const char* SDL2_LIBRARY_NAME = "libSDL2.so";
#endif

    // Get library search paths for auto-detection
    std::vector<std::filesystem::path> GetLibrarySearchPaths()
    {
        std::vector<std::filesystem::path> searchPaths;
        
        // Always check executable directory first
        searchPaths.push_back(std::filesystem::current_path());
        
#if defined(__linux__) || defined(__APPLE__)
        // Unix-like systems: check only the most common paths
        if (std::filesystem::exists("/usr/lib"))
            searchPaths.push_back("/usr/lib");
        if (std::filesystem::exists("/usr/local/lib"))
            searchPaths.push_back("/usr/local/lib");
#endif
        
        return searchPaths;
    }

    bool InitializeDynamicAPI()
    {
        // Auto-detect SDL2 library if no manual override is set
        // NOTE: This must be done BEFORE any SDL functions are called
        if (!IsDynamicAPIActive())
        {
            std::vector<std::filesystem::path> searchPaths = GetLibrarySearchPaths();
            
            // Search for SDL2 library in common paths
            for (const auto& searchPath : searchPaths)
            {
                std::filesystem::path sdl2Path = searchPath / SDL2_LIBRARY_NAME;
                
                if (TestDynamicLibrary(sdl2Path.string().c_str()))
                {
                    SetDynamicLibraryPath(sdl2Path.string().c_str());
                    break; // Found and set, stop searching
                }
            }
        }
        
        return true;
    }
    
    bool IsDynamicAPIActive()
    {
        const char* dynamicAPI = std::getenv("SDL_DYNAMIC_API");
        return (dynamicAPI != nullptr && dynamicAPI[0] != '\0');
    }
    
    bool IsVersionDifferent(const SDL_version& compiled, const SDL_version& linked)
    {
        return (compiled.major != linked.major || 
                compiled.minor != linked.minor || 
                compiled.patch != linked.patch);
    }
    
    SDLLibraryInfo GetLibraryInfo()
    {
        SDLLibraryInfo info = {};
        
        SDL_VERSION(&info.compiledVersion);
        SDL_GetVersion(&info.linkedVersion);
        
        // Get video driver (may be null if video not initialized)
        const char* driver = SDL_GetCurrentVideoDriver();
        info.videoDriver = driver ? driver : "Not initialized";
        
        // Check if dynamic API is potentially active
        info.isDynamicOverride = IsDynamicAPIActive();
        
        // Additional logging about SDL version after initialization
        if (info.isDynamicOverride)
        {
            // If versions differ, dynamic API is likely working
            if (IsVersionDifferent(info.compiledVersion, info.linkedVersion))
            {
                LOGFN("SDL dynamic api: SDL {}.{}.{}", 
                      info.linkedVersion.major, info.linkedVersion.minor, info.linkedVersion.patch);
            }
            else
            {
                LOGFN("SDL dynamic api: SDL {}.{}.{} (fallback)", 
                      info.linkedVersion.major, info.linkedVersion.minor, info.linkedVersion.patch);
            }
        }
        
        return info;
    }
    
    bool SetDynamicLibraryPath(const char* libPath)
    {
        if (!libPath || libPath[0] == '\0')
            return false;
        
#if defined(_WIN32)
        return _putenv_s("SDL_DYNAMIC_API", libPath) == 0;
#else
        return setenv("SDL_DYNAMIC_API", libPath, 1) == 0;
#endif
    }
    
    bool HasValidLibraryExtension(const std::filesystem::path& path)
    {
        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
#if defined(_WIN32)
        return extension == ".dll";
#elif defined(__APPLE__)
        return extension == ".dylib";
#else
        return extension == ".so" || extension.find(".so.") != std::string::npos;
#endif
    }
    
    bool TestDynamicLibrary(const char* libPath)
    {
        if (!libPath || libPath[0] == '\0')
            return false;
        
        std::filesystem::path path(libPath);
        if (!std::filesystem::exists(path) || !HasValidLibraryExtension(path))
            return false;
        
        // Check file size to detect SDL2-compat
        std::error_code ec;
        auto fileSize = std::filesystem::file_size(path, ec);
        
        if (!ec && fileSize < SDL2_COMPAT_MAX_SIZE)
        {
            LOGFN("SDL dynamic api: SDL2-compat detected, attempting compatibility mode");
        }
        
        return true;
    }
}
