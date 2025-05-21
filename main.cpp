#include "main.h"
#include "rc.h"
#include <filesystem>

void main_window::on_paint(HDC hdc) 
{
	RECT client_rect;
	GetClientRect(*this, &client_rect);

	if (loaded_image)
	{
		Gdiplus::Graphics graphics(hdc);
		Gdiplus::RectF display_area(
			static_cast<Gdiplus::REAL>(client_rect.left),
			static_cast<Gdiplus::REAL>(client_rect.top),
			static_cast<Gdiplus::REAL>(client_rect.right - client_rect.left),
			static_cast<Gdiplus::REAL>(client_rect.bottom - client_rect.top)
		);

		graphics.DrawImage(loaded_image.get(), display_area);

		int width = client_rect.right - client_rect.left;
		int height = client_rect.bottom - client_rect.top;
		int font_size = static_cast<int>(width * 0.035);

		Gdiplus::Font font(L"Calibri", static_cast<Gdiplus::REAL>(font_size), Gdiplus::FontStyleBold);
		Gdiplus::StringFormat format;
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
		format.SetLineAlignment(Gdiplus::StringAlignmentFar);

		Gdiplus::SolidBrush shadow_brush(Gdiplus::Color(100, 0, 0, 0)); // poluprozirna sjena
		Gdiplus::SolidBrush text_brush(Gdiplus::Color::White);

		Gdiplus::RectF shadow_area = display_area;
		shadow_area.X += 2;
		shadow_area.Y += 2;

		graphics.DrawString(file_name.c_str(), -1, &font, shadow_area, &format, &shadow_brush);
		graphics.DrawString(file_name.c_str(), -1, &font, display_area, &format, &text_brush);
	}
}

void main_window::on_command(int id) 
{
	switch (id) 
	{
		case ID_OPEN:
		{
			TCHAR file_path[MAX_PATH]{};
			OPENFILENAME ofn{ sizeof(OPENFILENAME) };
			ofn.hwndOwner = *this;
			ofn.lpstrFile = file_path;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = _T("Images (*.jpg;*.png;*.bmp)\0*.jpg;*.png;*.bmp\0");
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

			if (GetOpenFileName(&ofn)) {
				file_name = std::filesystem::path(file_path).filename().wstring();
				loaded_image = std::make_unique<Gdiplus::Image>(file_path);
				::InvalidateRect(*this, nullptr, TRUE);
			}
			break;
		}
		case ID_EXIT:
			DestroyWindow(*this);
			break;
	}
}

void main_window::on_destroy() 
{
	::PostQuitMessage(0);
}

int WINAPI _tWinMain(HINSTANCE instance, HINSTANCE, LPTSTR, int)
{
	gdiplus_application app;
	main_window wnd;
	wnd.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, _T("NWP 7"), (int64_t)LoadMenu(instance, MAKEINTRESOURCE(IDM_MAIN)));
	return app.run();
}
