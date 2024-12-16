/*
 * Copyright 2024 Pavle Mihajlovic
 */

#include "input.h"

bool isPressed(u32 key) {
	return s_keyMap[key].load();
}
