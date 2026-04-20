#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <direct.h>

GLuint LoadShader(const char* vPath, const char* fPath) {
    auto readFile = [](const char* path) -> std::string {
        std::ifstream file(path);
        if (!file.is_open()) return "";
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
        };

    std::string vSrcStr = readFile(vPath);
    std::string fSrcStr = readFile(fPath);

    if (vSrcStr.empty() || fSrcStr.empty()) {
        std::cout << "Hiba: shader fajl nem talalhato!" << std::endl;
        return 0;
    }

    const char* vSrc = vSrcStr.c_str();
    const char* fSrc = fSrcStr.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vSrc, NULL);
    glCompileShader(vs);

    GLint success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(vs, 1024, NULL, infoLog);
        std::cout << "Vertex shader hiba:\n" << infoLog << std::endl;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fSrc, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(fs, 1024, NULL, infoLog);
        std::cout << "Fragment shader hiba:\n" << infoLog << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cout << "Shader linkelesi hiba:\n" << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

glm::vec2 cPos(300.0f, 300.0f);
glm::vec2 vel(4.0f, 0.0f);   // induláskor jobbra megy
float padY = 300.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        if (key == GLFW_KEY_S) {
            cPos = glm::vec2(300.0f, 300.0f);
            vel = glm::vec2(4.0f, 0.0f);
        }

        if (key == GLFW_KEY_UP) {
            padY += 15.0f;
            if (padY > 590.0f) padY = 590.0f;
        }

        if (key == GLFW_KEY_DOWN) {
            padY -= 15.0f;
            if (padY < 10.0f) padY = 10.0f;
        }
    }
}

int main() {
    char cwd[1024];
    _getcwd(cwd, sizeof(cwd));
    std::cout << "Aktualis futasi mappa: " << cwd << std::endl;

    if (!glfwInit()) {
        std::cout << "GLFW init hiba!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(600, 600, "Foldesi_Kristof_WD97DD", NULL, NULL);
    if (!win) {
        std::cout << "Ablak letrehozas hiba!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(win);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW init hiba!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(win, key_callback);

    GLuint prog = LoadShader("vertexShader.glsl", "fragmentShader.glsl");
    if (prog == 0) {
        glfwTerminate();
        return -1;
    }

    float vertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(win)) {
        // MOZGÁS minden frame-ben
        cPos += vel;

        // bal-jobb visszapattanás
        if (cPos.x <= 50.0f) {
            cPos.x = 50.0f;
            vel.x = 4.0f;
        }

        if (cPos.x >= 550.0f) {
            cPos.x = 550.0f;
            vel.x = -4.0f;
        }

        int fbW, fbH;
        glfwGetFramebufferSize(win, &fbW, &fbH);
        glViewport(0, 0, fbW, fbH);

        glClearColor(1.0f, 0.9f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);

        int hit = 0;
        if (std::abs(cPos.y - padY) <= 50.0f &&
            cPos.x >= 200.0f &&
            cPos.x <= 400.0f) {
            hit = 1;
        }

        glUniform2f(glGetUniformLocation(prog, "u_cPos"), cPos.x, cPos.y);
        glUniform2f(glGetUniformLocation(prog, "u_res"), (float)fbW, (float)fbH);
        glUniform1i(glGetUniformLocation(prog, "u_hit"), hit);
        glUniform1f(glGetUniformLocation(prog, "u_padY"), padY);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(prog);

    glfwTerminate();
    return 0;
}