#pragma once
#include <vector>
#include <filesystem>
#include <SDL.h>

namespace UnleashedRecomp::SDL
{
    struct SDLLibraryInfo
    {
        SDL_version compiledVersion;
        SDL_version linkedVersion;
        const char* videoDriver;
        bool isDynamicOverride;
    };

    bool InitializeDynamicAPI();
    bool IsDynamicAPIActive();
    SDLLibraryInfo GetLibraryInfo();
    bool SetDynamicLibraryPath(const char* libPath);
    bool TestDynamicLibrary(const char* libPath);
    
    bool IsVersionDifferent(const SDL_version& compiled, const SDL_version& linked);
    std::vector<std::filesystem::path> GetLibrarySearchPaths();
}
