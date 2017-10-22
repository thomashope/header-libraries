#define TJH_DRAW_IMPLEMENTATION
#include "../tjh_draw.h"

const int WIDTH = 640;
const int HEIGHT = 480;

int main()
{
	draw::init( __FILE__, 640, 480 );

	SDL_Event event;
	bool done = false;
	while( !done )
	{
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT ) done = true;
			else if( event.type == SDL_KEYDOWN )
			{
				if( event.key.keysym.scancode == SDL_SCANCODE_ESCAPE ) done = true;
			}
		}

		draw::clear( 0.1, 0.1, 0.1 );

		draw::setColor( 0.9 );
		draw::texturedRect( 0, 0, 256, 256 );

		draw::text( "You exist,\nthat is enough.", WIDTH/2, 10 );

		draw::setColor( 0, 0.9, 1 );
		draw::text( "Sometimes text has color.\nThese are good times,\nremember them.", WIDTH/2, 60, 10 );

		draw::present();
	}

	draw::shutdown();
	return 0;
}