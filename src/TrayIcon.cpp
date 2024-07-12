#include "TrayIcon.h"
#include <CommCtrl.h>

TrayIcon::TrayIcon(HINSTANCE hInst, HWND hWnd, UINT iconResource, std::wstring &toolTip) {
	instanceHandle = hInst;
	iconData.cbSize = sizeof(NOTIFYICONDATAW);
	iconData.hWnd = hWnd;
	iconData.uFlags = NIF_ICON | NIF_TIP;
	iconData.uID = iconResource;
	LoadIconMetric(instanceHandle, MAKEINTRESOURCE(iconResource), LIM_SMALL, &iconData.hIcon);
	toolTip.copy(iconData.szTip, toolTip.length());
}

TrayIcon::~TrayIcon() {
	hide();
	DestroyIcon(iconData.hIcon);
}

void TrayIcon::show() {
	Shell_NotifyIcon(NIM_ADD, &iconData);
}

void TrayIcon::hide() {
	Shell_NotifyIcon(NIM_DELETE, &iconData);
}
