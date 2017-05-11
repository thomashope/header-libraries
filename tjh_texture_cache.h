#ifndef TJH_TEXTURE_CACHE_H_HEADER
#define TJH_TEXTURE_CACHE_H_HEADER

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
// LIBNAME vX.X.X which is Copyright (C) HOLDER_NAME and distributed under the
// LICENSE_NAME license. See here for more LINK_TO_RELEVANT_PAGE

////// DOCUMENTATION ///////////////////////////////////////////////////////////
//
// ABOUT
//
// 
//
// USAGE
//
// Simply drop this file in your project directory and in any ONE .cpp file do
// the following:
//
// #define TJH_xxx_IMPLEMENTATION
// #include "tjh_texture_cache.h"
//
// You can then #include "tjh_texture_cache.h" as normal in every other file that wishes to
// use any of the window functions.

////// LIBRARY OPTIONS /////////////////////////////////////////////////////////
//
// You can modify the following defines to customize the library for your needs
// (also feel free to reach in and modify the code if you so desire).

// Change this to use a custom printf like function, for example SDL_Log
#define TJH_TEXTURE_CACHE_PRINTF printf
// Change this to use a different namespace if this one conflicts
#define TJH_TEXTURE_CACHE_NAMESPACE Texture
// Change this to use a different typename for the texture handles
#define TJH_TEXTURE_CACHE_HANDLE_TYPENAME Handle
// The place where glew.h is located
#define TJH_TEXTURE_CACHE_GLEW_H_LOCATION <GL/glew.h>

////// TODO ////////////////////////////////////////////////////////////////////
//
// - list dependancies, their licenses, and their respective copyright holders
// - write ABOUT documentation
// - add examples to USAGE documentation
// - make it so defines can be changed from outside the file
// - single textures are reloadable
// - all textures are reloadable
// - textures can be bound to different slots
// - report errors
// - called can request the number of channels

////// HEADER //////////////////////////////////////////////////////////////////

#include TJH_TEXTURE_CACHE_GLEW_H_LOCATION

namespace TJH_TEXTURE_CACHE_NAMESPACE
{
    struct TJH_TEXTURE_CACHE_HANDLE_TYPENAME
    {
        int width           = -1;
        int height          = -1;
        GLint s_wrap        = GL_CLAMP_TO_EDGE;
        GLint t_wrap        = GL_CLAMP_TO_EDGE;
        GLint min_filter    = GL_LINEAR;
        GLint max_filter    = GL_LINEAR;
        GLuint texture      = 0;
        char channels       = 0;

        void bind();
    };
    
    TJH_TEXTURE_CACHE_HANDLE_TYPENAME
    load(std::string filename, char channels = 4);

    void clear();
}

#endif // END HEADER

////// IMPLEMENTATION //////////////////////////////////////////////////////////

#ifdef TJH_TEXTURE_CACHE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <map>

namespace TJH_TEXTURE_CACHE_NAMESPACE
{
    // 'PRIVATE' MEMBER VARIABLES
    std::map<std::string, TJH_TEXTURE_CACHE_HANDLE_TYPENAME> texture_cache_;

    // 'PRIVATE' MEMBER FUNCTIONS

    TJH_TEXTURE_CACHE_HANDLE_TYPENAME
    load(std::string filename, char channels)
    {
        auto it = texture_cache_.find(filename);
        if( it != texture_cache_.end() )
        {
            // Return a copy of the version in the cache
            TJH_TEXTURE_CACHE_PRINTF( "Retrieved %s from the cache.\n", filename.c_str() );
            return it->second;
        }
        else
        {
            // Load the texture in anew
            TJH_TEXTURE_CACHE_HANDLE_TYPENAME result;
            result.channels = channels;

            unsigned char* data = nullptr;
            int n; // This is the number of channels that the image originally had, currently we don't care

            // TODO: error checking if the texture failed to load
            data = stbi_load( filename.c_str(), &result.width, &result.height, &n, result.channels );
            if( !data )
            {
                TJH_TEXTURE_CACHE_PRINTF( "ERROR: failed to load image %s\n", filename.c_str() );
                return result;
            }

            // TODO: error checking if the texture failed to gen
            glGenTextures( 1, &result.texture );
            glBindTexture( GL_TEXTURE_2D, result.texture );

            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                result.width, result.height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, data );

            stbi_image_free(data);

            texture_cache_[filename] = result;
            TJH_TEXTURE_CACHE_PRINTF( "Added %s to the texture cache.\n", filename.c_str() );

            return result;
        }
    }

    void clear()
    {
        // Iterate over the entire map and delete all the textures
        for( const auto& t : texture_cache_ )
        {
            glDeleteTextures( 1, &t.second.texture );
        }

        texture_cache_.clear();
    }

    void TJH_TEXTURE_CACHE_HANDLE_TYPENAME::bind()
    {
        glBindTexture( GL_TEXTURE_2D, texture );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
}

#undef STB_IMAGE_IMPLEMENTATION
#undef TJH_TEXTURE_CACHE_H_IMPLEMENTATION
#endif // END IMPLEMENTATION
