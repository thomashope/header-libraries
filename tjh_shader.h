#ifndef TJH_SHADER_H
#define TJH_SHADER_H

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
// GLEW v2.0.0 which is Copyright (C) Nate Robins - 1997, Michael Wimmer - 1999
// Milan Ikits - 2002-2008, Nigel Stewart - 2008-2013 and distributed under the
// Modified BSD License, Mesa 3-D Licsense (MIT) and the Khronos License (MIT).
// See here for more http://glew.sourceforge.net/

////// TODO ////////////////////////////////////////////////////////////////////
//
// - write the readme
// - add geometry shader
// - multiple output buffers
// - simplify vao attrib layout specification

////// EXAMPLE SHADER CODE /////////////////////////////////////////////////////
//
// Some basic GLSL code to get you started
/*
// A handy macro to help you define glsl directly in your source code
// produces unhelpful error messages
#define GLSL(src) "#version 150 core\n" #src

const GLchar* vert_src = GLSL(
    in vec3 vPos;
    in vec3 vCol;
    out vec3 fCol;
    void main()
    {
        gl_Position = vec4(vPos, 1.0);
        fCol = vCol;
    }
    );

const GLchar* frag_src = GLSL(
    in vec3 fCol;
    out vec4 outDiffuse;
    void main()
    {
       outDiffuse = vec4(fCol, 1);
    }
    );
*/

////// LIBRARY OPTIONS /////////////////////////////////////////////////////////
//
// You can comment or uncomment the following #defines to enable or disable
// additional library features.
//
// Change this to customise the namespace of the library (or rather, the name of the class)
#define TJH_SHADER_TYPENAME Shader
// Change this to use a custom printf like function for your platform, for example SDL_Log
#define TJH_SHADER_PRINTF printf
// Either set this correctly or comment it out if you would preffer tjh_shader.h did not include glew
#define TJH_SHADER_GLEW_H_LOCATION <GL/glew.h>

////// HEADER //////////////////////////////////////////////////////////////////

#ifdef TJH_SHADER_GLEW_H_LOCATION
    #include TJH_SHADER_GLEW_H_LOCATION
#endif
#include <string>

class TJH_SHADER_TYPENAME
{
public:
    TJH_SHADER_TYPENAME();
    ~TJH_SHADER_TYPENAME();

    // Returns true if shader compilation was a success, false if there was an error
    bool init();
    // Can be used to explicitly clean up OpenGL resources, not required
    void shutdown();

    // Returns true if the source files were loaded successfully
    // false if they could not be loaded
    bool loadVertexSourceFile( std::string file_path );
    bool loadFragmentSourceFile( std::string file_path ); 
    bool loadGeometrySourceFile( std::string file_path ); 

    // Sets the shader source strings directly
    void setVertexSourceString( const std::string& source ) { vertex_source_ = source; }
    void setFragmentSourceString( const std::string& source ) { fragment_source_ = source; }
    void setGeometrySourceString( const std::string& source ) { geometry_source_ = source; tried_set_geometry_source_ = true; }

    static void setShaderBasePath( std::string path ) { shader_base_path_ = path; }

    // Bind the shader to the OpenGL context, ready for use
    void bind() const { glUseProgram( program_ ); }
    void unbind() const { glUseProgram( 0 ); }

    // Don't forget to bind shaders before trying to get uniforms or attributes
    GLint getUniformLocation( const GLchar* name ) const;
    GLint getAttribLocation( const GLchar* name ) const;
    GLint getProgram() const { return program_; }

private:
    // Loads the text file 'filename' and passes the contents to the pointer
    // returns true on success
    bool load_file( std::string filename, std::string& file_contents  );
    // returns true on success
    bool compile_shader( GLenum type, GLuint& shader, const std::string& source );
    // returns true if the shader did compile ok
    bool did_shader_compile_ok( GLuint shader );

    // Converts GLenums such as GL_VERTEX_SHADER to a string
    std::string glenumShaderTypeToString( GLenum type );

    GLuint program_;
    GLuint vertex_shader_;
    GLuint fragment_shader_;
    GLuint geometry_shader_;

    std::string vertex_source_;
    std::string fragment_source_;
    std::string geometry_source_;
    bool tried_set_geometry_source_;

    static std::string shader_base_path_;
};

#endif // END TJH_SHADER_H

////// IMPLEMENTATION //////////////////////////////////////////////////////////
#ifdef TJH_SHADER_IMPLEMENTATION

// TODO: can I reduce these includes?
#include <fstream>
#include <sstream>

// Shader base path is found by SDL the first time it is needed
std::string TJH_SHADER_TYPENAME::shader_base_path_ = "";

#ifdef _WIN32
    #define PATH_SEPERATOR '\\'
#else
    #define PATH_SEPERATOR '/'
#endif

TJH_SHADER_TYPENAME::TJH_SHADER_TYPENAME() :
program_(0),
vertex_shader_(0),
fragment_shader_(0),
geometry_shader_(0),
tried_set_geometry_source_(false)
{}

TJH_SHADER_TYPENAME::~TJH_SHADER_TYPENAME()
{
    shutdown();
}

bool TJH_SHADER_TYPENAME::init()
{
    program_ = glCreateProgram();

    bool compiled_vertex_source = compile_shader( GL_VERTEX_SHADER, vertex_shader_, vertex_source_ );
    if( compiled_vertex_source )
    {
        glAttachShader( program_, vertex_shader_ );
    }

    bool compiled_fragment_source = compile_shader( GL_FRAGMENT_SHADER, fragment_shader_, fragment_source_ );
    if( compiled_fragment_source )
    {
        glAttachShader( program_, fragment_shader_ );
    }

    // If the programmer tied to use a geometry shader, try to compile and attach it
    bool compiled_geometry_source = false;
    if( tried_set_geometry_source_ )
    {
        compiled_geometry_source = compile_shader( GL_GEOMETRY_SHADER, geometry_shader_, geometry_source_ );
        if( compiled_geometry_source )
        {
            glAttachShader( program_, geometry_shader_ );
        }
    }

    bool everything_ok = compiled_fragment_source
                         && compiled_vertex_source
                         && (tried_set_geometry_source_ == compiled_geometry_source);

    if( everything_ok ) {
         // Link the shaders together into a program
        glBindFragDataLocation( program_, 0, "outDiffuse" );
        glLinkProgram( program_ );
    }

    // now cleanup the shaders
    // TODO: it's possible we want to the same vertex/fagment/etc. shader multple times in different programs
    // TODO: find a way of supporting this without the caller having to do extra work, on the other hand it might not be worth it
    // TODO: will this cause a problem if I try to delete a shader that was never created?
    //  If the shader is not created the value will be left at 0, which could be a valid shader

    if( compiled_vertex_source ) {
        glDetachShader( program_, vertex_shader_ );
        glDeleteShader( vertex_shader_ );
        vertex_shader_ = 0;
    }

    if( compiled_fragment_source ) {
        glDetachShader( program_, fragment_shader_ );
        glDeleteShader( fragment_shader_ );
        fragment_shader_ = 0;
    }

    if( tried_set_geometry_source_ && compiled_geometry_source )
    {
        glDetachShader( program_, geometry_shader_ );
        glDeleteShader( geometry_shader_ ); 
        geometry_shader_ = 0;
    }

    // Clear all the strings
    vertex_source_.clear();
    vertex_source_.shrink_to_fit();
    fragment_source_.clear();
    fragment_source_.shrink_to_fit();
    geometry_source_.clear();
    geometry_source_.shrink_to_fit();

    return everything_ok;
}

void TJH_SHADER_TYPENAME::shutdown()
{
    if( program_ ) {
        glDeleteProgram( program_ );
        program_ = 0;
    }
    if( vertex_shader_ ) {
        glDeleteShader( vertex_shader_ );
        vertex_shader_ = 0;
    }
    if( fragment_shader_ ) {
        glDeleteShader( fragment_shader_ );
        fragment_shader_ = 0;
    }
    if( geometry_shader_ ) {
        glDeleteShader( geometry_shader_ );
        geometry_shader_ = 0;
    }
}

GLint TJH_SHADER_TYPENAME::getUniformLocation( const GLchar* name ) const
{
    GLint uniform = glGetUniformLocation( program_, name );
    if( uniform == -1 ) {
        TJH_SHADER_PRINTF("ERROR: did not find uniform '%s' in shader program '%i'\n", name, program_ );
    }
    return uniform;
}


GLint TJH_SHADER_TYPENAME::getAttribLocation( const GLchar* name ) const
{
    GLint attribute = glGetAttribLocation( program_, name );
    if( attribute == -1 ) {
        TJH_SHADER_PRINTF("ERROR: did not find attribute '%s' in shader program '%i'\n", name, program_ );
    }
    return attribute;
}

bool TJH_SHADER_TYPENAME::loadVertexSourceFile( std::string file_path )
{
    return load_file( file_path, vertex_source_ );
}

bool TJH_SHADER_TYPENAME::loadFragmentSourceFile( std::string file_path )
{
    return load_file( file_path, fragment_source_ );
}

bool TJH_SHADER_TYPENAME::loadGeometrySourceFile( std::string file_path )
{
    tried_set_geometry_source_ = true;
    return load_file( file_path, geometry_source_ );
}

bool TJH_SHADER_TYPENAME::load_file( std::string filename, std::string& file_contents )
{   
    std::ifstream file( shader_base_path_ + filename );
    if( !file.good() )
    {
        TJH_SHADER_PRINTF("ERROR: Could not load '%s%s'!\n", shader_base_path_.c_str(), filename.c_str() );
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    file_contents = buffer.str();

    return true;
}

bool TJH_SHADER_TYPENAME::compile_shader( GLenum type, GLuint& shader, const std::string& source )
{
    if( source.empty() )
    {
        TJH_SHADER_PRINTF( "ERROR: '%s' source was empty! (not loaded/set)\n", glenumShaderTypeToString(type).c_str() );
        return false;
    }

    shader = glCreateShader( type );
    if( shader == 0 )
    {
        TJH_SHADER_PRINTF( "ERROR: could not create shader, %s\n", glenumShaderTypeToString(type).c_str() );
        return false;
    }

    // Shader strings have to be converted to const GLchar* so OpenGL can compile them
    const GLchar* source_ptr = (const GLchar*)source.c_str();

    glShaderSource( shader, 1, &source_ptr, NULL );
    glCompileShader( shader );

    return did_shader_compile_ok( shader );
}

bool TJH_SHADER_TYPENAME::did_shader_compile_ok( GLuint shader )
{    
    // Check the shader was compiled succesfully
    GLint status;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
    if( status != GL_TRUE )
    {
        // Get the length of the error log
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        // Now get the error log itself
        GLchar buffer[log_length];
        glGetShaderInfoLog( shader, log_length, NULL, buffer );

        // Print the error
        // TODO: if we have the source then could we print the line that was broken?
        GLint type;
        glGetShaderiv( shader, GL_SHADER_TYPE, &type );
        TJH_SHADER_PRINTF( "ERROR: compiling shader %s\n", glenumShaderTypeToString(type).c_str() );
        TJH_SHADER_PRINTF( "%s", buffer );
        return false;
    }

    return true;
}

std::string TJH_SHADER_TYPENAME::glenumShaderTypeToString( GLenum type )
{
    switch( type )
    {
    case GL_VERTEX_SHADER:          return "GL_VERTEX_SHADER";
    case GL_TESS_CONTROL_SHADER:    return "GL_TESS_CONTROL_SHADER";
    case GL_TESS_EVALUATION_SHADER: return "GL_TESS_EVALUATION_SHADER";
    case GL_GEOMETRY_SHADER:        return "GL_GEOMETRY_SHADER";
    case GL_FRAGMENT_SHADER:        return "GL_FRAGMENT_SHADER";
    case GL_COMPUTE_SHADER:         return "GL_COMPUTE_SHADER";
    default: return "UNKNOWN";
    }
}

// Prevent the macros from leaking into the global namespace
#undef PATH_SEPERATOR

// Prevent the implementation from leaking into subsequent includes
#undef TJH_SHADER_IMPLEMENTATION
#endif // END TJH_SHADER_IMPLEMENTATION
