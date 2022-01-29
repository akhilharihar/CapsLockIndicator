#ifndef TRAYICON_H_
#define TRAYICON_H_
#include <Windows.h>

class TrayIcon {
	NOTIFYICONDATAW nid;
	static const UINT uicond{742};
	HINSTANCE phInst{};
	void removeIcon();
public:
	void reloadIcon();
	TrayIcon(HWND, HINSTANCE);
	~TrayIcon();
	void ShowIcon(bool = true);
};

#endif // !TRAYICON_H_

