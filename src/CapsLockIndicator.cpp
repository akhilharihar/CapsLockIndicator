#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "CapsLockIndicator.h"
#include "../resource.h"
#include <CommCtrl.h>
#include "TrayIcon.h"

HANDLE hMutexHandle;
const wchar_t szWindowClass[] = L"CapsLockWindowClass";

HHOOK gHK;
TrayIcon* CapsLockIcon{ nullptr };

int WINAPI wWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE,_In_ PWSTR pCmdLine,_In_ int nCmdShow) {
	// Ensure that there's only one instance of this application.
	hMutexHandle = CreateMutex(NULL, true, szWindowClass);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		return 8;
	}

	HWND gHwnd{ NULL };

	// Register Window class
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = szWindowClass;
	LoadIconMetric(hInstance, MAKEINTRESOURCE(IDI_CAPSLOCK_B), LIM_SMALL, &wc.hIcon);
	LoadIconMetric(hInstance, MAKEINTRESOURCE(IDI_CAPSLOCK_B), LIM_SMALL, &wc.hIconSm);
	RegisterClassEx(&wc);

	// Window
#if _DEBUG
	HWND ParentWindow = NULL;
#else
	HWND ParentWindow = HWND_MESSAGE;
#endif

	gHwnd = CreateWindowW(
		szWindowClass,                     // Window class
		L"CapsLock Indicator",    // Window title
		WS_OVERLAPPEDWINDOW,            // Window style
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		ParentWindow,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (!gHwnd) {
		return 10;
	}

	// register tray icon
	CapsLockIcon = new TrayIcon(gHwnd, hInstance);

	// Keyboard hook
	gHK = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
	if (!gHK) {
		return 11;
	}

	ShowWindow(gHwnd, nCmdShow);

	// Run the message loop.
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	cleanup();

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0 || wParam != WM_KEYUP) {

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	PKBDLLHOOKSTRUCT inputKey = (PKBDLLHOOKSTRUCT)lParam;

	if (inputKey->vkCode == VK_CAPITAL) {

		if ((GetKeyState(VK_CAPITAL) & 1)) {
			CapsLockIcon->ShowIcon();
		}
		else {
			CapsLockIcon->ShowIcon(false);
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void cleanup() {
	ReleaseMutex(hMutexHandle);
	CloseHandle(hMutexHandle);
	UnhookWindowsHookEx(gHK);
	if (CapsLockIcon) {
		delete(CapsLockIcon);
		CapsLockIcon = nullptr;
	}
}