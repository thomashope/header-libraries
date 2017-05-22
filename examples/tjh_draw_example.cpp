#define TJH_WINDOW_IMPLEMENTATION
#include "../tjh_window.h"

// Define TJH_DRAW_IMPLEMENTATION in ONE .cpp file in your project
#define TJH_DRAW_IMPLEMENTATION
#include "../tjh_draw.h"

const int WIDTH = 800;
const int HEIGHT = 600;
bool multisample = true;

int main(int argc, char const *argv[])
{
	Window::init( "Window Title!", WIDTH, HEIGHT );

	Draw::init( 320, 240 );

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

		Draw::clear( 0, 0, 0 );

        // Give us some corners so we know whats going down
        Draw::setColor( 0, 0, 1 );
        Draw::quad( 0, 0, 8, 8 );
        Draw::setColor( 1, 0, 1 );
        Draw::quad( 312, 0, 8, 8 );
        Draw::setColor( 0, 1, 1 );
        Draw::quad( 0, 232, 8, 38 );
        Draw::setColor( 1, 1, 1 );
        Draw::quad( 312, 232, 8, 8 );

        // Draw a bunch of points
        // Multisampling will make points less than perfect
        for( int i = 100; i < 200; i += 2 ) {
            Draw::point( i, 100 );
        }

        Draw::setColor( 0.5, 0.5, 0.5 );
        Draw::circle( 200, 200, 32, 20 );

        // Ensure all the graphics is actually drawn
		Draw::flush();

		Window::present();
	}

	Draw::shutdown();
	Window::shutdown();
	return 0;
}
