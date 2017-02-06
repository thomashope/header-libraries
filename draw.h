
// Get someone else to create the gl context

namespace Draw
{
	////// BASIC FUNCTIONS ////////////////////////////////////////////////////
	
	void init();		// TODO: return and error flag or something?
	void shutdown();
	void begin();
	void end();

	////// SET STATE //////////////////////////////////////////////////////////

	// TODO: void setResolution( int width, int height );
	//		This would scale the 
	void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f);

	////// PRIMATIVES /////////////////////////////////////////////////////////

	void line( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
	void quad( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
	void point( GLfloat x, GLfloat y );
}

#ifdef TJH_DRAW_IMPLEMENTATION
	
namespace Draw
{
	GLuint vao_ = 0;
	GLuint vbo_ = 0;
	GLuint shader_program_ = 0;
	GLint colour_uniform_ = -1;

	void init()
	{
		const GLchar* vert_src =
		    "#version 150 core\n"
		    "in vec2 vPos;"
		    "void main()"
		    "{"
		    "    gl_Position = vec4(vPos, 0.0, 1.0);"
		    "}";
		const GLchar* frag_src =
			"#version 150 core\n"
			"uniform vec4 colour;"
			"out vec4 outColour;"
			"void main()"
			"{"
			"    outColour = colour;"
			"}";
		GLuint vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
		if( vertex_shader_ == 0 || fragment_shader_ == 0 ) printf("ERROR: cold not init shaders!");

		// Compile and check the vertex shader
		glShaderSource( vertex_shader_, 1, &vert_src, NULL );
		glCompileShader( vertex_shader_ );
		GLint status = GL_TRUE;
		glGetShaderiv( vertex_shader_, GL_COMPILE_STATUS, &status );
		if( status != GL_TRUE )
		{
			printf("ERROR: could not compile vertex shader\n");
	        // Get the length of the error log
	        GLint log_length = 0;
	        glGetShaderiv(vertex_shader_, GL_INFO_LOG_LENGTH, &log_length);

	        // Now get the error log itself
	        GLchar buffer[log_length];
	        glGetShaderInfoLog( vertex_shader_, log_length, NULL, buffer );
			printf("%s\n", buffer);
		}

		// Compile and check the fragment shader
		glShaderSource( fragment_shader_, 1, &frag_src, NULL );
		glCompileShader( fragment_shader_ );
		status = GL_TRUE;
		glGetShaderiv( fragment_shader_, GL_COMPILE_STATUS, &status );
		if( status != GL_TRUE )
		{
			printf("ERROR: could not compile fragment shader\n");
	        // Get the length of the error log
	        GLint log_length = 0;
	        glGetShaderiv(fragment_shader_, GL_INFO_LOG_LENGTH, &log_length);

	        // Now get the error log itself
	        GLchar buffer[log_length];
	        glGetShaderInfoLog( fragment_shader_, log_length, NULL, buffer );
			printf("%s\n", buffer);
		}

		shader_program_ = glCreateProgram();
		glAttachShader( shader_program_, vertex_shader_ );
		glAttachShader( shader_program_, fragment_shader_ );
		glBindFragDataLocation( shader_program_, 0, "outColour");
		glLinkProgram( shader_program_ );
		glUseProgram( shader_program_ );
		glDeleteShader(vertex_shader_);
		glDeleteShader(fragment_shader_);

		colour_uniform_ = glGetUniformLocation(shader_program_, "colour");

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);
		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		GLint posAtrib = glGetAttribLocation(shader_program_, "vPos");
		if( posAtrib == -1 ) printf("ERROR: position attribute not found in shader\n");
		glEnableVertexAttribArray(posAtrib);
		glVertexAttribPointer(posAtrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}
	void shutdown()
	{
		glDeleteProgram(shader_program_);
	}
	void begin()
	{
		// store previous values and set stuff, see dear imgui
		glUseProgram(shader_program_);
	}
	void end()
	{
		// TOOD: This should restore the previous state instead
		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a )
	{
		glUniform4f( colour_uniform_, r, g, b, a );
	}

	void line( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
	{
		glBindVertexArray(vao_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		GLfloat verts[] = {
			x1, y1, x2, y2
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
		glDrawArrays(GL_LINES, 0, 2);
	}
	void quad( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
	{
		glBindVertexArray(vao_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		GLfloat verts[] = {
			x1, y1, x2, y1, x2, y2,
			x1, y1, x2, y2, x1, y2
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	void point( GLfloat x, GLfloat y )
	{
		glBindVertexArray(vao_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		GLfloat verts[] = { x, y };

		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
		glDrawArrays(GL_POINTS, 0, 2);
	}
}

#endif