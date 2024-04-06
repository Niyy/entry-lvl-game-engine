#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <thread>

#include <iostream>


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
    GLuint vbo;
    GLuint vao;
    GLuint ebo;
    GLuint tex;
    GLuint something_fun;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint status;
    GLuint shaderProgram;
    GLint posAttrib;
    GLint colAttrib;
    GLfloat vertices[] =
    {
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f//,
//        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f
    };
    GLuint elements[] =
    {
        0, 1, 2,
        2, 3, 0
    };
    float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    float pixels[] =
    {
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };

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

    // create and compile the vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

    if (!status)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        printf("VERTEX_SHADER::ERROR");
        printf(buffer);
    }
    
    // create and compile the fragment shader
    status = 0;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);

    if (!status)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
        printf("ERROR::FRAGMENT_SHADER::LINKING_FAILED");
        printf(buffer);
    }

    // link the vertex and fragment shader into a shader program
    status = 0;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);

    if(!status)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, buffer);
        printf("ERROR::SHADER_PROGRAM::LINK_FAILED");
    }

    // build objects

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // create a vertex buffer object and copy the vertex data to it
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//    glGenBuffers(1, &ebo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
//        GL_STATIC_DRAW);

    posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
        0);

    colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
        (void*)(3 * sizeof(float)));

    // both the colAttrib and posAttrib can be done with layouts and ints.

    something_fun = glGetUniformLocation(shaderProgram, "something_fun");

    glUseProgram(shaderProgram);

    while (!glfwWindowShouldClose(window))
    {
        float time = glfwGetTime();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glUniform4f(something_fun, sin(time) + 0.5f, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
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