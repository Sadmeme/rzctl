#pragma once

#define IOCTL_MOUSE 0x88883020

#define MAX_VAL 65536
#include <cstdint>

class MOUSE_IOCTL_STRUCT
{
public:
	int32_t unk0; //0x0000
	int32_t unk1; //0x0004
	int32_t max_val; //0x0008
	int32_t unk2; //0x000C
	int32_t unk3; //0x0010
	int32_t x; //0x0014
	int32_t y; //0x0018
	int32_t unk4; //0x001C
}; //Size: 0x0020

enum MOUSE_CLICK {
	LEFT_DOWN = 1,
	LEFT_UP = 2
};

namespace rzctl {
	/*
	* finds the symbolic link that contains name RZCONTROL and opens a handle to the respective device
	*/
	bool init();

	/*
	* if going from point, x and y will be the offset from current mouse position
	* otherwise it will be a number in range of 1 to 65536, where 1, 1 is top left of screen
	* if using multiple monitors the input values remain the same, but outcome different, i just don't recommend bothering with this bs
	* note: x and/or y can not be 0 unless going from start point	
	*/
	void mouse_move(int x, int y, bool from_start_point = true);
	void mouse_click(int up_down);

	/*
	* communicates to the device
	*/
	void _impl_mouse_ioctl(MOUSE_IOCTL_STRUCT* pMi);
}