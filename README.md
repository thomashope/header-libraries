# Tom's Single Header Libraries

[![Build Status](https://travis-ci.org/Cyphre117/header-libraries.svg?branch=master)](https://travis-ci.org/Cyphre117/header-libraries)

Here are some single header libraries I made to try and make my life easier.
Maybe you will find them usefull too!

Inspried by Sean Barrett http://nothings.org/

## WHY ARE YOU DOING THIS?

To spend less time writing the same old code
and more time getting stuff done.

The design of this library is heavily influenced by Sean Barrett, creater of
the `stb_*.h` libraries. You can take a look and find out more about them
at https://github.com/nothings/stb . Having said that I am not trying to
mimic the `stb_*.h` libraries exactly. I am instead trying to prevent myself
from having to rewrite the same code over and over, and so this little
collection of libraries I have modestly named the `tjh_*.h` libraries, is very
much tailored to the kind of code I write.

However if you happen to write code like me, then I hope this can be of use.
If you have any suggestions of how I can improve this library get in touch
through Twitter or Github, i'd be interested to hear your thoughts!

Much love, Tom
(Thomas J. Hope)

## TODO

- Improve test coverage!! Currently non-existant!
- write a breif description of each library
- write tjh_camera.h example
- tjh_audio.h
    - OpenAL integration
    - Classes for buffer, source, global listener
    - cache audio sources
    - Decodes audio files (requires SDL_LoadWAV?, integrate stb_vorbis?)
    - alternatively use that other guy's flac and wav single header libs
- tjh_colour.h?
    - conversion between HLSL and RGB with values [0,1], or [0,255]
    - could use glm vector types for rather than (float r, float g, float b)?
    - what other colour functions are usefull? here are some ideas https://github.com/WetDesertRock/vivid
    - perhaps incorporate this into tjh_draw.h
- tjh_screen_logger.h?
    - print debug log output graphicaly
    - can also draw static strings
    - allows messages to be updated after being pushed
    - what do other loggers do... message levels, colours
