//============================================================================
// Name        : OpenGL_Hello.cpp
// Author      : James L. Makela
// Version     : 0.1.1
// Copyright   : LGPL v3.0
// Description : OpenGL Hello World in C++ 11 for Legacy OpenGL 2.1.
//               Here is a simple-as-possible example of an OpenGL
//               Program.
//
//               I decided it was time to finally learn some of the ins
//               and outs of OpenGL software development, and of course
//               there were quite a few instructional online resources
//               willing to give a few hints here and there. There happened
//               to be a few online tutorials showcasing an OpenGL version
//               of a "hello world" program named hello_triangle.cpp, which
//               would put a triangle in a window, rendered by OpenGL.
//
//               These tutorials are great, or they would have been if I was
//               using a modern PC with a modern graphics card.  You see,
//               these tutorials were focused on OpenGL 3.3 or higher
//               (as of Jan, 2017).
//
//               As it was, I had an old laptop (circa ~2009) with an old
//               video card (GeForce 7150M / nForce 630M).  And I was running
//               Linux, so the newest drivers I could get only supported
//               OpenGL 2.1 (GLSL version 1.20)
//
//               So I went through a lot of pain and effort, and took a lot
//               of time going through the legacy specifications that I could
//               find online.  And I was finally able to develop a version
//               of the hello_triangle demo program that worked with the older
//               APIs.
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

// we don't have GLSL version 3.3 on our old ass PC
//const GLchar *vertexShaderSource = "#version 330 core\n"
//                                   "\n"
//                                   "layout (location = 0) in vec3 position;\n"
//                                   "\n"
//                                   "void main()\n"
//                                   "{\n"
//                                   "    gl_Position = vec4(position.x,\n"
//                                   "                       position.y,\n"
//                                   "                       position.z,\n"
//                                   "                       1.0);\n"
//                                   "}\n";
//
//const GLchar *fragmentShaderSource = "#version 330 core\n"
//                                     "\n"
//                                     "out vec4 color;\n"
//                                     "\n"
//                                     "void main()\n"
//                                     "{\n"
//                                     "    color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//                                     "}\n";

const GLchar *vertexShaderSource = "#if __VERSION__ >= 140\n"
                                   "    in vec3 position;\n"
                                   "#else\n"
                                   "    attribute vec3 position;\n"
                                   "#endif\n"
                                   "\n"
                                   "void main()\n"
                                   "{\n"
                                   "    gl_Position = vec4(position, 1.0);\n"
                                   "}\n";

const GLchar *fragmentShaderSource = "#if __VERSION__ >= 140\n"
                                     "    varying out vec4 out_color;\n"
                                     "#endif\n"
                                     "\n"
                                     "void main()\n"
                                     "{\n"
                                     "#if __VERSION__ >= 140\n"
                                     "    out_color = vec4(1.0f, 0.5f, 0.2f,\n"
                                     "                     1.0f);\n"
                                     "#else\n"
                                     "    gl_FragColor = vec4(1.0, 0.5, 0.2,\n"
                                     "                        1.0);\n"
                                     "#endif\n"
                                     "}\n";

int main() {
    if (!glfwInit()) {
        // Initialization failed
        cout << "GLFW Initialization Failed!!" << endl;
        return -1;
    }
    else {
        cout << "Initialized GLFW..." << endl;
    }

    glfwSetErrorCallback(&report_error);
    cout << "Set GLFW Error Callback..." << endl;

    ConfigureGLFW();
    cout << "Initialized GLFW Window Hints..." << endl;


    GLFWwindow* window = glfwCreateWindow(800, 600,
                                          "Hello OpenGL",
                                          nullptr, nullptr);

    if (window == nullptr) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    else {
        cout << "Created GLFW window" << endl;
    }

    glfwMakeContextCurrent(window);

    cout << "OpenGL version supported by this platform: "
         << glGetString(GL_VERSION) << endl;
    cout << "GLSL version supported by this platform: "
         << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    // Note: glewInit() will fail if it is attempted before the current
    //       window context is made via glfwMakeContextCurrent()
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cout << "Failed to initialize GLEW" << endl;
        return -1;
    }
    else {
        cout << "Initialized GLEW..." << endl;
    }

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

    // Initialize our Vertex Array Object
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    // Initialize the vertex buffer so we can load data to it
    GLuint VBO;
    glGenBuffers(1, &VBO);

    // bind our Vertex Array Object first
    glBindVertexArray(VAO);

    // Then bind and set our buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW);

    // Tell OpenGL how it should interpret the vertex array data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Note that this is allowed, the call to glVertexAttribPointer
    // registered VBO as the currently bound vertex buffer object so
    // afterwards we can safely unbind.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

        // draw
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
