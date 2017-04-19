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

////// DEPENDENCIES ////////////////////////////////////////////////////////////
//
// This single header library relies on the following:
//
// SDL v2.0.5 which is Copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>
// and distributed under the zlib license. See here for more
// https://www.libsdl.org/
//
// GLEW v2.0.0 which is Copyright (C) Nate Robins - 1997, Michael Wimmer - 1999
// Milan Ikits - 2002-2008, Nigel Stewart - 2008-2013 and distributed under the
// Modified BSD License, Mesa 3-D Licsense (MIT) and the Khronos License (MIT)
// license. See here for more http://glew.sourceforge.net/

////// TODO ////////////////////////////////////////////////////////////////////
//
// - choose requested opengl version
// - set fullscreen state

////// DOCUMENTATION ///////////////////////////////////////////////////////////
//
// ABOUT
//
// This library tries to make getting an OpenGL context up as painless as
// possible, allowing you to get going with as little as three function calls.
//
// - Window::init("Title", width, height) // Creates the window
// - Window::present()      // Call this every frame
// - Window::shutdown()     // Call this when you are done
//
// The library will optionally initialize SDL and GLEW for you for maximum
// convinience!
//
// USAGE
//
// Simply drop this file in your project directory and in any ONE .cpp file do
// the following:
//
// #define TJH_WINDOW_IMPLEMENTATION
// #include "window.h"
//
// You can then #include "window.h" as normal in every other file that wishes to
// use any of the window functions.
//
// You will need to link your application with SDL2 GLEW and OpenGL. On macOS
// the OpenGL framework is included by default, and you can easily install SDL2
// and GLEW using homebrew https://brew.sh . To compile run the following in
// terminal:
//
// clang++ main.cpp -lSDL2 -lglew -framework opengl

////// LIBRARY OPTIONS /////////////////////////////////////////////////////////
//
// You can modify the following defines to customize the library for your needs
// (also feel free to reach in and modify the code if you so desire).

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
    bool init(const char* title, int width, int height);    // Call one, can init SDL and GLEW if macros are set
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

    bool init(const char* title, int width, int height)
    {
        bool success = true;

    #ifdef TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL
        if( SDL_Init(SDL_INIT_EVERYTHING) )
        {
            TJH_WINDOW_PRINTF("ERROR: could not init SDL2 %s\n", SDL_GetError());
            success = false;
        }
    #endif

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        // Assume we want basic multisampling
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        sdl_window = SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL );
        if( sdl_window == NULL )
        {
            TJH_WINDOW_PRINTF("ERROR: creating window %s\n", SDL_GetError());
            success = false;
        }

        sdl_gl_context = SDL_GL_CreateContext( sdl_window );
        if( sdl_gl_context == NULL )
        {
            TJH_WINDOW_PRINTF("ERROR: creating opengl context %s\n", SDL_GetError());
            success = false;
        }

    #ifdef TJH_WINDOW_AUTO_INIT_GLEW
        glewExperimental = GL_TRUE;
        GLenum error = glewInit();
        if( error != GLEW_OK ) TJH_WINDOW_PRINTF("ERROR: starting glew %d", error);

        // Turn on multisampling in opengl
        glEnable(GL_MULTISAMPLE);
    #endif

        // try to set Vsync by default, don't really care if it doesn't work though...
        setVsync( true );

        return success;
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

// Prevent the implementation from leaking into subsequent includes
#undef TJH_WINDOW_IMPLEMENTATION
#endif // END TJH_WINDOW_IMPLEMENTATION
