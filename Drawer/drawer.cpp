#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
namespace
{
	void entry();
	void render();
	LRESULT __stdcall DrawerWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int nShowCmd = SW_SHOW;
	HINSTANCE hInstance;
	HWND hWnd = nullptr;
	HDC backBufferDC = nullptr;
	HBITMAP backBufferBMP = nullptr;
	HPEN hPen = nullptr;
	HBRUSH hBrush = nullptr;
	bool init = false;
	bool opaquePen = false;
	bool opaqueBrush = true;
	int x = 0, y = 0;

	namespace draw
	{
		void redraw();
	}
}
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd)
{
	::nShowCmd = nShowCmd;
	::hInstance = hInstance;
	entry();
	return 0;
}
int main(int argc, char **argv)
{
	hInstance = GetModuleHandleA(nullptr);
	entry();
	return 0;
}

void Init();
void Draw();

namespace
{
	void draw::redraw()
	{
		InvalidateRect(hWnd, nullptr, FALSE);
	}


	void entry()
	{
		// Register window class
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DrawerWindowProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = "DrawerWindowClass";
		wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_WINLOGO);
		if (!RegisterClassEx(&wcex))
			return;

		// Create window
		RECT rc = { 0, 0, 640, 480 };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		hWnd = CreateWindowExA(0L, "DrawerWindowClass", "Drawer", WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX),
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
		if (!hWnd)
			return;

		// Show window
		ShowWindow(hWnd, nShowCmd);

		// Application init
		Init();

		init = true;
		// Prepare
		opaquePen = true;
		opaqueBrush = false;
		hBrush = CreateSolidBrush(0xFFFFFF);
		FillRect(backBufferDC, &RECT({ 0, 0, 640, 480 }), hBrush);
		DeleteObject(hBrush);
		hPen = CreatePen(PS_SOLID, 1, 0);
		opaquePen = false;
		opaqueBrush = true;

		// Main loop
		MSG msg = { 0 };
		while (WM_QUIT != msg.message) {
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
				render();
		}
	}
	void render()
	{
		Draw();
		HDC hdc = GetDC(hWnd);
		BitBlt(hdc, 0, 0, 640, 480, backBufferDC, 0, 0, SRCCOPY);
		ReleaseDC(hWnd, hdc);
	}

	LRESULT __stdcall DrawerWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HDC hdc;

		switch (uMsg)
		{
		case WM_CREATE:
			// Create back buffer
			hdc = GetDC(hWnd);
			backBufferBMP = CreateCompatibleBitmap(hdc, 640, 480);
			if (backBufferBMP == nullptr)
				PostQuitMessage(0);
			backBufferDC = CreateCompatibleDC(hdc);
			if (backBufferDC == nullptr)
				PostQuitMessage(0);
			DeleteObject((HBITMAP)SelectObject(backBufferDC, backBufferBMP));
			ReleaseDC(hWnd, hdc);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		}
		return 0;
	}
}

void DrawLine(int from_x, int from_y, int to_x, int to_y)
{
	if (!init) return;

	if (!opaquePen)
	{
		MoveToEx(backBufferDC, from_x, from_y, nullptr);
		LineTo(backBufferDC, to_x, to_y);
	}
}
void MoveTo(int x, int y)
{
	if (!init) return;

	::x = x;
	::y = y;
}
void LineTo(int x, int y)
{
	if (!init) return;

	DrawLine(::x, ::y, x, y);
	MoveTo(x, y);
}
void DrawRect(int x1, int y1, int x2, int y2)
{
	if (!init) return;

	auto rc = RECT({ x1, y1, x2, y2 });
	if (!opaqueBrush)
		FillRect(backBufferDC, &rc, hBrush);

	if (!opaquePen)
	{
		DrawLine(x1, y1, x2, y1);
		DrawLine(x1, y2, x2, y2);
		DrawLine(x1, y1, x1, y2);
		DrawLine(x2, y1, x2, y2);
	}
}
void DrawEllipse(int x1, int y1, int x2, int y2)
{
	if (!init || (opaquePen && opaqueBrush)) return;

	if (opaquePen)
	{
		LOGBRUSH log;
		GetObjectA(hBrush, sizeof(LOGBRUSH), &log);
		SelectObject(backBufferDC, CreatePen(PS_SOLID, 0, log.lbColor));
	}
	Ellipse(backBufferDC, x1, y1, x2, y2);
	if (opaquePen)
	{
		DeleteObject((HPEN)SelectObject(backBufferDC, hPen));
	}
}
void SetPenColor(unsigned char r, unsigned char g, unsigned char b)
{
	if (!init) return;

	opaquePen = false;
	DeleteObject((HPEN)SelectObject(backBufferDC, hPen = CreatePen(PS_SOLID, 1, RGB(r, g, b))));
}
void SetPenTransparent()
{
	if (!init) return;

	opaquePen = true;
}
void SetBrushColor(unsigned char r, unsigned char g, unsigned char b)
{
	if (!init) return;

	opaqueBrush = false;
	DeleteObject((HBRUSH)SelectObject(backBufferDC, hBrush = CreateSolidBrush(RGB(r, g, b))));
}
void SetBrushTransparent()
{
	if (!init) return;

	DeleteObject((HBRUSH)SelectObject(backBufferDC, GetStockObject(HOLLOW_BRUSH)));
	opaqueBrush = true;
}
void Fill(unsigned char r, unsigned char g, unsigned char b)
{
	if (!init) return;

	bool tOP = opaquePen;
	bool tOB = opaqueBrush;
	opaquePen = true;
	opaqueBrush = false;
	HBRUSH brush = CreateSolidBrush(RGB(r, g, b));
	FillRect(backBufferDC, &RECT({ 0, 0, 640, 480 }), brush);
	DeleteObject(brush);
	opaquePen = tOP;
	opaqueBrush = tOB;
}