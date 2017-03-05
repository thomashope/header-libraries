#define TJH_SHADER_IMPLEMENTATION
#include "shader.h"

#define TJH_WINDOW_IMPLEMENTATION
#include "window.h"

int main(int argc, char const *argv[])
{
	Window::init( "my window", 640, 480 );
	//Draw::init();
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

	Shader shader;
	GLuint vao = 0;

	{
		// A handy macro to help you define glsl directly in your source code
		// produces unhelpful error messages
		#define GLSL(src) "#version 330 core\n" #src

		const GLchar* vert_src = GLSL(
		    in vec3 pos;
		    in vec3 col;
		    out vec3 vCol;
		    void main()
		    {
		        gl_Position = vec4(pos, 1.0);
		        vCol = col;
		    }
		    );
		const GLchar* geo_src = GLSL(
		    layout(points) in;
		    layout(line_strip, max_vertices = 11) out;
		    in vec3 vCol[];
		    out vec3 gCol;

		    const float PI = 3.1415926;

		    void main()
		    {

		    gCol = vCol[0];

			    for (int i = 0; i <= 10; i++) {
			        float ang = PI * 2.0 / 10.0 * i;

			        vec4 offset = vec4(cos(ang) * 0.3, -sin(ang) * 0.4, 0.0, 0.0);
			        gl_Position = gl_in[0].gl_Position + offset;

			        EmitVertex();
			    }

		        EndPrimitive();
		    }
		    );
		const GLchar* frag_src = GLSL(
		    in vec3 gCol;
		    out vec4 outDiffuse;
		    void main()
		    {
		       outDiffuse = vec4(gCol,1);
		    }
		    );

		shader.setVertexSourceString( std::string(vert_src) );
		shader.setFragmentSourceString( std::string(frag_src) );
		shader.setGeometrySourceString( std::string(geo_src) );
		shader.init();
		shader.bind();

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

        GLint posAttrib = shader.getAttribLocation( "pos" );
        glEnableVertexAttribArray( posAttrib );
    	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), 0 );
        GLint colAttrib = shader.getAttribLocation( "col" );
        glEnableVertexAttribArray( colAttrib );
    	glVertexAttribPointer( colAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)) );

	}

	SDL_Event event;
	bool done = false;
	while( !done )
	{
		while( SDL_PollEvent(&event) )
		{
			if( event.type == SDL_QUIT ) done = true;
		}

		glClearColor(0,0,0,0);
		glClear( GL_COLOR_BUFFER_BIT );

		shader.bind();
		glBindVertexArray( vao );
		glDrawArrays( GL_POINTS, 0, 4 );

		Window::present();
	}

	//Draw::shutdown();
	Window::shutdown();
	return 0;
}