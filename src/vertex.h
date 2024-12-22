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

// A simple structure to store vertex data
struct PosNormalTexVertex {
    float m_position[3];
    float m_normal[3];
    float m_texCoords[2];

    static void init() {
        s_layout.begin().
            add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).
            add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float).
            add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float).
            end();
    }

    static bgfx::VertexLayout s_layout;
};
