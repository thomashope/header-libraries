#define TJH_WINDOW_IMPLEMENTATION
#include "../tjh_window.h"
#define TJH_CAMERA_IMPLEMENTATION
#include "../tjh_camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#include <glm/gtc/type_ptr.hpp>

// Define TJH_DRAW_IMPLEMENTATION in ONE .cpp file in your project
#define TJH_DRAW_IMPLEMENTATION
#include "../tjh_draw.h"

const int WIDTH = 800;
const int HEIGHT = 600;
const int SPEED = 20;
bool multisample = true;

Uint32 ticks = 0;
Uint32 prev_ticks = 0;

Camera camera;

void update_camera( float dt );

int main()
{
	Window::init( "Window Title!", WIDTH, HEIGHT );

	Draw::init( 320, 240 );

    GLuint tex;
    glGenTextures( 1, &tex );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tex );
    int x,y,n;
    unsigned char *pixels = stbi_load("sample.png", &x, &y, &n, 0);
    stbi_image_free(pixels);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels ); 
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glGenerateMipmap( GL_TEXTURE_2D );

	bool running = true;
	while( running )
	{
		SDL_Event event;
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT ) running = false;
            if( event.type == SDL_KEYDOWN ) {
                // Toggle multisampling with the space bar
                if( event.key.keysym.scancode == SDL_SCANCODE_SPACE ) {
                    multisample = !multisample;
                    // Random forum posts suggest this doesn't work at runtime on all hardware
                    // Appears to work on my MBP with AMD Radeon R9 M370X 2048 MB/Intel Iris Pro 1536 MB
                    if( multisample ) glEnable(GL_MULTISAMPLE);
                    else glDisable(GL_MULTISAMPLE);
                }
            }
		}

        prev_ticks = ticks;
        ticks = SDL_GetTicks();
        float dt = (ticks - prev_ticks)/1000.0f;

        update_camera( dt );

		Draw::clear( 0, 0, 0 );

        // Give us some corners so we know whats going down
        Draw::setColor( 0, 0, 1 );
        Draw::quad( 0, 0, 8, 8 );
        Draw::setColor( 1, 0, 1 );
        Draw::quad( 312, 0, 8, 8 );
        Draw::setColor( 0, 1, 1 );
        Draw::quad( 0, 232, 8, 38 );
        Draw::setColor( 0.5, 0.5, 0.5 );
        Draw::circle( 312, 232, 8, 8 );

        // Draw a bunch of points
        // Multisampling will make points less than perfect
        for( int i = 100; i < 200; i += 2 ) {
            Draw::point( i, 220 );
        }

        // Try a textured shape
        
        glBindTexture( GL_TEXTURE_2D, tex );
        Draw::texturedQuad( 20, 20, 100, 100 );

        Draw::texturedQuad( 250, 20, 50, 50, 0.2, 0.2, 0.6, 0.6 );

        Draw::texturedTriangle( 200, 20, 240, 20, 200, 60, 0, 0, 1, 0, 0, 1 );

        // Try a 3D shape

        glm::mat4 mvp = camera.projection( WIDTH, HEIGHT ) * camera.view();
        Draw::setMVPMatrix( glm::value_ptr( mvp[0] ) );
        
        Draw::setColor( 0.6, 0.6, 0.6 );
        Draw::quad( 1, 0, 1, 1, 0, -1, -1, 0, -1, -1, 0, 1 );
        Draw::setColor( 0.4, 0.4, 0.4 );
        Draw::triangle( 0, 0, 0, 0, 1, 0, 1, 0, 0 );

        // Ensure all the graphics is actually drawn
		Draw::flush();

		Window::present();
	}

	Draw::shutdown();
	Window::shutdown();
	return 0;
}

void update_camera( float dt )
{
    // Update keyboard
    const Uint8* keyboard = SDL_GetKeyboardState( NULL );

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
        camera.rotateLeft( 3.0f * dt );
    }
    if( keyboard[SDL_SCANCODE_RIGHT] ) {
        camera.rotateRight( 3.0f * dt );
    }
    if( keyboard[SDL_SCANCODE_UP] ) {
        camera.rotateUp( 3.0f * dt );
    }
    if( keyboard[SDL_SCANCODE_DOWN] ) {
        camera.rotateDown( 3.0f * dt );
    }
}
