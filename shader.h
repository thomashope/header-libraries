#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

// TODO: add geometry shader
// TODO: multiple output buffers
// TODO: simplify vao attrib layout specification

////// README //////////////////////////////////////////////////////////////////
//
// TODO: write the readme...

////// EXAMPLE SHADER CODE /////////////////////////////////////////////////////
//
// Some basic GLSL code to get you started
/*
const GLchar* vert_src =
    "#version 150\n"
    "in vec3 vPos;"
    "in vec3 vCol;"
    "out vec3 fCol;"
    "void main()"
    "{"
    "   gl_Position = vec4(vPos, 1.0);"
    "   fCol = vCol;"
    "}";
*//*
const GLchar* frag_src =
    "#version 150\n"
    "in vec3 fCol;"
    "out vec4 outDiffuse;"
    "void main()"
    "{"
    "   outDiffuse = vec4(fCol, 1);"
    "}";
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

#define TJH_WINDOW_SDL_H_LOCATION <SDL2/SDL.h>
#define TJH_WINDOW_GLEW_H_LOCATION <GL/glew.h>

////// HEADER //////////////////////////////////////////////////////////////////

// TODO: can i reduce these includes? Remove dependancy on SDL2? At least in header section?
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <string>

class TJH_SHADER_TYPENAME
{
public:
    TJH_SHADER_TYPENAME();
    ~TJH_SHADER_TYPENAME() {}

    // Returns true if shader compilation was a success, false if there was an error
    bool init();
    void shutdown();

    // Returns true if the source files were loaded successfully, false if they could not be loaded
    bool loadVertexSourceFile( std::string file_path );
    bool loadFragmentSourceFile( std::string file_path ); 

    // Sets the shader source strings directly
    void setVertexSourceString( const std::string& source ) { vertex_source_ = source; }
    void setFragmentSourceString( const std::string& source ) { fragment_source_ = source; }

    // Bind the shader to the OpenGL context, ready for use
    void bind();

    // Don't forget to bind shaders before trying to get uniforms or attributes
    GLint getUniformLocation( const GLchar* name );
    GLint getAttribLocation( const GLchar* name );
    GLint getProgram() { return program_; }

private:
    // Loads the text file 'filename' and passes the contents to the pointer
    bool load_file( std::string filename, std::string* file_contents  );
    bool did_shader_compile_ok( GLuint shader );
    void get_shader_base_path();

    std::string vertex_source_;
    std::string fragment_source_;

    GLuint program_;
    GLuint vertex_shader_;
    GLuint fragment_shader_;

    static std::string shader_base_path_;
};

#endif

////// IMPLEMENTATION //////////////////////////////////////////////////////////
#ifdef TJH_SHADER_IMPLEMENTATION

// TODO: can I reduce these includes?
#include <fstream>
#include <sstream>

// Shader base path is found by SDL the first time it is needed
std::string TJH_SHADER_TYPENAME::shader_base_path_ = "";
// The folder to look in relative to the base path
#define SHADER_FOLDER "shaders"

#ifdef _WIN32
    #define PATH_SEPERATOR '\\'
#else
    #define PATH_SEPERATOR '/'
#endif

TJH_SHADER_TYPENAME::TJH_SHADER_TYPENAME() :
vertex_shader_(0),
fragment_shader_(0)
{}

bool TJH_SHADER_TYPENAME::init()
{
    bool success = true;
    const GLchar* source_ptr = nullptr;

    if( !vertex_source_.empty() )
    {
        vertex_shader_ = glCreateShader( GL_VERTEX_SHADER );
        if( vertex_shader_ == 0 )
        {
            TJH_SHADER_PRINTF( "ERROR could not create vertex shader\n" );
            success = false;
        }

        // Shader strings have to be converted to const GLchar* so OpenGL can compile them
        source_ptr = (const GLchar*)vertex_source_.c_str();
        
        glShaderSource( vertex_shader_, 1, &source_ptr, NULL );
        glCompileShader( vertex_shader_ );

        if( did_shader_compile_ok( vertex_shader_ ) )
            vertex_source_.resize(0);
        else
            success = false;
    }
    else
    {
        TJH_SHADER_PRINTF( "ERROR vertex shader source was empty! (not set)\n" );
    }

    if( !fragment_source_.empty() )
    {
        // Create the fragment shader
        fragment_shader_ = glCreateShader( GL_FRAGMENT_SHADER );
        if( fragment_shader_ == 0 )
        {
            TJH_SHADER_PRINTF( "ERROR could not create fragment shader\n" );
            success = false;
        }

        // Shader strings have to be converted to const GLchar* so OpenGL can compile them
        source_ptr = (const GLchar*)fragment_source_.c_str();
        
        glShaderSource( fragment_shader_, 1, &source_ptr , NULL );
        glCompileShader( fragment_shader_ );

        if( did_shader_compile_ok( fragment_shader_ ) )
            fragment_source_.resize(0);
        else
            success = false;
    }
    else
    {
        TJH_SHADER_PRINTF( "ERROR fragment shader source was empty! (not set)\n" );
    }

    if( success ) {
         // Create the shader program, attach the vertex and fragment shaders
        program_ = glCreateProgram();
        glAttachShader( program_, vertex_shader_ );
        glAttachShader( program_, fragment_shader_ );
        glBindFragDataLocation( program_, 0, "outDiffuse" );
        glLinkProgram( program_ );

        // now cleanup the shaders
        // TODO: it's possible we want to the same vertex/fagment/etc. shader multple times in different programs
        // TODO: find a way of supporting this without the caller having to do extra work, on the other hand it might not be worth it
        glDetachShader( program_, vertex_shader_ );
        glDetachShader( program_, fragment_shader_ );
        glDeleteShader( vertex_shader_ );
        glDeleteShader( fragment_shader_ ); 
    }

    // Return true on success, false on error
    return success;
}

void TJH_SHADER_TYPENAME::shutdown()
{
    glDeleteProgram( program_ );
    glDeleteShader( vertex_shader_ );
    glDeleteShader( fragment_shader_ );
}

void TJH_SHADER_TYPENAME::bind()
{
    glUseProgram( program_ );
}

GLint TJH_SHADER_TYPENAME::getUniformLocation( const GLchar* name )
{
    GLint uniform = glGetUniformLocation( program_, name );
    if( uniform == -1 ) {
        TJH_SHADER_PRINTF("ERROR did not find uniform '%s' in shader program '%i'", name, program_ );
    }
    return uniform;
}


GLint TJH_SHADER_TYPENAME::getAttribLocation( const GLchar* name )
{
    GLint attribute = glGetAttribLocation( program_, name );
    if( attribute == -1 ) {
        TJH_SHADER_PRINTF("ERROR did not find attribute '%s' in shader program '%i'", name, program_ );
    }
    return attribute;
}

bool TJH_SHADER_TYPENAME::loadVertexSourceFile( std::string file_path )
{
    return load_file( file_path, &vertex_source_ );
}

bool TJH_SHADER_TYPENAME::loadFragmentSourceFile( std::string file_path )
{
    return load_file( file_path, &fragment_source_ );
}

// TODO: convert this to SDL rw ops and move to seperate header file
bool TJH_SHADER_TYPENAME::load_file( std::string filename, std::string* file_contents )
{
    // Use SDL to get the base resource path the first time only;
    if( shader_base_path_ == "" )
    {
        get_shader_base_path();
    }
    
    std::ifstream file( shader_base_path_ + filename );
    if( !file.good() )
    {
        TJH_SHADER_PRINTF("Could not load  %s!", filename.c_str() );
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    *file_contents = buffer.str();

    return true;
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
        TJH_SHADER_PRINTF( "ERROR: compiling shader..." );
        TJH_SHADER_PRINTF( "%s", buffer );
        return false;
    }

    return true;
}

void TJH_SHADER_TYPENAME::get_shader_base_path()
{
    // Use SDL to get the base path of the exe
    char* charPath = SDL_GetBasePath();

    // Add the shader folder to the project base path and a seperator on the end
    shader_base_path_ = std::string(charPath) + SHADER_FOLDER + PATH_SEPERATOR;

    // Free the SDL allocated string
    SDL_free(charPath);
}

// Prevent the macros from leaking into the global namespace
#undef SHADER_FOLDER
#undef PATH_SEPERATOR

#endif