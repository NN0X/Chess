#ifndef DATATYPES_HPP
#define DATATYPES_HPP

#include <array>

struct Color
{
        float r, g, b;
};

struct Vector2f
{
        float x, y;
};

struct Square
{
        Vector2f pos;
        float size;
        Color color;
};

struct Board
{
        std::array<Square, 64> squares;
};

#endif
