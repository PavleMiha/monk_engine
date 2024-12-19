/*
 * Copyright 2024 Pavle Mihajlovic
 */
#pragma once

#include "types.h"
#include <atomic>
#include "GLFW/glfw3.h"

enum class EventType
{
	Exit,
	Key,
	Mouse,
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

struct MouseEvent
{
	EventType type = EventType::Mouse;
	int		  button;
	int		  action;
};

struct ResizeEvent
{
	EventType type = EventType::Resize;
	u32		  width;
	u32		  height;
};


