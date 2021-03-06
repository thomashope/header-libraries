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
// Compiler that supports C++11 Features!
//
// GLEW v2.0.0 which is Copyright (C) Nate Robins - 1997, Michael Wimmer - 1999
// Milan Ikits - 2002-2008, Nigel Stewart - 2008-2013 and distributed under the
// Modified BSD License, Mesa 3-D Licsense (MIT) and the Khronos License (MIT).
// See here for more http://glew.sourceforge.net/

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
#define TJH_SHADER_GLEW_H <GL/glew.h>

////// TODO ////////////////////////////////////////////////////////////////////
//
// - write the readme
// - make sure we can recompile the shader cleanly by setting the source strings
//   embedding file IO in the shader class is not necessarily best practice, though good for getting stuff going
// - multiple output buffers
// - OPTIMISATION: only reload files that changed
// - test OpenGL ES compatability
// - DONE: apply rule of 3/5/0. Delete copy constructors but implemenet move constructors?
//  - TEST: that I did the move copy/assignment correctly???

////// HEADER //////////////////////////////////////////////////////////////////

#ifdef TJH_SHADER_GLEW_H
    #include TJH_SHADER_GLEW_H
#endif
#include <string>

class TJH_SHADER_TYPENAME
{
public:
    TJH_SHADER_TYPENAME() {};
    ~TJH_SHADER_TYPENAME();

    // Do not allow copy construction or assignment
    TJH_SHADER_TYPENAME( const TJH_SHADER_TYPENAME& other ) = delete;
    TJH_SHADER_TYPENAME& operator = ( const TJH_SHADER_TYPENAME& other ) = delete;

    // Allow move constructor and assignment operators
    TJH_SHADER_TYPENAME( TJH_SHADER_TYPENAME&& other );
    TJH_SHADER_TYPENAME& operator = ( TJH_SHADER_TYPENAME&& other );

    // Returns true if shader compilation was a success, false if there was an error
    bool compileAndLink();

    // Can be used to explicitly clean up OpenGL resources, automatically called by destructor
    void shutdown();

    // If shader source is a file, reloads and recompiles
    // If reloading fails, the shader will not be changed
    // NOTE: things like uniform locations may change as a result!
    bool reload();

    // Call this so we know where to look for the shaders
    static void setShaderBasePath( std::string path ) { shader_base_path_ = path; }

    struct VertexAttribArrayDesc {
        std::string name;
        GLint count;
        GLenum type;
        GLboolean normalized;
    };

    // NOTE: preliminary reading suggest this is bound to the VAO, not the shader.
    // If so this would have to be called for each shader bound, how could I indicate this?
    // Saves calling a whole heap of gl functions, use it like so:
    // shader.setVertexAttribArray({ {"pos", 3, GL_FLOAT, GL_FALSE}, {"col", 3, GL_FLOAT, GL_FALSE} });
    bool setVertexAttribArrays( GLuint vao, const std::initializer_list<VertexAttribArrayDesc>& desc_list );

    // Returns true if the source files were loaded successfully
    // false if they could not be loaded
    bool loadVertexSourceFile( std::string filename );
    bool loadFragmentSourceFile( std::string filename ); 
    bool loadGeometrySourceFile( std::string filename ); 

    // Sets the shader source strings directly
    void setVertexSourceString( const std::string& source );
    void setFragmentSourceString( const std::string& source );
    void setGeometrySourceString( const std::string& source );

    // Bind the shader to the OpenGL context, ready for use
    void bind()   const { glUseProgram( program_ ); }
    void unbind() const { glUseProgram( 0 ); }

    // Don't forget to bind shaders before trying to get uniforms or attributes
    GLint getUniformLocation( const GLchar* name ) const;
    GLint getAttribLocation( const GLchar* name ) const;
    GLuint getProgram() const { return program_; }

private:

    // Loads the text file 'filename' and sets file_content to the content of the file
    // returns true on success
    bool load_file( std::string filename, std::string& file_content ) const;

    // Tries to compile 'source' as a shader with the given 'type', stores result in 'shader'
    // returns true on success
    bool compile_shader( GLenum type, GLuint& shader, const std::string& source ) const;

    // Checks the given shader for errors, printing them if any were found
    // returns true if the shader did compile ok
    bool did_shader_compile_ok( GLuint shader ) const;

    // Converts GLenums such as GL_VERTEX_SHADER to a string
    std::string glenum_shader_to_string( GLenum shader ) const;

    // Get the size in bytes of a GL_xxx type such as GL_FLOAT or GL_SHORT
    int glenum_type_to_size_in_bytes( GLenum type ) const;

    GLuint program_             = 0;
    GLuint vertex_shader_       = 0;
    GLuint fragment_shader_     = 0;
    GLuint geometry_shader_     = 0;

    std::string vertex_source_filename_     = "";
    std::string fragment_source_filename_   = "";
    std::string geometry_source_filename_   = "";

    std::string vertex_source_              = "";
    std::string fragment_source_            = "";
    std::string geometry_source_            = "";
    bool tried_set_geometry_source_         = false;

    static std::string shader_base_path_;

    // Set all member variables to their defaults without deleting resources
    void resetMembersToDefaults();
};

#endif // END TJH_SHADER_H

////// IMPLEMENTATION //////////////////////////////////////////////////////////
#ifdef TJH_SHADER_IMPLEMENTATION

#include <fstream>
#include <sstream>
#include <vector>

// Shader base path is found by SDL the first time it is needed
std::string TJH_SHADER_TYPENAME::shader_base_path_ = "";

#ifdef _WIN32
    #define PATH_SEPERATOR '\\'
#else
    #define PATH_SEPERATOR '/'
#endif

// Destructor
TJH_SHADER_TYPENAME::~TJH_SHADER_TYPENAME()
{
    shutdown();
}
// Move constructor
TJH_SHADER_TYPENAME::TJH_SHADER_TYPENAME( TJH_SHADER_TYPENAME&& other )
{
    program_            = other.program_;
    vertex_shader_      = other.vertex_shader_;
    fragment_shader_    = other.fragment_shader_;
    geometry_shader_    = other.geometry_shader_;

    vertex_source_filename_     = other.vertex_source_filename_;
    fragment_source_filename_   = other.fragment_source_filename_;
    geometry_source_filename_   = other.geometry_source_filename_;

    vertex_source_              = other.vertex_source_;
    fragment_source_            = other.fragment_source_;
    geometry_source_            = other.geometry_source_;
    tried_set_geometry_source_  = other.tried_set_geometry_source_;

    other.resetMembersToDefaults();
}
// Move assignment operator
TJH_SHADER_TYPENAME& TJH_SHADER_TYPENAME::operator = ( TJH_SHADER_TYPENAME&& other )
{
    if( this != &other )
    {
        program_            = other.program_;
        vertex_shader_      = other.vertex_shader_;
        fragment_shader_    = other.fragment_shader_;
        geometry_shader_    = other.geometry_shader_;

        vertex_source_filename_     = other.vertex_source_filename_;
        fragment_source_filename_   = other.fragment_source_filename_;
        geometry_source_filename_   = other.geometry_source_filename_;

        vertex_source_              = other.vertex_source_;
        fragment_source_            = other.fragment_source_;
        geometry_source_            = other.geometry_source_;
        tried_set_geometry_source_  = other.tried_set_geometry_source_;

        other.resetMembersToDefaults();
    }
    return *this;
}

void TJH_SHADER_TYPENAME::resetMembersToDefaults()
{
    program_             = 0;
    vertex_shader_       = 0;
    fragment_shader_     = 0;
    geometry_shader_     = 0;

    vertex_source_filename_     = "";
    fragment_source_filename_   = "";
    geometry_source_filename_   = "";

    vertex_source_              = "";
    fragment_source_            = "";
    geometry_source_            = "";
    tried_set_geometry_source_  = false; 
}

bool TJH_SHADER_TYPENAME::compileAndLink()
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
    }

    if( compiled_fragment_source ) {
        glDetachShader( program_, fragment_shader_ );
    }

    if( tried_set_geometry_source_ && compiled_geometry_source )
    {
        glDetachShader( program_, geometry_shader_ );
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

bool TJH_SHADER_TYPENAME::reload()
{
    GLuint new_program              = glCreateProgram();
    GLuint new_vertex_shader        = 0;
    GLuint new_fragment_shader      = 0;
    GLuint new_geometry_shader      = 0;
    bool compiled_new_shaders_ok    = false;

    // Try to load and recompile the vertex shader
    if( !vertex_source_filename_.empty() )
    {
        compiled_new_shaders_ok = loadVertexSourceFile( vertex_source_filename_ )
                                && compile_shader( GL_VERTEX_SHADER, new_vertex_shader, vertex_source_ );
        if( !compiled_new_shaders_ok )
            TJH_SHADER_PRINTF("ERROR: failed to recompile vertex shader in program '%i'\n", program_ );
    }

    // Try to load and recompile the fragment shader
    if( !fragment_source_filename_.empty() && compiled_new_shaders_ok )
    {
        compiled_new_shaders_ok = loadFragmentSourceFile( fragment_source_filename_ )
                                && compile_shader( GL_FRAGMENT_SHADER, new_fragment_shader, fragment_source_ );
        if( !compiled_new_shaders_ok )
            TJH_SHADER_PRINTF("ERROR: failed to recompile fragment shader in program '%i'\n", program_ );
    }

    // Try to load and recompile the geometry shader
    if( !geometry_source_filename_.empty() && compiled_new_shaders_ok )
    {
        compiled_new_shaders_ok = loadGeometrySourceFile( geometry_source_filename_ )
                                && compile_shader( GL_GEOMETRY_SHADER, new_geometry_shader, geometry_source_ );
        if( !compiled_new_shaders_ok )
            TJH_SHADER_PRINTF("ERROR: failed to recompile geometry shader in program '%i'\n", program_ );
    }
    
    if( compiled_new_shaders_ok )
    {
        // Attach and create the shader new programs
        glBindFragDataLocation( new_program, 0, "outDiffuse" );
        glAttachShader( new_program, new_vertex_shader );
        glAttachShader( new_program, new_fragment_shader );
        glAttachShader( new_program, new_geometry_shader );
        glLinkProgram( new_program );

        // Call shutdown to free the old resources
        shutdown();

        // Now substitute our new resources
        program_            = new_program;
        vertex_shader_      = new_vertex_shader;
        fragment_shader_    = new_vertex_shader;
        geometry_shader_    = new_geometry_shader;
    }
    else
    {
        // If something went wrong, free any resources that were created
        if( new_program ) {
            glDeleteProgram( new_program );
        }
        if( new_vertex_shader ) {
            glDeleteShader( new_vertex_shader );
        }
        if( new_fragment_shader ) {
            glDeleteShader( new_fragment_shader );
        }
        if( new_geometry_shader ) {
            glDeleteShader( new_geometry_shader );
        }
    }

    return compiled_new_shaders_ok;
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

bool TJH_SHADER_TYPENAME::setVertexAttribArrays( GLuint vao, const std::initializer_list<VertexAttribArrayDesc>& desc_list )
{
    bool success = true;
    std::vector<long unsigned> offset_list;

    GLsizei stride = 0;
    for( const auto& desc : desc_list )
    {
        int size = glenum_type_to_size_in_bytes( desc.type );
        stride += size * desc.count;
        offset_list.push_back( stride - size * desc.count );
    }
    
    // Bind the vao so we know we are working with the right one
    glBindVertexArray( vao );

    // Manual loop counter because initalizer lists like ranged for loops
    // but we also want and index for the vector
    size_t i = 0;
    for( const auto& desc : desc_list )
    {
        GLint attrib = getAttribLocation( desc.name.c_str() );
        glEnableVertexAttribArray( attrib );
    	glVertexAttribPointer(
            attrib,
            desc.count,
            desc.type,
            desc.normalized,
            stride,
            (void*)(offset_list[i]) );
        i++;
    }

    return success;
}

bool TJH_SHADER_TYPENAME::loadVertexSourceFile( std::string filename )
{
    vertex_source_filename_ = filename;
    return load_file( filename, vertex_source_ );
}

bool TJH_SHADER_TYPENAME::loadFragmentSourceFile( std::string filename )
{
    fragment_source_filename_ = filename;
    return load_file( filename, fragment_source_ );
}

bool TJH_SHADER_TYPENAME::loadGeometrySourceFile( std::string filename )
{
    geometry_source_filename_ = filename;
    tried_set_geometry_source_ = true;
    return load_file( filename, geometry_source_ );
}

void TJH_SHADER_TYPENAME::setVertexSourceString( const std::string& source )
{
    vertex_source_ = source;
    vertex_source_filename_.clear();
}

void TJH_SHADER_TYPENAME::setFragmentSourceString( const std::string& source )
{
    fragment_source_ = source;
    fragment_source_filename_.clear();
}

void TJH_SHADER_TYPENAME::setGeometrySourceString( const std::string& source )
{
    geometry_source_ = source;
    geometry_source_filename_.clear();
    tried_set_geometry_source_ = true;
}

bool TJH_SHADER_TYPENAME::load_file( std::string filename, std::string& file_content ) const
{   
    std::ifstream file( shader_base_path_ + filename );
    if( !file.good() )
    {
        TJH_SHADER_PRINTF("ERROR: Could not load '%s%s'!\n", shader_base_path_.c_str(), filename.c_str() );
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    file_content = buffer.str();

    return true;
}

bool TJH_SHADER_TYPENAME::compile_shader( GLenum type, GLuint& shader, const std::string& source ) const
{
    if( source.empty() )
    {
        TJH_SHADER_PRINTF( "ERROR: '%s' source was empty! (not loaded/set)\n", glenum_shader_to_string(type).c_str() );
        return false;
    }

    shader = glCreateShader( type );
    if( shader == 0 )
    {
        TJH_SHADER_PRINTF( "ERROR: could not create shader, %s\n", glenum_shader_to_string(type).c_str() );
        return false;
    }

    // Shader strings have to be converted to const GLchar* so OpenGL can compile them
    const GLchar* source_ptr = static_cast<const GLchar*>(source.c_str());

    glShaderSource( shader, 1, &source_ptr, NULL );
    glCompileShader( shader );

    return did_shader_compile_ok( shader );
}

bool TJH_SHADER_TYPENAME::did_shader_compile_ok( GLuint shader ) const
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
        std::vector<GLchar> buffer;
        buffer.reserve( static_cast<size_t>(log_length) );
        glGetShaderInfoLog( shader, log_length, NULL, buffer.data() );

        // Print the error
        // TODO: if we have the source then could we print the line that was broken?
        GLint type;
        glGetShaderiv( shader, GL_SHADER_TYPE, &type );
        TJH_SHADER_PRINTF( "ERROR: compiling shader %s\n", glenum_shader_to_string(static_cast<GLenum>(type)).c_str() );
        TJH_SHADER_PRINTF( "%s", buffer.data() );
        return false;
    }

    return true;
}

std::string TJH_SHADER_TYPENAME::glenum_shader_to_string( GLenum shader ) const
{
    switch( shader )
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

int TJH_SHADER_TYPENAME::glenum_type_to_size_in_bytes( GLenum type ) const
{
    switch( type )
    {
        case GL_BYTE:               return sizeof(GLbyte);
        case GL_UNSIGNED_BYTE:      return sizeof(GLubyte);
        case GL_SHORT:              return sizeof(GLshort);
        case GL_UNSIGNED_SHORT:     return sizeof(GLushort);
        case GL_INT:                return sizeof(GLint);
        case GL_UNSIGNED_INT:       return sizeof(GLuint);
        case GL_FIXED:              return sizeof(GLfixed);
        case GL_HALF_FLOAT:         return sizeof(GLhalf);
        case GL_FLOAT:              return sizeof(GLfloat);
        case GL_DOUBLE:             return sizeof(GLdouble);
        default:
            TJH_SHADER_PRINTF( "ERROR: unknown type in glenumTypeToSizeInBytes %d\n", type );
            return 0;
    }
}

// Prevent the macros from leaking into the global namespace
#undef PATH_SEPERATOR

// Prevent the implementation from leaking into subsequent includes
#undef TJH_SHADER_IMPLEMENTATION
#endif // END TJH_SHADER_IMPLEMENTATION
