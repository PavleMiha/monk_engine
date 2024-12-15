/*
 * Copyright 2024 Pavle Mihajlovic
 */

#include "types.h"
#include <atomic>
#include "GLFW/glfw3.h"

enum class EventType
{
	Exit,
	Key,
	Resize
};

struct ExitEvent
{
	EventType type = EventType::Exit;
};

struct KeyEvent
{
	EventType type = EventType::Key;
	int		  key;
	int		  action;
};

struct ResizeEvent
{
	EventType type = EventType::Resize;
	u32		  width;
	u32		  height;
};

extern std::atomic<u8> s_keyMap[GLFW_KEY_LAST];
