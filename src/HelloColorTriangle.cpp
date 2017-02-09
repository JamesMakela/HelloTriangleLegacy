//============================================================================
// Name        : HelloColorTriangle.cpp
// Author      : James L. Makela
// Version     : 0.1.1
// Copyright   : LGPL v3.0
// Description : OpenGL colored triangle in C++ 11 for Legacy OpenGL 2.1.
//               Another simple example of an OpenGL Program.
//               Here we expand upon the HelloTriangle by adding a variable
//               color gradient to it, sort of like a color select dialog
//               would have.
//
//               Techniques illustrated here are:
//               - In addition to a vertex array buffer, we will introduce
//                 a vertex color array buffer.  So each vertex will have
//                 an associated color.
//               - Our vertex shader will receive color information, so that
//                 it can pass it to the fragment shader.
//               - Our fragment shader will mix the vertex colors, producing
//                 an appropriate color for the pixel.
//
//               Note: We will try to be compatible with Mac OSX, as well as
//                     Linux, since those are the platforms at my disposal.
//
//============================================================================

#include <iostream>
using std::cout;
using std::cin;
using std::endl;

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// forward declarations defined after main()
// I like organizing my functions in a top-down fashion
void ConfigureGLFW();
void report_error(int code, const char * description);
void key_callback(GLFWwindow* window,
                  int key, int scancode, int action, int mode);

const GLchar *vertexShaderSource = "#if __VERSION__ >= 140\n"
                                   "    in vec3 position;\n"
                                   "    in vec3 vertex_color;\n"
                                   "    out vec3 color;\n"
                                   "#else\n"
                                   "    attribute vec3 position;\n"
                                   "    attribute vec3 vertex_color;\n"
                                   "    varying vec3 color;\n"
                                   "#endif\n"
                                   "\n"
                                   "void main()\n"
                                   "{\n"
                                   "    color = vertex_color;\n"
                                   "    gl_Position = vec4(position, 1.0);\n"
                                   "}\n";

const GLchar *fragmentShaderSource = "#if __VERSION__ >= 140\n"
                                     "    in vec3 color;\n"
                                     "    varying out vec4 out_color;\n"
                                     "#else\n"
                                     "    varying vec3 color;\n"
                                     "#endif\n"
                                     "\n"
                                     "void main()\n"
                                     "{\n"
                                     "#if __VERSION__ >= 140\n"
                                     "    out_color = vec4(color, 1.0f);\n"
                                     "#else\n"
                                     "    gl_FragColor = vec4(color, 1.0);\n"
                                     "#endif\n"
                                     "}\n";

int main() {
    if (!glfwInit()) {
        // Initialization failed
        cout << "GLFW Initialization Failed!!" << endl;
        return -1;
    }

    glfwSetErrorCallback(&report_error);

    ConfigureGLFW();


    GLFWwindow* window = glfwCreateWindow(800, 600,
                                          "OpenGL Color Triangle",
                                          nullptr, nullptr);

    if (window == nullptr) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Note: glewInit() will fail if it is attempted before the current
    //       window context is made via glfwMakeContextCurrent()
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cout << "Failed to initialize GLEW" << endl;
        return -1;
    }

    cout << "OpenGL version supported by this platform: "
         << glGetString(GL_VERSION) << endl;
    cout << "GLSL version supported by this platform: "
         << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    cout << "Set the Viewport..." << endl;

    glfwSetKeyCallback(window, key_callback);

    // Here is where we build and compile our shader program
    // setup a basic vertex shader
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
             << infoLog << endl;
    }

    // setup a basic fragment shader
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
             << infoLog << endl;
    }

    // attach our shaders to a shader program
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();

    // insert location binding code here
    // Note: using OGL 3.3 or higher, we could put location
    //       information inside the GLSL code.  But Macs use
    //       OGL 3.2 and Linux can vary widely.
    glBindAttribLocation(shaderProgram, 0, "position");
    glBindAttribLocation(shaderProgram, 1, "color");

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
             << infoLog << endl;
    }

    // once we have compiled our program and linked in our shaders,
    // we don't need the local instances anymore.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Setup our vertex data
    GLfloat vertices[] = {-0.5f, -0.5f, 0.0f,
                          0.5f, -0.5f, 0.0f,
                          0.0f,  0.5f, 0.0f};

    GLfloat colors[] = {1.f, 0.f, 0.f,
                        0.f, 1.f, 0.f,
                        0.f, 0.f, 1.f};

    GLuint VAO, VBO;
    GLuint colorsVBO = 0;

    // Initialize our Vertex Array Object and buffer objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &colorsVBO);

    // bind our Vertex Array Object first
    glBindVertexArray(VAO);

    // Initialize the vertex buffer and the color buffer objects so we can
    // Then bind and set our buffers
    // Note: the order in which things are done here is important. The order
    //       of operations that works for me is:
    //       - bind the buffer object
    //       - copy the data into the buffer
    //       - Set the attribute pointer
    //       - move on to the next buffer object
    //       I think the determining factor is that we can only operate
    //       on a single bound buffer at a time.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), (GLvoid*)0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Note that this is allowed, the call to glVertexAttribPointer
    // registered VBO as the currently bound vertex buffer object so
    // afterwards we can safely unbind.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Note: Remember, do NOT unbind the EBO, keep it bound to this VAO

    // Unbind the Vertex Array Object.
    // (It is always good to unbind any buffer/array to prevent strange bugs)
    glBindVertexArray(0);

    // our main loop
    while(!glfwWindowShouldClose(window))
    {
        // check input events(kbd, mouse, etc.)
        glfwPollEvents();

        //
        // rendering routines
        //
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // grab our graphics pipeline context
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // draw our colored triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // cleanup
        glBindVertexArray(0);

        //
        // done rendering
        //

        glfwSwapBuffers(window);
    }

    // Properly deallocate all resources once we are done.
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &colorsVBO);

    glfwTerminate();
    cout << "Terminated GLFW..." << endl;
    return 0;
}

void ConfigureGLFW() {

    // Note: I don't know if it is ever a good idea requiring a version
    //       It would be nice if we could query the version and provide
    //       backward compatibility, at least down to OpenGL version 2.0
    //       (GLSL version 1.x)
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Note: Context profiles are only defined for OpenGL version 3.2
    //       and above.  My old PC uses old NVidia linux drivers, so it is not
    //       possible to set this.
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}


void report_error(int code, const char * description)
{
    cout << "GLFW Error: "
         << "(" << code << ")" << endl
         << '\t' << description
         << endl;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true,
    // closing the application
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
