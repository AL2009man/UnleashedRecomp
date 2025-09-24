#pragma once

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
}