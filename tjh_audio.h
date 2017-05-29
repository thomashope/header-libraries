#ifndef TJH_AUDIO_H_HEADER
#define TJH_AUDIO_H_HEADER

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
// #define TJH_AUDIO_H_IMPLEMENTATION
// #include "tjh_audio.h"
//
// You can then #include "tjh_audio.h" as normal in every other file that wishes to
// use any of the window functions.

////// LIBRARY OPTIONS /////////////////////////////////////////////////////////
//
// You can modify the following defines to customize the library for your needs
// (also feel free to reach in and modify the code if you so desire).

// Change this to use a custom printf like function, for example SDL_Log
#define TJH_AUDIO_PRINTF printf
// Change this to use a different namespace if this one conflicts
#define TJH_AUDIO_NAMESPACE Audio

#define TJH_AUDIO_SOURCE_TYPENAME AudioSource

#define TJH_AUDIO_BUFFER_TYPENAME AudioBuffer

////// TODO ////////////////////////////////////////////////////////////////////
//
// - list dependancies, their licenses, and their respective copyright holders
// - write ABOUT documentation
// - add examples to USAGE documentation
// - play short sound effects
// - stream longer audio like music
// - replace SDL dependancy with the single header vorbis, wav, flac libs
// - cache audio buffers
// - reload buffers

////// HEADER //////////////////////////////////////////////////////////////////

#include <SDL2/SDL.h>
#include <OpenAL/OpenAL.h>
#include <string>

// Struct representing an audio buffer
class TJH_AUDIO_BUFFER_TYPENAME
{
public:
    TJH_AUDIO_BUFFER_TYPENAME( ALuint id ) : id_(id) {}

    inline ALuint bufferId() const { return id_; }
private:
    ALuint id_ = 0;
};

// Struct representing an audio source
class TJH_AUDIO_SOURCE_TYPENAME
{
public:
    TJH_AUDIO_SOURCE_TYPENAME()
    {
        alGenSources( 1, &id_ );
        // TODO: error check soure generated ok
    }
    
    inline void play( const TJH_AUDIO_BUFFER_TYPENAME& buffer )
    {
        alSourcei( id_, AL_BUFFER, buffer.bufferId() );
        alSourcePlay( id_ );
    }

    // default pitch is 1, doubling increases by an octave, halfing decreases by an octave, 0 is invalid
    inline void setPitch( float pitch ) const { alSourcef( id_, AL_PITCH, pitch ); }
    // gain should be in range 0 to 1
    inline void setGain( float gain ) const { alSourcef( id_, AL_GAIN, gain ); }
    inline void setLooping( bool looping ) const { alSourcei( id_, AL_LOOPING, looping ? AL_TRUE : AL_FALSE ); }
    inline void setPosition( float x, float y, float z ) const { alSource3f( id_, AL_POSITION, x, y, z ); }
    inline void setVelocity( float x, float y, float z ) const { alSource3f( id_, AL_VELOCITY, x, y, z ); }
    inline ALuint sourceId() const { return id_; }
private:
    ALuint id_ = 0;
};

namespace TJH_AUDIO_NAMESPACE
{
    bool init();
    void shutdown();

    TJH_AUDIO_BUFFER_TYPENAME load( std::string filepath );
}


#endif // END HEADER

////// IMPLEMENTATION //////////////////////////////////////////////////////////

#ifdef TJH_AUDIO_H_IMPLEMENTATION

namespace TJH_AUDIO_NAMESPACE
{
    // 'PRIVATE' MEMBER VARIABLES
    
    ALCdevice* device_ = nullptr;
    ALCcontext* context_ = nullptr;

    // 'PRIVATE' MEMBER FUNCTIONS
    
    bool load_wav( std::string filepath, ALuint buffer_id );
    std::string extract_filetype( std::string filepath );
    
    bool init()
    {
        bool success = true;

        // Create the audio device
        if( !device_ )
        {
            device_ = alcOpenDevice(nullptr);
            if( !device_ )
            {
                TJH_AUDIO_PRINTF("ERROR: failed to open audio device\n");
                success = false;
            }
        }

        // Create the audio context
        if( !context_ && success )
        {
            context_ = alcCreateContext( device_, nullptr );
            if( !context_ )
            {
                TJH_AUDIO_PRINTF("ERROR: failed to create audio context\n");
                success = false;
            }
        }

        if( success )
        {
            alcMakeContextCurrent( context_ );
        }

        return success;
    }

    void shutdown()
    {
        alcDestroyContext( context_ );
        context_ = nullptr;
        alcCloseDevice( device_ );
        device_ = nullptr;
    }
    
    TJH_AUDIO_BUFFER_TYPENAME load( std::string filepath )
    {
        ALuint buffer_id;
        alGenBuffers( 1, &buffer_id );
        // TODO: error check buffer generation

        bool loaded = false, known_extension = true;
        std::string extension = extract_filetype( filepath );
        if( extension == "wav" )
        {
            loaded = load_wav( filepath, buffer_id );
        }
        else known_extension = false;
        
        if( !loaded || !known_extension )
        {
            if( !known_extension ) TJH_AUDIO_PRINTF( "ERROR: unsupported audio extension%s\n", extension.c_str() );
            if( !loaded ) TJH_AUDIO_PRINTF( "ERROR: failed to load audio file '%s'\n", filepath.c_str() );
            
            if( buffer_id )
            {
                alDeleteBuffers( 1, &buffer_id );
                buffer_id = 0;
            }
        }

        return TJH_AUDIO_BUFFER_TYPENAME( buffer_id );
    }

    bool load_wav( std::string filepath, ALuint buffer_id )
    {
        bool success = true;
        
        SDL_AudioSpec wav_spec;
        Uint32 wav_length;
        Uint8 *wav_buffer;

        if( !SDL_LoadWAV( filepath.c_str(), &wav_spec, &wav_buffer, &wav_length ) )
        {
            TJH_AUDIO_PRINTF( "ERROR: could not load audio file" );
            success = false;
            return success;
        }

        ALenum format = 0;

        // Figure out what type of audio we have loaded
        // TODO: if the format is not natively supported by OpenAL (8 bit unsigned and 16 bit signed) the we should do a conversion
        // formats that should be supported include signed 8bit unsigned 16bit, signed and unsigned 32 bit

        switch( wav_spec.channels )
        {
        case 1: // Mono audio
            switch( wav_spec.format )
            {
                case AUDIO_U8:  format = AL_FORMAT_MONO8; break;
                case AUDIO_S16: format = AL_FORMAT_MONO16; break;
                default:
                    TJH_AUDIO_PRINTF("ERROR: Audio has unsupported format: %d", wav_spec.format );
                    success = false;
                break;
            }
        break;
        case 2: // Stereo (does not support 3d positioning)
            switch( wav_spec.format )
            {
                case AUDIO_U8:  format = AL_FORMAT_STEREO8; break;
                case AUDIO_S16: format = AL_FORMAT_STEREO16; break;
                default:
                    TJH_AUDIO_PRINTF("ERROR: Audio has unsupported format: %d", wav_spec.format );
                    success = false;
                break;
            }
        break;
        default:
            TJH_AUDIO_PRINTF("ERROR: unsupported number of audio channels: %d", wav_spec.channels );
            success = false;
        break;
        }

        alBufferData( buffer_id, format, wav_buffer, wav_length, wav_spec.freq );

        // Now we have stored the wav data in the buffer we can free it
        SDL_FreeWAV( wav_buffer );

        return success;
    }

    std::string extract_filetype( std::string filepath )
    {
        size_t dot = filepath.find_last_of('.');

        if( dot == std::string::npos )
        {
            // Did not find a dot
            return "";
        }

        // return everything after the dot
        return filepath.substr(dot+1);
    }
}

#undef TJH_AUDIO_H_IMPLEMENTATION
#endif // END IMPLEMENTATION
