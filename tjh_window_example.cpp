#define TJH_WINDOW_IMPLEMENTATION
#include "window.h"

int main(int argc, char const *argv[])
{
	// Creates the window, optionaly inits SDL and GLEW
	Window::init( "Window Title!", 800, 600 );

	// Call to set vsync if you like, tries to use late swap tearing if possible
	Window::setVsync( true );

	bool running = true;
	while( running )
	{
		SDL_Event event;
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT ) running = false;
		}

		glClearColor( 0, 0, 0, 1 );
		glClear( GL_COLOR_BUFFER_BIT );


		// Display the contents of the window!
		Window::present();
	}

	Window::shutdown();
	return 0;
}