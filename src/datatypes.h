#ifndef DATATYPES_HPP
#define DATATYPES_HPP

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

#endif
