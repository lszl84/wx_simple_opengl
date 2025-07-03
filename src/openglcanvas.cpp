#include "openglcanvas.h"

wxDEFINE_EVENT(wxEVT_OPENGL_INITIALIZED, wxCommandEvent);

constexpr auto VertexShaderSource = R"(#version 330 core

    layout(location = 0) in vec3 inPosition;

    void main()
    {
        gl_Position = vec4(inPosition, 1.0);
    }
)";

constexpr auto FragmentShaderSource = R"(#version 330 core
out vec4 fragColor;

void main()
{
    fragColor = vec4(1.0, 0.4, 0.2, 1.0); // Orange color
}
)";

OpenGLCanvas::OpenGLCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs)
    : wxGLCanvas(parent, canvasAttrs)
{
    wxGLContextAttrs ctxAttrs;
    ctxAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 3).EndList();
    openGLContext = new wxGLContext(this, nullptr, &ctxAttrs);

    if (!openGLContext->IsOK()) {
        wxMessageBox("This sample needs an OpenGL 3.3 capable driver.",
            "OpenGL version error", wxOK | wxICON_INFORMATION, this);
        delete openGLContext;
        openGLContext = nullptr;
    }

    Bind(wxEVT_PAINT, &OpenGLCanvas::OnPaint, this);
    Bind(wxEVT_SIZE, &OpenGLCanvas::OnSize, this);
}


OpenGLCanvas::~OpenGLCanvas() { 

    if (isOpenGLInitialized) {
        SetCurrent(*openGLContext);

        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
        glDeleteProgram(shaderProgram);
    }
    delete openGLContext; 
}

bool OpenGLCanvas::InitializeOpenGLFunctions()
{
    return gladLoadGL();
}

bool OpenGLCanvas::InitializeOpenGL()
{
    if (!openGLContext) {
        return false;
    }

    SetCurrent(*openGLContext);

    if (!InitializeOpenGLFunctions()) {
        wxMessageBox("Error: Could not initialize OpenGL function pointers.",
            "OpenGL initialization error", wxOK | wxICON_INFORMATION,
            this);
        return false;
    }

    wxLogDebug("OpenGL version: %s",
        reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    wxLogDebug("OpenGL vendor: %s",
        reinterpret_cast<const char*>(glGetString(GL_VENDOR)));

    GLfloat quadVertices[] = {
        -0.5f, -0.5f, 0.0f, // Bottom left
         0.5f, -0.5f, 0.0f, // Bottom right
        -0.5f,  0.5f, 0.0f, // Top left
         0.5f,  0.5f, 0.0f  // Top right
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
        GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(0);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    isOpenGLInitialized = true;

    wxCommandEvent evt(wxEVT_OPENGL_INITIALIZED);
    evt.SetEventObject(this);
    ProcessWindowEvent(evt);

    return true;
}

void OpenGLCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    if (!isOpenGLInitialized) {
        return;
    }

    SetCurrent(*openGLContext);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLint depthFuncValue;
    glGetIntegerv(GL_DEPTH_FUNC, &depthFuncValue);
    glClearDepth(depthFuncValue == GL_LESS ? 1.0f : 0.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    
    SwapBuffers();
}

void OpenGLCanvas::OnSize(wxSizeEvent& event)
{
    bool firstApperance = IsShownOnScreen() && !isOpenGLInitialized;

    if (firstApperance) {
        InitializeOpenGL();
    }

    if (isOpenGLInitialized) {
        SetCurrent(*openGLContext);

        auto viewPortSize = event.GetSize() * GetContentScaleFactor();
        glViewport(0, 0, viewPortSize.x, viewPortSize.y);
    }

    event.Skip();
}

