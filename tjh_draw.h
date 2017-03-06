#ifndef TJH_DRAW_H
#define TJH_DRAW_H

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
// Programable pipline OpenGL. You can load all your OpenGL functions with a
// library like glew.h. See here for more http://glew.sourceforge.net/

////// TODO ////////////////////////////////////////////////////////////////////
//
// - void setResolution( int width, int height );
// - optional float type

////// README //////////////////////////////////////////////////////////////////
//
// INSTALLATION:
//
//  1) #define TJH_DRAW_IMPLEMENTATION then #include this file in *ONE*
//  .cpp file in your project.
//
//  2) Call `Draw::init()` somewhere after you have setup your OpenGL context.
//  Beware this library will not setup an OpenGL context for you! You should
//  create one yourself, or use my handy TJH_WINDOW library if you like :)
//  
//  3) Call `Draw::shutdown()` somewhere before you have destroyed your OpenGL
//  context.
//
//  4) #include this file in all the files that want to draw stuff
//
//  5) Call `Draw::begin()` _draw the things_ then call `Draw::end()`
//

////// LIBRARY OPTIONS /////////////////////////////////////////////////////////
//
// You can modify the following #defines to enable or disable additional library
// features.
//
// Change this to customise the namespace for this library
#define TJH_DRAW_NAMESPACE Draw
// Change this to use a custom printf like function for your platform, for example SDL_Log
#define TJH_DRAW_PRINTF printf

////// HEADER //////////////////////////////////////////////////////////////////

namespace TJH_DRAW_NAMESPACE
{
    // LIBRARY FUNCTIONS ///////////////////////////////////////////////////////
    
    void init();        // TODO: return and error flag or something?
    void shutdown();
    void begin();
    void end();

    // STATE ///////////////////////////////////////////////////////////////////

    // TODO: void setResolution( int width, int height );
    //      This would scale all subsequent calls to use the given virtual
    //      resolution. This would mean 'points' would have to be actual
    //      quads. Allowing the user to do something like setResolution(32,32)
    //      Then have a cool pixel art looking type thing
    void clear( GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f );
    void setColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f );

    // PRIMATIVES //////////////////////////////////////////////////////////////

    void point( GLfloat x, GLfloat y );
    void line( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
    void quad( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
    // TODO: tri( x, y, x, y, x, y )
    void circle( GLfloat x, GLfloat y, GLfloat radius, int segments = 16 );

    extern const float PI;
}

#endif

////// IMPLEMENTATION //////////////////////////////////////////////////////////
#ifdef TJH_DRAW_IMPLEMENTATION
    
namespace TJH_DRAW_NAMESPACE
{
    // PUBLIC MEMBERS
    const float PI = 3.14159265359;

    // 'PRIVATE' MEMBERS
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint shader_program_ = 0;
    GLint colour_uniform_ = -1;

    // LIBRARY FUNCTIONS ///////////////////////////////////////////////////////

    void init()
    {
        const char* vert_src =
            "#version 150 core\n"
            "in vec2 vPos;"
            "void main()"
            "{"
            "    gl_Position = vec4(vPos, 0.0, 1.0);"
            "}";
        const char* frag_src =
            "#version 150 core\n"
            "uniform vec4 colour;"
            "out vec4 outColour;"
            "void main()"
            "{"
            "    outColour = colour;"
            "}";
        GLuint vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
        if( vertex_shader_ == 0 || fragment_shader_ == 0 ) TJH_DRAW_PRINTF("ERROR: cold not init shaders!");

        // Compile and check the vertex shader
        glShaderSource( vertex_shader_, 1, &vert_src, NULL );
        glCompileShader( vertex_shader_ );
        GLint status = GL_TRUE;
        glGetShaderiv( vertex_shader_, GL_COMPILE_STATUS, &status );
        if( status != GL_TRUE )
        {
            TJH_DRAW_PRINTF("ERROR: draw could not compile vertex shader\n");
            // Get the length of the error log
            GLint log_length = 0;
            glGetShaderiv(vertex_shader_, GL_INFO_LOG_LENGTH, &log_length);

            // Now get the error log itself
            GLchar buffer[log_length];
            glGetShaderInfoLog( vertex_shader_, log_length, NULL, buffer );
            TJH_DRAW_PRINTF("%s\n", buffer);
        }

        // Compile and check the fragment shader
        glShaderSource( fragment_shader_, 1, &frag_src, NULL );
        glCompileShader( fragment_shader_ );
        status = GL_TRUE;
        glGetShaderiv( fragment_shader_, GL_COMPILE_STATUS, &status );
        if( status != GL_TRUE )
        {
            TJH_DRAW_PRINTF("ERROR: draw could not compile fragment shader\n");
            // Get the length of the error log
            GLint log_length = 0;
            glGetShaderiv(fragment_shader_, GL_INFO_LOG_LENGTH, &log_length);

            // Now get the error log itself
            GLchar buffer[log_length];
            glGetShaderInfoLog( fragment_shader_, log_length, NULL, buffer );
            TJH_DRAW_PRINTF("%s\n", buffer);
        }

        shader_program_ = glCreateProgram();
        glAttachShader( shader_program_, vertex_shader_ );
        glAttachShader( shader_program_, fragment_shader_ );
        glBindFragDataLocation( shader_program_, 0, "outColour");
        glLinkProgram( shader_program_ );
        glUseProgram( shader_program_ );
        glDeleteShader(vertex_shader_);
        glDeleteShader(fragment_shader_);

        colour_uniform_ = glGetUniformLocation(shader_program_, "colour");

        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        GLint posAtrib = glGetAttribLocation(shader_program_, "vPos");
        if( posAtrib == -1 ) TJH_DRAW_PRINTF("ERROR: position attribute not found in shader\n");
        glEnableVertexAttribArray(posAtrib);
        glVertexAttribPointer(posAtrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }

    void shutdown()
    {
        glDeleteVertexArrays( 1, &vao_ );
        vao_ = 0;
        glDeleteProgram(shader_program_);
    }

    void begin()
    {
        // store previous values and set stuff, see dear imgui
        glUseProgram(shader_program_);
    }

    void end()
    {
        // TOOD: This should restore the previous state instead
        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // STATE ///////////////////////////////////////////////////////////////////

    void clear( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
    {
        glClearColor( r, g, b, a );
        glClear( GL_COLOR_BUFFER_BIT );
    }

    void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a )
    {
        glUniform4f( colour_uniform_, r, g, b, a );
    }

    // PRIMATIVES //////////////////////////////////////////////////////////////

    void point( GLfloat x, GLfloat y )
    {
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        GLfloat verts[] = { x, y };

        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
        glDrawArrays(GL_POINTS, 0, 1);
    }
    void line( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
    {
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        GLfloat verts[] = {
            x1, y1, x2, y2
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
        glDrawArrays(GL_LINES, 0, 2);
    }
    void quad( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
    {
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        GLfloat verts[] = {
            x1, y1, x2, y1, x2, y2,
            x1, y1, x2, y2, x1, y2
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    void circle( GLfloat x, GLfloat y, GLfloat radius, int segments )
    {
        GLfloat* verts = new GLfloat[segments * 2];
        float fraction = (PI*2) / (float)segments;

        for( int i = 0; i < segments; i++ )
        {
            verts[i*2]   = x + sin(fraction*i) * radius;
            verts[i*2+1] = y + cos(fraction*i) * radius;
        }

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * segments, verts, GL_STREAM_DRAW);
        glDrawArrays(GL_LINE_LOOP, 0, segments);

        delete[] verts;
    }
}

#endif