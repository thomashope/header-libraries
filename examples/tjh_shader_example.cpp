#define TJH_WINDOW_IMPLEMENTATION
#include "../tjh_window.h"

// Define TJH_SHADER_IMPLEMENTATION in ONE .cpp file in your project
#define TJH_SHADER_IMPLEMENTATION
#include "../tjh_shader.h"

int main(int argc, char const *argv[])
{
	Window::init( "my window", 640, 480 );
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

	Shader shader;
	GLuint vao = 0;

	{
		shader.loadVertexSourceFile( "examples/tjh_shader_example_vert.glsl" );
		shader.loadGeometrySourceFile( "examples/tjh_shader_example_geo.glsl" );
		shader.loadFragmentSourceFile( "examples/tjh_shader_example_frag.glsl" );
		if( !shader.init() )
        {
            printf("oh no! Error creating shader!\n" );
        }

		glGenVertexArrays( 1, &vao );
        glBindVertexArray( vao );
        GLuint vbo = 0;
        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );

        GLfloat verts[] = {
        	-0.5, -0.5, 0, 1, 0, 1,
        	 0.5, -0.5, 0, 1, 1, 1,
        	 0.5,  0.5, 0, 0, 0, 1,
        	-0.5,  0.5, 0, 1, 1, 0
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        shader.setVertexAttribArrays({
            {"pos", 3, GL_FLOAT, GL_FALSE},
            {"col", 3, GL_FLOAT, GL_FALSE} });
	}

	SDL_Event event;
	bool done = false;
	while( !done )
	{
		while( SDL_PollEvent(&event) )
		{
			if( event.type == SDL_QUIT ) done = true;
            if( event.type == SDL_KEYDOWN )
            {
                if( event.key.keysym.scancode == SDL_SCANCODE_Q )
                {
                    done = true;
                }
                else if( event.key.keysym.scancode == SDL_SCANCODE_GRAVE )
                {
                    shader.reload();
                }
            }
		}

		glClearColor(0,0,0,0);
		glClear( GL_COLOR_BUFFER_BIT );

		shader.bind();
		glBindVertexArray( vao );
		glDrawArrays( GL_POINTS, 0, 4 );

		Window::present();
	}

	Window::shutdown();
	return 0;
}
