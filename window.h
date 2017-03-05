#ifndef TJH_WINDOW_H
#define TJH_WINDOW_H

////// UNLICENSE ///////////////////////////////////////////////////////////////
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>

////// TODO ////////////////////////////////////////////////////////////////////
//

////// DOCUMENTATION ///////////////////////////////////////////////////////////
//
// * Define TJH_WINDOW_IMPLEMENTATION in *ONE CPP FILE ONLY*
// * This library relies on GLEW and SDL2
//
// To compile on mac, install SDL2 and glew using homebrew
// then link during compilation with -lSDL2 -lglew -framework OpenGL
//
// TODO: write a short tutorial

////// LIBRARY OPTIONS /////////////////////////////////////////////////////////
//
// You can comment or uncomment the following #defines to enable or disable
// additional library features.

// If defined init() and shutdown() will setup and quit the SDL2 library for you
#define TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL
// If defined init() will call glewInit() with glewExperimental set to true
#define TJH_WINDOW_AUTO_INIT_GLEW
// If defined this file will include SDL.h
#define TJH_WINDOW_INCLUDE_SDL
// If defined this file will include glew.h
#define TJH_WINDOW_INCLUDE_GLEW

// IMPORTANT: If any of the above options are set these must also be set correctly
#define TJH_WINDOW_SDL_H_LOCATION <SDL2/SDL.h>
#define TJH_WINDOW_GLEW_H_LOCATION <GL/glew.h>

// Change this to customise the namespace for this library
#define TJH_WINDOW_NAMESPACE Window
// Change this to use a custom printf like function for your platform, for example SDL_Log
#define TJH_WINDOW_PRINTF printf

////// HEADER //////////////////////////////////////////////////////////////////

#ifdef TJH_WINDOW_INCLUDE_SDL
    #include TJH_WINDOW_SDL_H_LOCATION
#endif
#ifdef TJH_WINDOW_INCLUDE_GLEW
    #include TJH_WINDOW_GLEW_H_LOCATION
#endif

namespace TJH_WINDOW_NAMESPACE
{
    // TODO: init() return an error flag or something?
    void init(const char* title, int width, int height);    // Call one, can init SDL and GLEW if macros are set
    void shutdown();                                        // Shuts down SDL window, and library if asked
    void present();                                         // Flip the buffers
    
    // Getters
    void getSize( int* width, int* height );                // Get the current size of the window

    // Returns true on success
    bool setVsync( bool enable );

    // PUBLIC MEMBERS
    extern SDL_Window* sdl_window;
    extern SDL_GLContext sdl_gl_context;
}

#endif // END TJH_WINDOW_H

////// IMPLEMENTATION //////////////////////////////////////////////////////////

#ifdef TJH_WINDOW_IMPLEMENTATION

namespace TJH_WINDOW_NAMESPACE
{
    // PUBLIC MEMBERS
    SDL_Window* sdl_window = NULL;
    SDL_GLContext sdl_gl_context = NULL;

    void init(const char* title, int width, int height)
    {
    #ifdef TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL
        SDL_Init(SDL_INIT_EVERYTHING);
        // TODO: error checking
    #endif

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        sdl_window = SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL );
        // TODO: error checking
        sdl_gl_context = SDL_GL_CreateContext( sdl_window );
        // TODO: error checking

    #ifdef TJH_WINDOW_AUTO_INIT_GLEW
        glewExperimental = GL_TRUE;
        GLenum error = glewInit();
        if( error != GLEW_OK ) TJH_WINDOW_PRINTF("ERROR: starting glew %d", error);
    #endif
    }

    void shutdown()
    {
        SDL_GL_DeleteContext( sdl_gl_context );
        sdl_gl_context = NULL;

        SDL_DestroyWindow( sdl_window );
        sdl_window = NULL;

    #ifdef TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL
        SDL_Quit();
    #endif
    }

    void present()
    {
        SDL_GL_SwapWindow( sdl_window );
    }

    void getSize( int* width, int* height )
    {
        SDL_GetWindowSize( sdl_window, width, height );
    }

    bool setVsync( bool enable )
    {
        if( enable )
        {
            // try late swap tearing first
            bool success = (SDL_GL_SetSwapInterval( -1 ) == 0);
            if( success )
            {
                return true;
            }
            else
            {
                // If that fails try normal vsync
                return SDL_GL_SetSwapInterval( 1 ) == 0;
            }
        }
        else
        {
            // Pass zero to disable vysinc
            return SDL_GL_SetSwapInterval( 0 ) == 0;
        }
    }
}

#undef TJH_WINDOW_IMPLEMENTATION
#endif // END TJH_WINDOW_IMPLEMENTATION