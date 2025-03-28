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
        Vector2f pos; // position of the square at the top left corner
        float size;
        Color color;

        inline bool in(float x, float y) const
        {
                return (x >= pos.x && x <= pos.x + size && y >= pos.y && y <= pos.y + size);
        }
};

#endif
