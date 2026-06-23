#include "Window.hpp"

Window::Window(WindowHints windowHints, WindowSize windowSize, WindowMode windowMode, const char* title, OpenGLDebugMode debugOpenGL) : windowMode(windowMode) {
    // inits GLFW and handles GLFW window hints.
    if (!glfwInit()) {
        // GLFW failed to init.
    }

    // handles GLFW window hints.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, windowHints.majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, windowHints.minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, windowHints.openglProfile);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // renders the GLFW Window as transparent.
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE); // disables window decorations.
    glfwWindowHint(GLFW_FLOATING, GL_TRUE); // makes the GLFW Window appear on top of other windows.

    // sets GLFW hint for OpenGL Debugging if OpenGL Debug Mode is enabled.
    if (debugOpenGL == OpenGLDebugMode::ENABLED_WITHOUT_NOTIFICATION_LEVEL || debugOpenGL == OpenGLDebugMode::ENABLED) {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    }

    const GLFWvidmode* glfwMonitorInfo = glfwGetVideoMode(glfwGetPrimaryMonitor());
    // determines if the GLFW Window will be in Windowed or Fullscreen mode or if "glfwGetVideoMode()" fails. it will fallback to it's default window size.
    if (glfwMonitorInfo != NULL && this->windowMode != WindowMode::WINDOWED_FALLBACK) {
        if (this->windowMode == WindowMode::WINDOWED) {
            this->window = glfwCreateWindow(glfwMonitorInfo->width, glfwMonitorInfo->height, title, NULL, NULL);
        }
        else if (this->windowMode == WindowMode::FULLSCREEN) {
            glfwWindowHint(GLFW_RED_BITS, glfwMonitorInfo->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, glfwMonitorInfo->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, glfwMonitorInfo->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, glfwMonitorInfo->refreshRate);
            this->window = glfwCreateWindow(glfwMonitorInfo->width, glfwMonitorInfo->height, title, glfwGetPrimaryMonitor(), NULL);
        }
    }
    else {
        // fallback if "glfwGetVideoMode()" fails.
        this->windowMode = WindowMode::WINDOWED_FALLBACK;
        this->window = glfwCreateWindow(windowSize.width, windowSize.height, title, NULL, NULL);
    }

    // creates the GLFW window and checks if it was successfully created.
    if (!this->window) {
        // window creation failed.
    }

    // makes the GLFW window the current context.
    glfwMakeContextCurrent(this->window);

    // passes this instance of the Window class as a void* which allows us to static cast it to a Window class pointer inside of callback functions.
    glfwSetWindowUserPointer(this->window, this);

    // sets the GLFW window size callback so we can resize the OpenGL Viewport when the GLFW window is resized.
    glfwSetWindowSizeCallback(this->window, windowResizeCallback);

    // inits GLAD.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // glad failed to init.
    }

    // creates the OpenGL viewport.
    if (glfwMonitorInfo != NULL) {
        glViewport(0, 0, glfwMonitorInfo->width, glfwMonitorInfo->height);
    }
    else {
        glViewport(0, 0, windowSize.width, windowSize.height);
    }

    // configure global opengl state.
    glEnable(GL_DEPTH_TEST);
    if (debugOpenGL == OpenGLDebugMode::ENABLED_WITHOUT_NOTIFICATION_LEVEL || debugOpenGL == OpenGLDebugMode::ENABLED) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(GLDebugMessageCallback, NULL);
    }

    // disables notification messages if the user doesn't want them.
    if (debugOpenGL == OpenGLDebugMode::ENABLED_WITHOUT_NOTIFICATION_LEVEL) {
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
    }

    // creates the ImGui context and it's IO.
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // sets the ImGui style to dark.
    ImGui::StyleColorsDark();

    // ImGui GLFW & OpenGL init functions.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

int Window::shouldWindowClose() {
    return glfwWindowShouldClose(this->window);
}

void Window::newFrameImGUI() {
    // ImGui NewFrame.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::renderImGUI() {
    // ImGui Render.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::swapBuffers() {
    // swaps the GLFW window buffers.
    glfwSwapBuffers(this->window);
}

void Window::pollEvents() {
    // polls the GLFW events.
    glfwPollEvents();
}

void Window::terminate() {
    // ImGui Shutdown.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // terminates GLFW.
    glfwTerminate();
}

WindowSize Window::getWindowSize() {
    WindowSize size;
    glfwGetWindowSize(this->window, &size.width, &size.height);
    return size;
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    static_cast<Window*>(glfwGetWindowUserPointer(window))->windowSizeChanged = true;
}

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *data) {
    char* _source;
    char* _type;
    char* _severity;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

        case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

        case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

        default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

        case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

        case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

        case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

        case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

        default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

        case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

        case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

        default:
        _severity = "UNKNOWN";
        break;
    }

    printf("%d: %s of %s severity, raised from %s: %s\n", id, _type, _severity, _source, msg);
}
