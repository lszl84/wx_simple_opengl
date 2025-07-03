#include <wx/settings.h>
#include <wx/wx.h>

#include "openglcanvas.h"

class MyApp : public wxApp {
public:
    MyApp() { }
    bool OnInit() wxOVERRIDE;
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);

private:
    OpenGLCanvas* openGLCanvas { nullptr };
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    MyFrame* frame = new MyFrame("Hello OpenGL");
    frame->Show(true);

    return true;
}


MyFrame::MyFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title)
{
    wxGLAttributes vAttrs;
    vAttrs.PlatformDefaults().Defaults().EndList();

    if (wxGLCanvas::IsDisplaySupported(vAttrs)) {
        openGLCanvas = new OpenGLCanvas(this, vAttrs);
    }
}


