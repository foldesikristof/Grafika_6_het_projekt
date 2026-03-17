#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <direct.h>

GLuint LoadShader(const char* vPath, const char* fPath) {
    auto read = [](const char* p) {
        std::ifstream s(p); std::stringstream ss; ss << s.rdbuf(); return ss.str();
    };
    std::string vS = read(vPath), fS = read(fPath);
    if(vS.empty() || fS.empty()) std::cout << "Hiba: Shader fajl nem talalhato!" << std::endl;
    const char *vSrc = vS.c_str(), *fSrc = fS.c_str();
    GLuint vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs, 1, &vSrc, NULL); glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs, 1, &fSrc, NULL); glCompileShader(fs);
    GLuint p = glCreateProgram(); glAttachShader(p, vs); glAttachShader(p, fs); glLinkProgram(p);
    return p;
}

glm::vec2 cPos(300.0f, 300.0f), vel(0.0f, 0.0f); // ablak kozepe
float padY = 300.0f;
bool active = false;

void key_callback(GLFWwindow* w, int k, int s, int a, int m) {
    if (a == GLFW_PRESS || a == GLFW_REPEAT) {
        if (k == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(w, GL_TRUE); // ESC kilepes
        
        if (k == GLFW_KEY_S) { // S gomb inditas
            float rad = 25.0f * 3.14 / 180.0f; // iranyvektor 25
            vel = glm::vec2(std::cos(rad), std::sin(rad)) * 4.0f;
            active = true;
        }
        if (k == GLFW_KEY_UP) padY = std::min(590.0f, padY + 15.0f); // szakasz fel mozog
        if (k == GLFW_KEY_DOWN) padY = std::max(10.0f, padY - 15.0f); // szakasz le mozog
    }
}

int main() {
    char cwd[1024];
    _getcwd(cwd, sizeof(cwd));
    std::cout << "Aktualis futasi mappa: " << cwd << std::endl;

    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(600, 600, "Foldesi_Kristof_WD97DD", NULL, NULL); // 600x600 ablak
    glfwMakeContextCurrent(win);
    glewExperimental = GL_TRUE; glewInit();

    GLuint prog = LoadShader("./vertexShader.glsl", "./fragmentShader.glsl");
    float v[] = { -1,-1,0, 1,-1,0, 1,1,0, -1,1,0 };
    GLuint vao, vbo; glGenVertexArrays(1, &vao); glBindVertexArray(vao);
    glGenBuffers(1, &vbo); glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); glEnableVertexAttribArray(0);

    glfwSetKeyCallback(win, key_callback);

    while (!glfwWindowShouldClose(win)) {
        if (active) {
            cPos += vel; // mozgas
            if (cPos.x < 50 || cPos.x > 550) { vel.x *= -1; cPos.x = glm::clamp(cPos.x, 50.1f, 549.9f); } // jobb-bal visszapattanas
            if (cPos.y < 50 || cPos.y > 550) { vel.y *= -1; cPos.y = glm::clamp(cPos.y, 50.1f, 549.9f); } // felso-also visszapattanas 
        }

        int fbW, fbH;
        glfwGetFramebufferSize(win, &fbW, &fbH);
        glViewport(0, 0, fbW, fbH);

        glClearColor(1.0f, 0.9f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        bool hit = (std::abs(cPos.y - padY) <= 50.0f) && (cPos.x >= 200.0f && cPos.x <= 400.0f);
        
        glUniform2f(glGetUniformLocation(prog, "u_cPos"), cPos.x, cPos.y); // uniform valtozokent atadas
        glUniform2f(glGetUniformLocation(prog, "u_res"), (float)fbW, (float)fbH);
        glUniform1i(glGetUniformLocation(prog, "u_hit"), hit);
        glUniform1f(glGetUniformLocation(prog, "u_padY"), padY);

        glBindVertexArray(vao); glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glfwSwapBuffers(win); glfwPollEvents();
    }
    glfwTerminate(); return 0;
}