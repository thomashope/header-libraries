#include <cstdio>
#define TJH_AUDIO_H_IMPLEMENTATION
#include "../tjh_audio.h"

int main()
{
    Audio::init();

    AudioBuffer sample_sound = Audio::load( "sample.wav" );
    AudioSource test_source;

    test_source.play( sample_sound );

    SDL_Delay( 1000 );
    Audio::shutdown();

    return 0;
}

