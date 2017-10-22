#include "../tjh_draw.h"

const int WIDTH = 640;
const int HEIGHT = 480;

float clamp( float v, float min, float max )
{
	float vmin = min < max ? min : max;
	float vmax = max > min ? max : min;
	if( v < vmin ) return vmin;
	if( v > vmax ) return vmax;
	return v;
}

int main()
{
	draw::init( __FILE__, 640, 480 );

	float x = 100;
	float y = 100;
	float xSpeed = 2;
	float ySpeed = 2;
	float size = 50;

	bool done = false;
	SDL_Event event;
	while( !done )
	{
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT ) done = true;
		}
		draw::clear(0.1, 0.1, 0.1);

		draw::setColor( 0.5, 0.5, 0.5 );
		draw::rect( x, y, size, size );

		x += xSpeed;
		float newx = clamp(x, 0, WIDTH - size);
		if( newx != x )
		{
			x = newx;
			xSpeed *= -1;
		}

		y += ySpeed;
		float newy = clamp(y, 0, HEIGHT - size);
		if( newy != y )
		{
			y = newy;
			ySpeed *= -1;
		}

		draw::flush();
		draw::present();
	}

	draw::shutdown();
	return 0;
}

#define TJH_DRAW_IMPLEMENTATION
#include "../tjh_draw.h"