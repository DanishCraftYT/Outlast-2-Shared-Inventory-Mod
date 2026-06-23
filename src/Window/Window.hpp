#pragma once

#include <stdio.h>
#include <functional>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

struct WindowHints {
    int majorVersion;
    int minorVersion;
    int openglProfile;
};

struct WindowSize {
    int width;
    int height;
};

enum WindowMode {
    WINDOWED,
    WINDOWED_FALLBACK,
    FULLSCREEN
};

enum OpenGLDebugMode {
    DISABLED,
    ENABLED_WITHOUT_NOTIFICATION_LEVEL,
    ENABLED
};

class Window {
public:
    Window(WindowHints windowHints, WindowSize windowSize, WindowMode windowMode, const char* title, OpenGLDebugMode debugOpenGL=OpenGLDebugMode::DISABLED);
    Window() = default;
    int shouldWindowClose();
    void newFrameImGUI();
    void renderImGUI();
    void swapBuffers();
    void pollEvents();
    void terminate();
    WindowSize getWindowSize();
    bool windowSizeChanged;
private:
    GLFWwindow* window;
    WindowMode windowMode;
};

void windowResizeCallback(GLFWwindow* window, int width, int height);
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *data);
