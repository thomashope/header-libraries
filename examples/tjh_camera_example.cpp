#define TJH_WINDOW_IMPLEMENTATION
#include "../tjh_window.h"

#define TJH_SHADER_IMPLEMENTATION
#include "../tjh_shader.h"

// Define TJH_CAMERA_IMPLEMENTATION in ONE .cpp file in your project
#define TJH_CAMERA_IMPLEMENTATION
#include "../tjh_camera.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;
const float SPEED = 10.0f;

int main()
{
    Window::init("tjh_camera example", WIDTH, HEIGHT);

    #define GLSL(src) "#version 330 core\n" #src

    const GLchar* vert_src = GLSL(
        uniform mat4 view;
        uniform mat4 projection;
        in vec3 pos;
        in vec3 col;
        out vec3 vCol;
        void main()
        {
            gl_Position = projection * view * vec4(pos, 1.0);
            vCol = col;
        }
        );
    const GLchar* frag_src = GLSL(
        in vec3 vCol;
        out vec4 outDiffuse;
        void main()
        {
           outDiffuse = vec4(vCol,1);
        }
        );

    // Setup the shader
    Shader shader;
    shader.setVertexSourceString( std::string(vert_src) );
    shader.setFragmentSourceString( std::string(frag_src) );
    if( !shader.compileAndLink() )
    {
        std::cout << "ERROR: failed to init shader!" << std::endl;
    }

    // Setup the scene
    GLint view_uniform = shader.getUniformLocation( "view" );
    GLint proj_uniform = shader.getUniformLocation( "projection" );
    GLuint vao = 0, vbo = 0;

    glGenVertexArrays( 1, &vao );
    glGenBuffers( 1, &vbo );
    glBindVertexArray( vao );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );

    GLfloat verts[] = {
        1, 0, 1,    1, 1, 1,
        -1, 0, 1,   1, 1, 1,
        0, 0, -1,   1, 0, 1
    };

    glBufferData( GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW );

    shader.setVertexAttribArrays({
        {"pos", 3, GL_FLOAT, GL_FALSE},
        {"col", 3, GL_FLOAT, GL_FALSE}
    });

    Camera camera;

    bool running = true;
    Uint32 curr_ticks = SDL_GetTicks();
    Uint32 prev_ticks = curr_ticks;
    float dt = (curr_ticks - prev_ticks) / 1000.0f;
    const Uint8* keyboard = SDL_GetKeyboardState( NULL );

    while( running )
    {
        SDL_Event event;
        while( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT ) running = false;
            else if( event.type == SDL_KEYDOWN )
            {
                switch( event.key.keysym.scancode )
                {
                case SDL_SCANCODE_ESCAPE:
                    running = false;
                break;
                default: break;
                }
            }
        }

        // Update dt
        prev_ticks = curr_ticks;
        curr_ticks = SDL_GetTicks();
        dt = (curr_ticks - prev_ticks) / 1000.0f;

        // Update keyboard
        keyboard = SDL_GetKeyboardState( NULL );

        // Move camera
        if( keyboard[SDL_SCANCODE_E] ) {
            camera.moveUp( SPEED * dt );
        }
        if( keyboard[SDL_SCANCODE_Q] ) {
            camera.moveDown( SPEED * dt );
        }
        if( keyboard[SDL_SCANCODE_W] ) {
            camera.moveForward( SPEED * dt );
        }
        if( keyboard[SDL_SCANCODE_S] ) {
            camera.moveBack( SPEED * dt );
        }
        if( keyboard[SDL_SCANCODE_A] ) {
            camera.moveLeft( SPEED * dt );
        }
        if( keyboard[SDL_SCANCODE_D] ) {
            camera.moveRight( SPEED * dt );
        }

        // Rotate camera
        if( keyboard[SDL_SCANCODE_LEFT] ) {
            camera.rotateLeft( 2.0f * dt );
        }
        if( keyboard[SDL_SCANCODE_RIGHT] ) {
            camera.rotateRight( 2.0f * dt );
        }
        if( keyboard[SDL_SCANCODE_UP] ) {
            camera.rotateUp( 2.0f * dt );
        }
        if( keyboard[SDL_SCANCODE_DOWN] ) {
            camera.rotateDown( 2.0f * dt );
        }

        // Clear the screen
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Draw the scene
        shader.bind();

        glUniformMatrix4fv( view_uniform, 1, GL_FALSE, glm::value_ptr( camera.view() ) );
        glUniformMatrix4fv( proj_uniform, 1, GL_FALSE, glm::value_ptr( camera.projection( WIDTH, HEIGHT ) ) );

        glBindVertexArray( vao );
        glDrawArrays( GL_TRIANGLES, 0, 3 );

        // Display graphics
        Window::present();
    }

    shader.shutdown();
    Window::shutdown();
	return 0;
}
