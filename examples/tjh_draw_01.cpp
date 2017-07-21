#define TJH_WINDOW_NAMESPACE window
#define TJH_WINDOW_IMPLEMENTATION
#include "../tjh_window.h"

#define TJH_DRAW_NAMESPACE draw
#define TJH_DRAW_IMPLEMENTATION
#include "../tjh_draw.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

const int WIDTH = 800;
const int HEIGHT = 600;
const int FRAMEBUFFER_WIDTH = 800;	// TODO: set these differently and make sure stuff still works
const int FRAMEBUFFER_HEIGHT = 600;
bool done = false;

void createFramebuffer( GLuint &framebuffer, GLuint &texture );

int main()
{
	window::init( __FILE__, WIDTH, HEIGHT );
	draw::init( WIDTH, HEIGHT );

	GLuint framebuffer, texture;
	createFramebuffer( framebuffer, texture );

	SDL_Event event;
	while( !done )
	{
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT ) done = true;
		}

		// Bind the framebuffer we want to draw to

		glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );

		// Do our drawing to the framebuffer

		draw::clear( 0, 0.5, 0.5 );

		draw::setColor( 1, 1, 1 );
		draw::quad( 0, 0, 50, 50 );

		draw::setColor( 1, 0, 0 );
		draw::quad( WIDTH, 0, -50, 50 );

		draw::setColor( 0, 1, 0 );
		draw::quad( 0, HEIGHT, 50, -50 );

		draw::setColor( 0, 0, 1 );
		draw::quad( WIDTH, HEIGHT, -50, -50 );

		draw::flush();

		// Now bind to the default framebuffer (the window)

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		// Grab the colour texture from the framebuffer and draw it to a fullscreen quad

		glBindTexture( GL_TEXTURE_2D, texture );
		
		draw::clear( 0.1, 0.1, 0.1 );
		draw::setColor( 1, 1, 1 );
		for( int i = 5; i < WIDTH; i += 10 )
		{
			draw::point( i, 10 );
			draw::point( i, HEIGHT - 10 );
		}
		draw::texturedQuad( 10, 10, WIDTH/5.0f, HEIGHT/5.0f );

		draw::setLineWidth( 2 );
		draw::setColor( 1, 0, 0 );
		draw::line( WIDTH/2, HEIGHT/2, WIDTH/2 + 50, HEIGHT/2 - 50 );
		draw::setColor( 0, 1, 0 );
		draw::line( WIDTH/2, HEIGHT/2, WIDTH/2 - 50, HEIGHT/2 + 50 );

		draw::flush();
		
		glBindTexture( GL_TEXTURE_2D, 0 );

		window::present();
	}

	// Cleanup resources
	if( framebuffer )
	{
		glDeleteFramebuffers( 1, &framebuffer );
		framebuffer = 0;
	}

	draw::shutdown();
	window::shutdown();

	return 0;
}

void createFramebuffer( GLuint &framebuffer, GLuint &texture )
{
	// Create a texture and fill with data
	
	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL ); 
	glBindTexture( GL_TEXTURE_2D, 0 );

	// Create the framebuffer and bind it ready for assembly
	
	glGenFramebuffers( 1, &framebuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );

	// Attach the texture to the framebuffer

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,			// The target framebuffer
		GL_COLOR_ATTACHMENT0,	// The slot in the buffer to attach to
		GL_TEXTURE_2D,			// The type of texture which will be attached
		texture,				// The texture to attach
		0 );					// Mipmap level

	// Check everything went ok
	
	if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
	{
		// Something went wrong when trying to create the framebuffer
		printf("ERROR: Something went wrong creating the framebuffer...\n");
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}