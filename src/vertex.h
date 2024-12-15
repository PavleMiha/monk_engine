/*
 * Copyright 2024 Pavle Mihajlovic
 */
#pragma once

#include "types.h"

#include <bgfx/bgfx.h>
 
struct PosColorVertex
{
	f32 m_x;
    f32 m_y;
    f32 m_z;
    u32 m_abgr;

    static void init() {
        s_layout.begin().
            add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).
            add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true).
            end();
    }

    static bgfx::VertexLayout s_layout;
};
