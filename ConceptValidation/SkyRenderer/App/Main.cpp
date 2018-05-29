#define NOMINMAX
#include <windows.h>

#include <Sauron/Context.hpp>
#include <Sauron/Core.hpp>

using namespace Sauron;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int main()
{
	::HINSTANCE instance = GetModuleHandle(nullptr);

	::WNDCLASSEX wcex;
	wcex.cbSize = sizeof(::WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"SauronWindowClass";
	wcex.hIconSm = nullptr;
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	RECT rc = { 0, 0, 1280, 720 };
	::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND wnd = ::CreateWindow(L"SauronWindowClass", L"Sauron",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, instance,
		nullptr);
	if (!wnd)
		return 1;

	::ShowWindow(wnd, SW_NORMAL);

	auto& context = Context::GetInstance();
	context.Init(wnd);

	::MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			context.Update(0);
			context.Draw();
		}
	}

	context.Deinit();

	return static_cast<int>(msg.wParam);
}
