#define TJH_WINDOW_IMPLEMENTATION
#include "../tjh_window.h"

#define TJH_SHADER_IMPLEMENTATION
#include "../tjh_shader.h"

#define TJH_TEXTURE_CACHE_IMPLEMENTATION
#include "../tjh_texture_cache.h"

#include "../stb_image.h"

const int WIDTH  = 800;
const int HEIGHT = 600;

int main(int argc, char const *argv[])
{
    Window::init( "textures", WIDTH, HEIGHT );

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLfloat vertices[] = {
    //  Position      Color             Texcoords
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create an element array
    GLuint ebo;
    glGenBuffers(1, &ebo);

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    Shader shader;
    shader.loadVertexSourceFile("tjh_texture_cache_example_vert.glsl");
    shader.loadFragmentSourceFile("tjh_texture_cache_example_frag.glsl");
    shader.compileAndLink();
    shader.bind();

    shader.setVertexAttribArrays({
        {"position", 2, GL_FLOAT, GL_FALSE},
        {"color",    3, GL_FLOAT, GL_FALSE},
        {"texcoord", 2, GL_FLOAT, GL_FALSE},
        });

    Texture::Handle tex = Texture::load("sample.png");
    tex.bind();

    SDL_Event event;
    bool done = false;
    while( !done )
    {
        while( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT ) done = true;
            if( event.type == SDL_KEYDOWN ) {
                if( event.key.keysym.sym == SDLK_q ) done = true;
            }
        }

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a rectangle from the 2 triangles using 6 indices
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        Window::present();
    }

    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    Window::shutdown();
    return 0;
}
