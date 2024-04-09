#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <thread>

#include <iostream>

#include "shader.h"
#include "stb_image.h"



using std::printf;


const char* vertexSource = R"glsl(
    #version 330 core
    
    in vec3 position;    
    in vec3 color;    

    uniform vec4 something_fun;

    out vec3 our_color;
    
    void main()
    {
        gl_Position = vec4(
            position.x, 
            position.y, 
            position.z, 
            1.0);
        our_color = color;
    }
)glsl";
const char* fragmentSource = R"glsl(
    #version 330 core
    
    in vec3 our_color;

    uniform vec4 something_fun;
    
    out vec4 out_color;
    
    void main()
    {
        out_color = vec4(
            our_color.x,
            our_color.y,
            our_color.z, 
            1.0); 
    }
)glsl";


int main(int argc, char* argv[])
{
    char buffer[512];
    GLFWwindow* window;
    GLuint vertexBuffer;
    GLuint vbo, vao, ebo;
    GLuint tex;
    GLuint something_fun;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint status;
    GLuint shaderProgram;
    GLint posAttrib;
    GLint colAttrib;
    GLint width, height, nrChannels;
    unsigned char* data;
    unsigned int texture;
    GLfloat vertices[] =
    {   // positions        // colors           // texture coords
        0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
    };
    GLuint indices[] =
    {
        0, 1, 3,
        1, 2, 3
    };
    GLfloat texCoords[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f
    };
    GLfloat borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };

    glfwInit();

    // define the glfw version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    Shader ourShader("shader.0.1.vs", "shader.0.1.fs");

    // build objects

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // create a vertex buffer object and copy the vertex data to it
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // build functionality into the shader class
//    posAttrib = glGetAttribLocation(ourShader.getID(), "position");
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
        0);

//    colAttrib = glGetAttribLocation(ourShader.getID(), "color");
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
        (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
        (void*)(6 * sizeof(float)));

    // mipmap filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
        GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
        GL_LINEAR);

    // generate the texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
//    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // filtering methods (what pixel color to choose)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
        GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load texture
    data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
            GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    while(!glfwWindowShouldClose(window))
    {
        float time = glfwGetTime();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    
        ourShader.use();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);

    glfwTerminate();

    return 0;
}

/*
    Terms
    ----------
        Uniforms: global attributes that allow dynamic changes to the triangle
        from the glsl program.
*/