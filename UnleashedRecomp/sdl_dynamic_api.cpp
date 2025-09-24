#include "stdafx.h"
#include "sdl_dynamic_api.h"

namespace UnleashedRecomp::SDL
{
    bool InitializeDynamicAPI()
    {
        // SDL_DYNAMIC_API is automatically initialized on first SDL function call
        // This function can be used to trigger early initialization if needed
        
        // Get SDL version to trigger dynamic API initialization
        SDL_version compiled;
        SDL_version linked;
        
        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        
        return true;
    }
    
    bool IsDynamicAPIActive()
    {
        // There's no direct way to check if dynamic API is active,
        // but we can infer it by checking if an override was specified
        const char* dynamicAPI = std::getenv("SDL_DYNAMIC_API");
        return (dynamicAPI != nullptr && dynamicAPI[0] != '\0');
    }
    
    SDLLibraryInfo GetLibraryInfo()
    {
        SDLLibraryInfo info = {};
        
        // Get compiled version (version this was built against)
        SDL_VERSION(&info.compiledVersion);
        
        // Get linked version (version currently loaded)
        SDL_GetVersion(&info.linkedVersion);
        
        // Get current video driver
        info.videoDriver = SDL_GetCurrentVideoDriver();
        
        // Check if dynamic override is active
        info.isDynamicOverride = IsDynamicAPIActive();
        
        return info;
    }
    
    bool SetDynamicLibraryPath(const char* libPath)
    {
        if (!libPath || libPath[0] == '\0')
        {
            return false;
        }
        
        // Set environment variable for SDL dynamic API
        // Note: This must be done before any SDL functions are called
#if defined(_WIN32)
        return _putenv_s("SDL_DYNAMIC_API", libPath) == 0;
#else
        return setenv("SDL_DYNAMIC_API", libPath, 1) == 0;
#endif
    }
}