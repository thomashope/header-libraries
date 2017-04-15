#define TJH_WINDOW_IMPLEMENTATION
#include "../tjh_window.h"

// Define TJH_DRAW_IMPLEMENTATION in ONE .cpp file in your project
#define TJH_DRAW_IMPLEMENTATION
#include "../tjh_draw.h"

int main(int argc, char const *argv[])
{
	Window::init( "Window Title!", 800, 600 );
	Window::setVsync( true );

	Draw::init();

	bool running = true;
	while( running )
	{
		SDL_Event event;
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT ) running = false;
		}

		Draw::clear( 0, 0, 0 );

		// You should call Begin before you do any actual drawing
		Draw::begin();
		{
//			Draw::setColor( 1, 0, 1 );
//			Draw::line( -0.5, -0.5, 0.5, 0.5 );
//
//			Draw::setColor( 1, 1, 1 );
//			Draw::circle( 0, 0, 0.3 );

            Draw::setColor( 0, 1, 1 );
            Draw::quad( 0.5, 0.5, 0.6, 0.4 );
		}
		Draw::end();

		Window::present();
	}

	Draw::shutdown();
	Window::shutdown();
	return 0;
}
