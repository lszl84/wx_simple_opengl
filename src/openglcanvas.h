#pragma once

#include <glad/glad.h>

#include <wx/glcanvas.h>
#include <wx/wx.h>


wxDECLARE_EVENT(wxEVT_OPENGL_INITIALIZED, wxCommandEvent);

class OpenGLCanvas : public wxGLCanvas {
public:
    OpenGLCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs);
    ~OpenGLCanvas();

    bool InitializeOpenGL();

    bool IsOpenGLInitialized() const { return isOpenGLInitialized; }

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);

private:
    bool InitializeOpenGLFunctions();

    GLuint shaderProgram { 0 };
    GLuint quadVAO { 0 };
    GLuint quadVBO { 0 };
    wxGLContext* openGLContext;
    bool isOpenGLInitialized { false };
};
