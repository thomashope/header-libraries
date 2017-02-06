// Define TJH_WINDOW_IMPLEMENTATION in *ONE FILE ONLY*
// This relies on GLEW and SDL2
// To compile on mac, install SDL2 and glew using homebrew
// then link during compilation with -lSDL2 -lglew -framework OpenGL

////// LIBRARY OPTIONS ////////////////////////////////////////////////////////

// Set these to the location of your libray headers!
#define TJH_WINDOW_SDL_H_LOCATION <SDL2/SDL.h>
#define TJH_WINDOW_GLEW_H_LOCATION <GL/glew.h>

// Options for your convinience!
#define TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL
#define TJH_WINDOW_AUTO_INIT_GLEW
#define TJH_WINDOW_INCLUDE_SDL
#define TJH_WINDOW_INCLUDE_GLEW

// Optional header includes
#ifdef TJH_WINDOW_INCLUDE_SDL
	#include TJH_WINDOW_SDL_H_LOCATION
#endif
#ifdef TJH_WINDOW_INCLUDE_GLEW
	#include TJH_WINDOW_GLEW_H_LOCATION
#endif


////// HEADER /////////////////////////////////////////////////////////////////
namespace Window
{
	// TODO: return an error flag or something?
	void init(const char* title, int width, int height);
	void shutdown();
	void present();
	void getSize( int* width, int* height );
	SDL_Window* getSDLWindow();
	SDL_GLContext getSDLGLContext();

	// PUBLIC MEMBERS
	extern SDL_Window* sdl_window_;
	extern SDL_GLContext sdl_gl_context_;
}

/*
class Window
{
public:

	static bool init( const char* title, int width, int height );
	static void shutdown();
	static void present() { SDL_GL_SwapWindow( sdl_window_ ); }

	static Window* get() { return self_; }

	static void getSize( int* width, int* height ) { SDL_GetWindowSize( sdl_window_, width, height ); }
	static SDL_Window* getSDLWindow() { return sdl_window_; }
	static SDL_GLContext getSDLGLContext() { return sdlgl_context_; }

protected:
	Window() {}
	~Window() {}

	static Window* self_;

	static SDL_Window* sdl_window_;
	static SDL_GLContext sdlgl_context_;
};*/

////// IMPLEMENTATION /////////////////////////////////////////////////////////
#ifdef TJH_WINDOW_IMPLEMENTATION

namespace Window
{
	void init(const char* title, int width, int height)
	{
#ifdef TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL
		SDL_Init(SDL_INIT_EVERYTHING);
#endif // TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL

	    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		sdl_window_ = SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL );
		sdl_gl_context_ = SDL_GL_CreateContext( sdl_window_ );

#ifdef TJH_WINDOW_AUTO_INIT_GLEW
		glewExperimental = GL_TRUE;
		GLenum error = glewInit();
		if( error != GLEW_OK ) printf("ERROR: starting glew %d", error);
#endif // TJH_WINDOW_AUTO_INIT_GLEW
	}

	void shutdown()
	{
		SDL_GL_DeleteContext( sdl_gl_context_ );
		sdl_gl_context_ = NULL;

		SDL_DestroyWindow( sdl_window_ );
		sdl_window_ = NULL;

#ifdef TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL
		SDL_Quit();
#endif // TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL
	}

	void present()
	{
		SDL_GL_SwapWindow( sdl_window_ );
	}

	void getSize( int* width, int* height )
	{
		SDL_GetWindowSize( sdl_window_, width, height );
	}

	SDL_Window* getSDLWindow()
	{
		return sdl_window_;
	}

	SDL_GLContext getSDLGLContext()
	{
		return sdl_gl_context_;
	}

	// MEMBERS
	SDL_Window* sdl_window_ = NULL;
	SDL_GLContext sdl_gl_context_ = NULL;
}
/*
// Declare static variables
Window* Window::self_ = NULL;
SDL_Window* Window::sdl_window_ = NULL;
SDL_GLContext Window::sdlgl_context_ = NULL;

bool Window::init( const char* title, int width, int height )
{
	if( self_ == NULL )
	{

#ifdef TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL
		SDL_Init(SDL_INIT_EVERYTHING);
#endif // TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL

	    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		self_ = new Window();
		sdl_window_ = SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL );
		sdlgl_context_ = SDL_GL_CreateContext( sdl_window_ );

#ifdef TJH_WINDOW_AUTO_INIT_GLEW
		glewExperimental = GL_TRUE;
		GLenum error = glewInit();
		if( error != GLEW_OK ) printf("ERROR: starting glew %d", error);
#endif // TJH_WINDOW_AUTO_INIT_GLEW
	}

	// Everything if fine
	return true;
}

void Window::shutdown()
{
	if( self_ )
	{
		delete self_;
		self_ = NULL;

		SDL_GL_DeleteContext( sdlgl_context_ );
		sdlgl_context_ = NULL;

		SDL_DestroyWindow( sdl_window_ );
		sdl_window_ = NULL;

#ifdef TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL
		SDL_Quit();
#endif // TJH_WINDOW_AUTO_INIT_AND_SHUTDOWN_SDL
	}
}*/

#endif // TJH_WINDOW_IMPLEMENTATION