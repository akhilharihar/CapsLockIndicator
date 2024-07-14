#include <string>

#include <Windows.h>

#ifndef _TRAYICON_H_
#define _TRAYICON_H_

class TrayIcon {
	NOTIFYICONDATAW iconData;
	HINSTANCE instanceHandle{};
public:
	TrayIcon(HINSTANCE, HWND, UINT, std::wstring &);
	~TrayIcon();
	void show();
	void hide();
};

#endif // !TRAYICON_H_

