#pragma once
#include <d3d9.h>
#include "settings.h"

namespace menu 
{
	inline bool show_menu = true;

	inline bool setup = false;

	inline HWND window = nullptr;
	inline WNDCLASSEX windowClass = { };
	inline WNDPROC originalWindowProcess = nullptr;

	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline LPDIRECT3D9 d3d9 = nullptr;

	bool SetupWindowClass(const char* windowClassName) noexcept;
	void DestroyWindowClass() noexcept;

	bool SetupWindow(const char* windowName) noexcept;
	void DestroyWindow() noexcept;

	bool SetupDirectX() noexcept;
	void DestroyDirectX() noexcept;

	void Setup();

	void SetupMenu(LPDIRECT3DDEVICE9 device) noexcept;
	void Destroy() noexcept;

	void Render() noexcept;

	void MenuStyle();

	void change_click(bool canclick);
	inline bool clickable = false;

	void cheatLoop();
}