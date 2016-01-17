// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include<iostream>
using std::cout;
using std::endl;

int width = 754;
int height = 1100;

class Button : public wxFrame
{
	public:
		Button(const wxString& title);
		void OnQuit(wxCommandEvent& event);
};

Button::Button(const wxString& title)
	:wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(270,150))
{
	wxPanel* panel = new wxPanel(this, wxID_ANY);
	wxImageHandler* jpegLoader = new wxJPEGHandler();
	wxImage::AddHandler(jpegLoader);
	wxImage jpgImage;
	jpgImage.LoadFile("./manga/Naruto/1-Uzumaki Naruto/2.jpg", wxBITMAP_TYPE_JPEG);
	cout << jpgImage.GetWidth() << " " << jpgImage.GetHeight() << endl;
	jpgImage.Rescale(width, height);
	wxBitmap bitmap(jpgImage);
	wxBitmapButton* button = new wxBitmapButton(panel, wxID_EXIT, bitmap, wxPoint(20, 20), wxSize(width, height));
	Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Button::OnQuit));
	button->SetFocus();
	Centre();
}

void Button::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

class MyApp : public wxApp
{
	public:
		virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	Button* btnapp = new Button(wxT("Button"));
	btnapp->Show(true);
	return true;
}
