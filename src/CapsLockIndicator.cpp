#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <string>
#include <memory>

#include <Windows.h>
#include <CommCtrl.h>

#include "resource.h"
#include "TrayIcon.h"
#include "ReturnStatus.h"

HANDLE applicationMutexHandle;
const std::wstring windowClassName{ L"CapsLockWindowClass" };

HHOOK applicationWindowHook;
std::unique_ptr<TrayIcon> capsLockIcon;

LRESULT CALLBACK processWindowMessageHandler(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK keyboardInputHandler(int, WPARAM, LPARAM);
UINT getCapsLockIconResourceValue();
HANDLE createMutexHandle();
void cleanupMutexHandle(HANDLE);
void cleanup();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// Ensure that there's only one instance of this application.
	applicationMutexHandle = createMutexHandle();
	if (!applicationMutexHandle) {
		return ReturnStatus::INSTANCE_EXISTS;
	}

	HWND windowHandle{ nullptr };
	WNDCLASSEXW windowClass{ sizeof(WNDCLASSEXW) };

	windowClass.lpfnWndProc = processWindowMessageHandler;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = windowClassName.c_str();
	windowClass.hIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_CAPSLOCK_DARK_ICON));
	windowClass.hIconSm = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_CAPSLOCK_DARK_ICON));
	RegisterClassExW(&windowClass);

	// Window

	windowHandle = CreateWindowW(
		windowClassName.c_str(),                     // Window class
		L"Caps Lock Indicator",    // Window title
		WS_OVERLAPPEDWINDOW,            // Window style
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,       // Parent window    
		nullptr,       // Menu
		hInstance,  // Instance handle
		nullptr        // Additional application data
	);

	if (!windowHandle) {
		cleanupMutexHandle(applicationMutexHandle);
		return ReturnStatus::ERROR_CREATING_WINDOW;
	}

	std::wstring toolTip{ L"Caps Lock Enabled" };

	// register tray icon
	capsLockIcon = std::make_unique<TrayIcon>(hInstance, windowHandle, getCapsLockIconResourceValue(), toolTip);

	// Keyboard hook
	applicationWindowHook = SetWindowsHookExW(WH_KEYBOARD_LL, keyboardInputHandler, nullptr, 0);
	if (!applicationWindowHook) {
		DestroyWindow(windowHandle);
		cleanupMutexHandle(applicationMutexHandle);
		return ReturnStatus::ERROR_REGISTERING_HOOK;
	}

#if _DEBUG
	ShowWindow(windowHandle, nCmdShow);
#endif

	// Run the message loop.
	MSG windowMessages = { };
	while (GetMessage(&windowMessages, nullptr, 0, 0) > 0) {
		TranslateMessage(&windowMessages);
		DispatchMessage(&windowMessages);
	}

	return ReturnStatus::OK;
}

LRESULT CALLBACK processWindowMessageHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE:
	case WM_QUIT:
	case WM_QUERYENDSESSION: {
		cleanup();
		DestroyWindow(hwnd);
		PostQuitMessage(ReturnStatus::OK);
		return 0;
	}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT keyboardInputHandler(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0 || wParam != WM_KEYUP) {

		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	PKBDLLHOOKSTRUCT inputKey = (PKBDLLHOOKSTRUCT)lParam;

	if (inputKey->vkCode == VK_CAPITAL) {

		if ((GetKeyState(VK_CAPITAL) & 1)) {
			capsLockIcon->show();
		}
		else {
			capsLockIcon->hide();
		}
	}

	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

HANDLE createMutexHandle() {
	return CreateMutexW(nullptr, true, windowClassName.c_str());
}

void cleanupMutexHandle(HANDLE mutexHandle) {
	ReleaseMutex(mutexHandle);
	CloseHandle(mutexHandle);
}

void cleanup() {
	cleanupMutexHandle(applicationMutexHandle);
	UnhookWindowsHookEx(applicationWindowHook);
	capsLockIcon.reset();
}

UINT getCapsLockIconResourceValue() {
	LSTATUS regRequest{};
	DWORD wzCLSID;
	DWORD statuslen{ sizeof(wzCLSID) };
	regRequest = RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, (void*)&wzCLSID, &statuslen);
	if (regRequest || !wzCLSID) {
		return IDI_CAPSLOCK_ICON;
	}

	return IDI_CAPSLOCK_DARK_ICON;
}