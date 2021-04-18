#include <Windows.h>
#include <cstdint>
#include <iostream>
#include "nt.h"
#include "rzctl.h"

HANDLE hDevice = INVALID_HANDLE_VALUE;

bool rzctl::init()
{
	if (hDevice != INVALID_HANDLE_VALUE)
		CloseHandle(hDevice);
		
	std::wstring name;

	if(!nt::find_sym_link(L"\\GLOBAL??", L"RZCONTROL", name))
		return false;

	std::wstring sym_link = L"\\\\?\\" + name;

	hDevice = CreateFileW(sym_link.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
// std::wstring path;
//	nt::get_path(hDevice, path);
//	std::wcout << path;


	return hDevice != INVALID_HANDLE_VALUE;
}

void rzctl::mouse_move(int x, int y, bool from_start_point)
{
	int max_val = 0;

	/*
	* To avoid errors, lets just fix the x and y :p
	*/
	if (!from_start_point) {
		max_val = MAX_VAL;
		if (x < 1)
			x = 1;

		if (x > max_val) 
			x = max_val;

		if (y < 1)
			y = 1;

		if (y > max_val) 
			y = max_val;
	} 

	/*
	* Hardcoded values copied from looking at buffer passed from "Razer Synapse Service.exe"
	*/
	MOUSE_IOCTL_STRUCT mm = {
		0, 2,
		max_val,
		0, 0,
		x, y,
		0
	};
	_impl_mouse_ioctl(&mm);
}

void rzctl::mouse_click(int up_down)
{
	MOUSE_IOCTL_STRUCT mm = {
		0, 2,
		0, up_down,
		0, 0,
		0, 0,
	};
	_impl_mouse_ioctl(&mm);
}

void rzctl::_impl_mouse_ioctl(MOUSE_IOCTL_STRUCT* pMi)
{
	if (pMi)
	{
		DWORD junk;

		BOOL bResult = DeviceIoControl(hDevice, IOCTL_MOUSE, pMi, sizeof(MOUSE_IOCTL_STRUCT), NULL, 0, &junk, NULL);
		
		// Re-open handle incase of failure
		if (!bResult)
			init();
	}
}
