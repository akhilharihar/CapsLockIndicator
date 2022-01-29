#include "TrayIcon.h"
#include <CommCtrl.h>
#include <strsafe.h>
#include "../resource.h"


void TrayIcon::removeIcon() {
	if (nid.hIcon) {
		this->ShowIcon(false);
		DestroyIcon(nid.hIcon);
	}
}

void TrayIcon::reloadIcon() {
	LSTATUS regRequest{};
	DWORD wzCLSID, statuslen{ sizeof(wzCLSID) };
	regRequest = RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", RRF_RT_REG_DWORD, NULL, (void*)&wzCLSID, &statuslen);
	this->removeIcon();
	if (regRequest or !wzCLSID) {
		LoadIconMetric(phInst, MAKEINTRESOURCE(IDI_CAPSLOCK_W), LIM_SMALL, &nid.hIcon);
	}
	else {
		LoadIconMetric(phInst, MAKEINTRESOURCE(IDI_CAPSLOCK_B), LIM_SMALL, &nid.hIcon);
	}
}

TrayIcon::TrayIcon(HWND hWnd, HINSTANCE hInst) {
	phInst = hInst;
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	nid.hWnd = hWnd;
	nid.uFlags = NIF_ICON | NIF_TIP;
	nid.uID = uicond;
	this->reloadIcon();
	StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), L"Caps Lock Enabled");
}

TrayIcon::~TrayIcon() {
	this->removeIcon();
}

void TrayIcon::ShowIcon(bool toShow) {
	Shell_NotifyIcon(toShow ? NIM_ADD : NIM_DELETE, &nid);
}

