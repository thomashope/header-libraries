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

////// README //////////////////////////////////////////////////////////////////
//
// USAGE:
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
//  5) Call `Draw::quad()` and such to  draw the things, then call `Draw::flush()`
//  as the last thing you do to ensure everything is written
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

////// TODO ////////////////////////////////////////////////////////////////////
//
//  - convert line() to use triangles, optional settable width
//  - test setScale x_offset and y_offset
//      - i implemented something along those lines but i don't know if it actually works
//  - solid shapes have a line draw mode
//      - when drawing shapes in line mode, lines expand inwards to preserve specified size
//  - 3d quad from point, normal, width, height
//  - 3d textured quad
//  - 3d circle from point, line, radius
//  - 3d sphere
//  - 3d cylinder

////// HEADER //////////////////////////////////////////////////////////////////

namespace TJH_DRAW_NAMESPACE
{
    // LIBRARY FUNCTIONS ///////////////////////////////////////////////////////
    
    void init();        // TODO: return and error flag or something?
    void shutdown();
    void flush();

    // STATE ///////////////////////////////////////////////////////////////////

    void clear( GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f );
    void setColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f );
    void setDepth( GLfloat depth );
    void setScale( GLfloat width, GLfloat height );
    void setScale( GLfloat x_offset, GLfloat y_offset, GLfloat width, GLfloat height );
    void setMVPMatrix( GLfloat* matrix );

    // PRIMATIVES //////////////////////////////////////////////////////////////

    void point( GLfloat x, GLfloat y );
//    void line( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
    void quad( GLfloat x, GLfloat y, GLfloat width, GLfloat height );
    void triangle( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3 );
    void circle( GLfloat x, GLfloat y, GLfloat radius, int segments = 16 );

    void triangle( GLfloat x1, GLfloat y1, GLfloat z1,
        GLfloat x2, GLfloat y2, GLfloat z2,
        GLfloat x3, GLfloat y3, GLfloat z3 );
    void quad( GLfloat x1, GLfloat y1, GLfloat z1,
        GLfloat x2, GLfloat y2, GLfloat z2,
        GLfloat x3, GLfloat y3, GLfloat z3,
        GLfloat x4, GLfloat y4, GLfloat z4 );

    void texturedQuad( GLfloat x, GLfloat y, GLfloat width, GLfloat height );
    void texturedQuad( GLfloat x, GLfloat y, GLfloat width, GLfloat height,
        GLfloat s, GLfloat t, GLfloat s_width, GLfloat t_height );
    void texturedTriangle( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3,
        GLfloat s1, GLfloat t1, GLfloat s2, GLfloat t2, GLfloat s3, GLfloat t3 );

    extern const float PI;
}

#endif

////// IMPLEMENTATION //////////////////////////////////////////////////////////
#ifdef TJH_DRAW_IMPLEMENTATION

#include <vector>
#include <cmath>

namespace TJH_DRAW_NAMESPACE
{
    // PUBLIC MEMBER VARIABLES
    const float PI = 3.14159265359;

    // 'PRIVATE' MEMBER VARIABLES
    enum class DrawMode { Colour2D, Texture2D, Colour3D, Texture3D };
    DrawMode current_mode_  = DrawMode::Colour2D;

    GLuint colour_program_   = 0;
    GLuint texture_program_  = 0; // TODO: implement

    GLuint colour_vao_   = 0;
    GLuint colour_vbo_   = 0;
    GLuint texture_vao_  = 0;
    GLuint texture_vbo_  = 0;

    float red_              = 1.0f;
    float green_            = 1.0f;
    float blue_             = 1.0f;
    float alpha_            = 1.0f;

    GLint colour_3d_mvp_uniform_    = 0;
    GLint texture_3d_mvp_uniform_   = 0;
    float width_                    = 1.0f;
    float height_                   = 1.0f;
    float depth_                    = 0.0f;
    float x_offset_                 = 0.0f;
    float y_offset_                 = 0.0f;

    GLfloat mvp_matrix_[16]         = { 0.0f };
    std::vector<GLfloat> vertex_buffer_;
    bool requires_flush_            = false;

    // 'PRIVATE' MEMBER FUNCTIONS
    void push2( GLfloat one, GLfloat two );
    void push3( GLfloat one, GLfloat two, GLfloat three );
    void push4( GLfloat one, GLfloat two, GLfloat three, GLfloat four );
    void send_ortho_matrix();
    void send_mvp_matrix();

    GLuint create_shader( GLenum type, const char* source );
    GLuint create_program( GLuint vertex_shader, GLuint fragment_shader );

    // LIBRARY FUNCTIONS ///////////////////////////////////////////////////////

    void init( GLfloat width, GLfloat height )
    {
        const char* colour_3d_vert_src =
            R"(#version 150 core
            uniform mat4 mvp;
            in vec3 vPos;
            in vec4 vCol;
            out vec4 fCol;
            void main()
            {
               fCol = vCol;
               gl_Position = mvp * vec4(vPos, 1.0);
            })";
        const char* colour_3d_frag_src =
            R"(#version 150 core
            in vec4 fCol;
            out vec4 outColour;
            void main()
            {
                outColour = fCol;
            })";

        colour_program_ = create_program(
            create_shader( GL_VERTEX_SHADER, colour_3d_vert_src ),
            create_shader( GL_FRAGMENT_SHADER, colour_3d_frag_src ) );

        colour_3d_mvp_uniform_ = glGetUniformLocation( colour_program_, "mvp" );

        glGenVertexArrays( 1, &colour_vao_ );
        glBindVertexArray( colour_vao_ );
        glGenBuffers( 1, &colour_vbo_ );
        glBindBuffer( GL_ARRAY_BUFFER, colour_vbo_ );

        GLint posAtrib = glGetAttribLocation(colour_program_, "vPos");
        if( posAtrib == -1 ) { TJH_DRAW_PRINTF("ERROR: position attribute not found in shader\n"); }
        glEnableVertexAttribArray( posAtrib );
        glVertexAttribPointer( posAtrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), 0 );

        GLint colAtrib = glGetAttribLocation(colour_program_, "vCol");
        if( colAtrib == -1 ) { TJH_DRAW_PRINTF("ERROR: Colour attribute not found in shader\n"); }
        glEnableVertexAttribArray( colAtrib );
        glVertexAttribPointer( colAtrib, 4, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(3*sizeof(float)) );

        const char* texture_3d_vert_src =
            R"(#version 150 core
            uniform mat4 mvp;
            in vec3 vPos;
            in vec4 vCol;
            in vec2 vTex;
            out vec4 fCol;
            out vec2 fTex;
            void main()
            {
               fCol = vCol;
               fTex = vTex;
               gl_Position = mvp * vec4(vPos, 1.0);
            })";
        const char* texture_3d_frag_src =
            R"(#version 150 core
            uniform sampler2D tex;
            in vec4 fCol;
            in vec2 fTex;
            out vec4 outColour;
            void main()
            {
                outColour = fCol * texture(tex, fTex);
            })";

        texture_program_ = create_program(
            create_shader( GL_VERTEX_SHADER, texture_3d_vert_src ),
            create_shader( GL_FRAGMENT_SHADER, texture_3d_frag_src ) );

        texture_3d_mvp_uniform_ = glGetUniformLocation( texture_program_, "mvp" );

        glGenVertexArrays( 1, &texture_vao_ );
        glBindVertexArray( texture_vao_ );
        glGenBuffers( 1, &texture_vbo_ );
        glBindBuffer( GL_ARRAY_BUFFER, texture_vbo_ );

        posAtrib = glGetAttribLocation( texture_program_, "vPos" );
        if( posAtrib == -1 ) { TJH_DRAW_PRINTF("ERROR: Position attribute not found in shader\n"); }
        glEnableVertexAttribArray( posAtrib );
        glVertexAttribPointer( posAtrib, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), 0 );

        colAtrib = glGetAttribLocation( texture_program_, "vCol" );
        if( colAtrib == -1 ) { TJH_DRAW_PRINTF("ERROR: Colour attribute not found in shader\n"); }
        glEnableVertexAttribArray( colAtrib );
        glVertexAttribPointer( colAtrib, 4, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(3*sizeof(float)) );

        GLint texAtrib = glGetAttribLocation( texture_program_, "vTex" );
        if( texAtrib == -1 ) { TJH_DRAW_PRINTF("ERROR: Texture attribute not found in shader\n"); }
        glEnableVertexAttribArray( texAtrib );
        glVertexAttribPointer( texAtrib, 2, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(7*sizeof(float)) );

        setScale( width, height );

        // Unbind  our state so we don't completely confuse the next person

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }

    void delete_and_zero_program( GLuint program ) { if(program){glDeleteProgram(program);program=0;} }

    void shutdown()
    {
    #define DELETE_AND_ZERO_RESOURCE( res, delete_func ) if(res){delete_func(1,&res);res=0;}
        DELETE_AND_ZERO_RESOURCE( colour_vao_, glDeleteVertexArrays );
        DELETE_AND_ZERO_RESOURCE( texture_vao_, glDeleteVertexArrays );
        DELETE_AND_ZERO_RESOURCE( colour_vbo_, glDeleteBuffers );
        DELETE_AND_ZERO_RESOURCE( texture_vbo_, glDeleteBuffers );
    #undef DELETE_AND_ZERO_RESOURCE

        delete_and_zero_program( colour_program_ );
        delete_and_zero_program( texture_program_ );
    }

    void flush()
    {
        if( !requires_flush_ ) return;

        switch( current_mode_ )
        {
        case DrawMode::Colour2D:
            glUseProgram( colour_program_ );
            glBindVertexArray( colour_vao_ );
            glBindBuffer( GL_ARRAY_BUFFER, colour_vbo_ );
            send_ortho_matrix();
        break;
        case DrawMode::Texture2D:
            glUseProgram( texture_program_ );
            glBindVertexArray( texture_vao_ );
            glBindBuffer( GL_ARRAY_BUFFER, texture_vbo_ );
            send_ortho_matrix();
        break;
        case DrawMode::Colour3D:
            glUseProgram( colour_program_ );
            glBindVertexArray( colour_vao_ );
            glBindBuffer( GL_ARRAY_BUFFER, colour_vbo_ );
            send_mvp_matrix();
        break;
        case DrawMode::Texture3D:
            glUseProgram( texture_program_ );
            glBindVertexArray( texture_vao_ );
            glBindBuffer( GL_ARRAY_BUFFER, texture_vbo_ );
            send_mvp_matrix();
        break;
        default:
            TJH_DRAW_PRINTF("ERROR: unknown draw mode!\n");
        break;
        }

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_[0]) * vertex_buffer_.size(), vertex_buffer_.data(), GL_STREAM_DRAW);
        if( current_mode_ == DrawMode::Colour2D || current_mode_ == DrawMode::Colour3D )
        {
            glDrawArrays( GL_TRIANGLES, 0, vertex_buffer_.size()/7 );
        }
        if( current_mode_ == DrawMode::Texture2D || current_mode_ == DrawMode::Texture3D )
        {
            glDrawArrays( GL_TRIANGLES, 0, vertex_buffer_.size()/9 );
        }

        vertex_buffer_.clear();
        requires_flush_ = false;
    }

    // STATE ///////////////////////////////////////////////////////////////////

    void clear( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
    {
        glClearColor( r, g, b, a );
        glClear( GL_COLOR_BUFFER_BIT );
    }
    void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a )
    {
        red_    = r;
        green_  = g;
        blue_   = b;
        alpha_  = a;
    }
    void setScale( GLfloat width, GLfloat height )
    {
        width_ = width;
        height_ = height;
    }
    void setScale( GLfloat x_offset, GLfloat y_offset, GLfloat width, GLfloat height )
    {
        x_offset_ = x_offset;
        y_offset_ = y_offset;
        height_ = height;
        width_ = width;
    }
    void setDepth( GLfloat depth )
    {
        depth_ = depth;
    }
    void setMVPMatrix( GLfloat* matrix )
    {
        std::memcpy( mvp_matrix_, matrix, sizeof(GLfloat) * 16 );
    }

    // PRIMATIVES //////////////////////////////////////////////////////////////

    //
    // Colour 2D primatives
    //
    
    void point( GLfloat x, GLfloat y )
    {
        if( current_mode_ != DrawMode::Colour2D ) flush();

        quad( x, y, 1, 1 );

        current_mode_ = DrawMode::Colour2D;
        requires_flush_ = true;
    }
//    void line( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
//    {
//        if( current_mode_ != DrawMode::Colour2D ) flush();
        // TODO: reimplement
//        glBindVertexArray(vao_);
//        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
//
//        GLfloat verts[] = {
//            x1, y1, x2, y2
//        };
//
//        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
//        glDrawArrays(GL_LINES, 0, 2);
//    }
    void quad( GLfloat x1, GLfloat y1, GLfloat width, GLfloat height )
    {
        if( current_mode_ != DrawMode::Colour2D ) flush();

        push3( x1, y1, depth_ );                    push4( red_, green_, blue_, alpha_ );
        push3( x1 + width, y1, depth_ );            push4( red_, green_, blue_, alpha_ );
        push3( x1 + width, y1 + height, depth_ );   push4( red_, green_, blue_, alpha_ );

        push3( x1, y1, depth_ );                    push4( red_, green_, blue_, alpha_ );
        push3( x1 + width, y1 + height, depth_ );   push4( red_, green_, blue_, alpha_ );
        push3( x1, y1 + height, depth_ );           push4( red_, green_, blue_, alpha_ );

        current_mode_ = DrawMode::Colour2D;
        requires_flush_ = true;
    }
    void triangle( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3 )
    {
        if( current_mode_ != DrawMode::Colour2D ) flush();

        push3( x1, y1, depth_ ); push4( red_, green_, blue_, alpha_ );
        push3( x2, y2, depth_ ); push4( red_, green_, blue_, alpha_ );
        push3( x3, y3, depth_ ); push4( red_, green_, blue_, alpha_ );

        current_mode_ = DrawMode::Colour2D;
        requires_flush_ = true;
    }
    void circle( GLfloat x, GLfloat y, GLfloat radius, int segments )
    {
        if( current_mode_ != DrawMode::Colour2D ) flush();

        float fraction = (PI*2) / (float)segments;
        for( int i = 0; i < segments; i++ )
        {
            float a1 = x + std::sin(fraction*i) * radius;
            float b1 = y + std::cos(fraction*i) * radius;
            float a2 = x + std::sin(fraction*(i+1)) * radius;
            float b2 = y + std::cos(fraction*(i+1)) * radius;
            push3(  x,  y, depth_ ); push4( red_, green_, blue_, alpha_ );
            push3( a1, b1, depth_ ); push4( red_, green_, blue_, alpha_ );
            push3( a2, b2, depth_ ); push4( red_, green_, blue_, alpha_ );
        }

        current_mode_ = DrawMode::Colour2D;
        requires_flush_ = true;
    }

    //
    // Textured 2D primatives
    //

    void texturedQuad( GLfloat x, GLfloat y, GLfloat width, GLfloat height )
    {
        if( current_mode_ != DrawMode::Texture2D ) flush();

        push3( x, y, depth_ );                  push4( red_, green_, blue_, alpha_ ); push2( 0, 0 );
        push3( x + width, y, depth_ );          push4( red_, green_, blue_, alpha_ ); push2( 1, 0 );
        push3( x + width, y + height, depth_ ); push4( red_, green_, blue_, alpha_ ); push2( 1, 1 );

        push3( x, y, depth_ );                  push4( red_, green_, blue_, alpha_ ); push2( 0, 0 );
        push3( x + width, y + height, depth_ ); push4( red_, green_, blue_, alpha_ ); push2( 1, 1 );
        push3( x, y + height, depth_ );         push4( red_, green_, blue_, alpha_ ); push2( 0, 1 );

        current_mode_ = DrawMode::Texture2D;
        requires_flush_ = true;
    }
    void texturedQuad( GLfloat x, GLfloat y, GLfloat width, GLfloat height,
        GLfloat s, GLfloat t, GLfloat s_width, GLfloat t_height )
    {
        if( current_mode_ != DrawMode::Texture2D ) flush();

        push3( x, y, depth_ );                  push4( red_, green_, blue_, alpha_ ); push2( s, t );
        push3( x + width, y, depth_ );          push4( red_, green_, blue_, alpha_ ); push2( s + s_width, t );
        push3( x + width, y + height, depth_ ); push4( red_, green_, blue_, alpha_ ); push2( s + s_width, t + t_height );

        push3( x, y, depth_ );                  push4( red_, green_, blue_, alpha_ ); push2( s, t );
        push3( x + width, y + height, depth_ ); push4( red_, green_, blue_, alpha_ ); push2( s + s_width, t + t_height );
        push3( x, y + height, depth_ );         push4( red_, green_, blue_, alpha_ ); push2( s, t + t_height );

        current_mode_ = DrawMode::Texture2D;
        requires_flush_ = true;
    }
    void texturedTriangle( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3,
        GLfloat s1, GLfloat t1, GLfloat s2, GLfloat t2, GLfloat s3, GLfloat t3 )
    {
        if( current_mode_ != DrawMode::Texture2D ) flush();

        push3( x1, y1, depth_ ); push4( red_, green_, blue_, alpha_ ); push2( s1, t1 );
        push3( x2, y2, depth_ ); push4( red_, green_, blue_, alpha_ ); push2( s2, t2 );
        push3( x3, y3, depth_ ); push4( red_, green_, blue_, alpha_ ); push2( s3, t3 );

        current_mode_ = DrawMode::Texture2D;
        requires_flush_ = true;
    }

    //
    // Colour 3D primatives
    //
    
    void triangle( GLfloat x1, GLfloat y1, GLfloat z1,
        GLfloat x2, GLfloat y2, GLfloat z2,
        GLfloat x3, GLfloat y3, GLfloat z3 )
    {
        if( current_mode_ != DrawMode::Colour3D ) flush();

        push3( x1, y1, z1 ); push4( red_, green_, blue_, alpha_ );
        push3( x2, y2, z2 ); push4( red_, green_, blue_, alpha_ );
        push3( x3, y3, z3 ); push4( red_, green_, blue_, alpha_ );

        current_mode_ = DrawMode::Colour3D;
        requires_flush_ = true;
    }
    void quad( GLfloat x1, GLfloat y1, GLfloat z1,
        GLfloat x2, GLfloat y2, GLfloat z2,
        GLfloat x3, GLfloat y3, GLfloat z3,
        GLfloat x4, GLfloat y4, GLfloat z4 )
    {
        if( current_mode_ != DrawMode::Colour3D ) flush();

        push3( x1, y1, z1 ); push4( red_, green_, blue_, alpha_ );
        push3( x2, y2, z2 ); push4( red_, green_, blue_, alpha_ );
        push3( x3, y3, z3 ); push4( red_, green_, blue_, alpha_ );
        push3( x1, y1, z1 ); push4( red_, green_, blue_, alpha_ );
        push3( x3, y3, z3 ); push4( red_, green_, blue_, alpha_ );
        push3( x4, y4, z4 ); push4( red_, green_, blue_, alpha_ );

        current_mode_ = DrawMode::Colour3D;
        requires_flush_ = true;
    }

    // UTILS //////////////////////////////////////////////////////////////////
    GLuint create_shader( GLenum type, const char* source )
    {
        GLuint shader = glCreateShader( type );
        if( shader == 0 )
        {
            TJH_DRAW_PRINTF("ERROR: cold not create shaders!"); 
            return shader;
        }

        // Compile and check the vertex shader
        glShaderSource( shader, 1, &source, NULL );
        glCompileShader( shader );
        GLint status = GL_TRUE;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
        if( status != GL_TRUE )
        {
            TJH_DRAW_PRINTF( "ERROR: draw could not compile %d shader\n", type );
            // Get the length of the error log
            GLint log_length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

            // Now get the error log itself
            GLchar buffer[log_length];
            glGetShaderInfoLog( shader, log_length, NULL, buffer );
            TJH_DRAW_PRINTF("%s\n", buffer);

            // Clean up the resources, not that it really matters at this point
            // we should really just crash
            glDeleteShader( shader );
            shader = 0;
        }

        return shader;
    }
    GLuint create_program( GLuint vertex_shader, GLuint fragment_shader )
    {
        GLuint program = glCreateProgram();
        glAttachShader( program, vertex_shader );
        glAttachShader( program, fragment_shader );
        glBindFragDataLocation( program, 0, "outColour");
        glLinkProgram( program );
        glUseProgram( program );
        glDeleteShader( vertex_shader );
        glDeleteShader( fragment_shader );
        return program;
    }
    void push2( GLfloat one, GLfloat two )
    {
        vertex_buffer_.push_back( one );
        vertex_buffer_.push_back( two );
    }
    void push3( GLfloat one, GLfloat two, GLfloat three )
    {
        vertex_buffer_.push_back( one );
        vertex_buffer_.push_back( two );
        vertex_buffer_.push_back( three );
    }
    void push4( GLfloat one, GLfloat two, GLfloat three, GLfloat four )
    {
        vertex_buffer_.push_back( one );
        vertex_buffer_.push_back( two );
        vertex_buffer_.push_back( three );
        vertex_buffer_.push_back( four );
    }
    void send_ortho_matrix()
    {
        GLfloat xs =  2.0f / width_;     // x scale
        GLfloat ys = -2.0f / height_;    // y scale
        GLfloat xo = -1.0f - x_offset_ / width_; // x offset
        GLfloat yo =  1.0f - y_offset_ / height_; // y offset
        GLfloat proj[16] = {
            xs,  0,  0,  0,
             0, ys,  0,  0,
             0,  0,  1,  0,
            xo, yo,  0,  1
        };
        glUniformMatrix4fv( colour_3d_mvp_uniform_, 1, GL_FALSE, proj );
        glUniformMatrix4fv( texture_3d_mvp_uniform_, 1, GL_FALSE, proj );
    }
    void send_mvp_matrix()
    {
        glUniformMatrix4fv( colour_3d_mvp_uniform_, 1, GL_FALSE, mvp_matrix_ );
        glUniformMatrix4fv( texture_3d_mvp_uniform_, 1, GL_FALSE, mvp_matrix_ );
    }
}
// Prevent the implementation from leaking into subsequent includes
#undef TJH_DRAW_IMPLEMENTATION
#endif
