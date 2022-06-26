//
//  ColorTypes.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef RENDERENGINE_COLORTYPES_H
#define RENDERENGINE_COLORTYPES_H

#include "RenderEngine/opengl/gl_platform.h"
#include "RenderEngine/math/CCGeometry.h"

namespace QMedia {
namespace RenderEngine {

struct Color4B;
struct Color4F;

typedef uint8_t Ubyte;
typedef float  Float;
/**
 * RGB color composed of bytes 3 bytes.
 * @since v3.0
 */
struct Color3B
{
    Color3B();
    Color3B(Ubyte _r, Ubyte _g, Ubyte _b);
    explicit Color3B(const Color4B& color);
    explicit Color3B(const Color4F& color);
    
    bool operator==(const Color3B& right) const;
    bool operator==(const Color4B& right) const;
    bool operator==(const Color4F& right) const;
    bool operator!=(const Color3B& right) const;
    bool operator!=(const Color4B& right) const;
    bool operator!=(const Color4F& right) const;
    
    bool equals(const Color3B& other) const
    {
        return (*this == other);
    }
    
    Ubyte r;
    Ubyte g;
    Ubyte b;
    
    static const Color3B WHITE;
    static const Color3B YELLOW;
    static const Color3B BLUE;
    static const Color3B GREEN;
    static const Color3B RED;
    static const Color3B MAGENTA;
    static const Color3B BLACK;
    static const Color3B ORANGE;
    static const Color3B GRAY;
};

/**
 * RGBA color composed of 4 bytes.
 * @since v3.0
 */
struct Color4B
{
    Color4B();
    Color4B(Ubyte _r, Ubyte _g, Ubyte _b, Ubyte _a);
    explicit Color4B(const Color3B& color, Ubyte _a = 255);
    explicit Color4B(const Color4F& color);
    
    inline void set(Ubyte _r, Ubyte _g, Ubyte _b, Ubyte _a)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }
    
    bool operator==(const Color4B& right) const;
    bool operator==(const Color3B& right) const;
    bool operator==(const Color4F& right) const;
    bool operator!=(const Color4B& right) const;
    bool operator!=(const Color3B& right) const;
    bool operator!=(const Color4F& right) const;
    
    Ubyte r;
    Ubyte g;
    Ubyte b;
    Ubyte a;
    
    static const Color4B WHITE;
    static const Color4B YELLOW;
    static const Color4B BLUE;
    static const Color4B GREEN;
    static const Color4B RED;
    static const Color4B MAGENTA;
    static const Color4B BLACK;
    static const Color4B ORANGE;
    static const Color4B GRAY;
};


/**
 * RGBA color composed of 4 floats.
 * @since v3.0
 */
struct Color4F
{
    Color4F();
    Color4F(Float _r, Float _g, Float _b, Float _a);
    explicit Color4F(const Color3B& color, float _a = 1.0f);
    explicit Color4F(const Color4B& color);
    
    bool operator==(const Color4F& right) const;
    bool operator==(const Color3B& right) const;
    bool operator==(const Color4B& right) const;
    bool operator!=(const Color4F& right) const;
    bool operator!=(const Color3B& right) const;
    bool operator!=(const Color4B& right) const;
    
    bool equals(const Color4F &other) const
    {
        return (*this == other);
    }
    
    Float r;
    Float g;
    Float b;
    Float a;
    
    static const Color4F WHITE;
    static const Color4F YELLOW;
    static const Color4F BLUE;
    static const Color4F GREEN;
    static const Color4F RED;
    static const Color4F MAGENTA;
    static const Color4F BLACK;
    static const Color4F ORANGE;
    static const Color4F GRAY;
};

Color4F& operator+=(Color4F& lhs, const Color4F& rhs);
Color4F operator+(Color4F lhs, const Color4F& rhs);

Color4F& operator-=(Color4F& lhs, const Color4F& rhs);
Color4F operator-(Color4F lhs, const Color4F& rhs);

Color4F& operator*=(Color4F& lhs, const Color4F& rhs);
Color4F operator*(Color4F lhs, const Color4F& rhs);
Color4F& operator*=(Color4F& lhs, float rhs);
Color4F operator*(Color4F lhs, float rhs);

Color4F& operator/=(Color4F& lhs, const Color4F& rhs);
Color4F operator/(Color4F lhs, const Color4F& rhs);
Color4F& operator/=(Color4F& lhs, float rhs);
Color4F operator/(Color4F lhs, float rhs);


/** A vertex composed of 2 floats: x, y
 @since v3.0
 */
// struct Vertex2F
// {
//     Vertex2F(float _x, float _y) :x(_x), y(_y) {}

//     Vertex2F(): x(0.f), y(0.f) {}

//     GLfloat x;
//     GLfloat y;
// };


/** A vertex composed of 2 floats: x, y
 @since v3.0
 */
// struct Vertex3F
// {
//     Vertex3F(float _x, float _y, float _z)
//         : x(_x)
//         , y(_y)
//         , z(_z)
//     {}

//     Vertex3F(): x(0.f), y(0.f), z(0.f) {}

//     GLfloat x;
//     GLfloat y;
//     GLfloat z;
// };

/** @struct Tex2F
 * A TEXCOORD composed of 2 floats: u, y
 * @since v3.0
 */
struct Tex2F {
    Tex2F(float _u, float _v): u(_u), v(_v) {}
    
    Tex2F(): u(0.f), v(0.f) {}
    
    Float u;
    Float v;
};

/** @struct Quad2
 * A 2D Quad. 4 * 2 floats.
 */
struct Quad2
{
    Vec2        tl;
    Vec2        tr;
    Vec2        bl;
    Vec2        br;
};

/** @struct Quad3
 * A 3D Quad. 4 * 3 floats.
 */
struct Quad3 {
    Vec3        bl;
    Vec3        br;
    Vec3        tl;
    Vec3        tr;
};

/** @struct V2F_C4B_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4B.
 */
struct V2F_C4B_T2F
{
    /// vertices (2F)
    Vec2       vertices;
    /// colors (4B)
    Color4B        colors;
    /// tex coords (2F)
    Tex2F          texCoords;
};

/** @struct V2F_C4B_PF
 *
 */
struct V2F_C4B_PF
{
    /// vertices (2F)
    Vec2       vertices;
    /// colors (4B)
    Color4B        colors;
    /// pointsize
    float      pointSize;
};

/** @struct V2F_C4F_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4F.
 */
struct V2F_C4F_T2F
{
    /// vertices (2F)
    Vec2       vertices;
    /// colors (4F)
    Color4F        colors;
    /// tex coords (2F)
    Tex2F          texCoords;
};

/** @struct V3F_C4B_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4B.
 */
struct V3F_C4B_T2F
{
    /// vertices (3F)
    Vec3     vertices;            // 12 bytes
    
    /// colors (4B)
    Color4B      colors;              // 4 bytes
    
    // tex coords (2F)
    Tex2F        texCoords;           // 8 bytes
};

/** @struct V3F_T2F
 * A Vec2 with a vertex point, a tex coord point.
 */
struct V3F_T2F
{
    /// vertices (2F)
    Vec3       vertices;
    /// tex coords (2F)
    Tex2F          texCoords;
};

/** @struct V2F_C4B_T2F_Triangle
 * A Triangle of V2F_C4B_T2F.
 */
struct V2F_C4B_T2F_Triangle
{
    V2F_C4B_T2F a;
    V2F_C4B_T2F b;
    V2F_C4B_T2F c;
};

/** @struct V2F_C4B_T2F_Quad
 * A Quad of V2F_C4B_T2F.
 */
struct V2F_C4B_T2F_Quad
{
    /// bottom left
    V2F_C4B_T2F    bl;
    /// bottom right
    V2F_C4B_T2F    br;
    /// top left
    V2F_C4B_T2F    tl;
    /// top right
    V2F_C4B_T2F    tr;
};

/** @struct V3F_C4B_T2F_Quad
 * 4 Vertex3FTex2FColor4B.
 */
struct V3F_C4B_T2F_Quad
{
    /// top left
    V3F_C4B_T2F    tl;
    /// bottom left
    V3F_C4B_T2F    bl;
    /// top right
    V3F_C4B_T2F    tr;
    /// bottom right
    V3F_C4B_T2F    br;
};

/** @struct V2F_C4F_T2F_Quad
 * 4 Vertex2FTex2FColor4F Quad.
 */
struct V2F_C4F_T2F_Quad
{
    /// bottom left
    V2F_C4F_T2F    bl;
    /// bottom right
    V2F_C4F_T2F    br;
    /// top left
    V2F_C4F_T2F    tl;
    /// top right
    V2F_C4F_T2F    tr;
};

/** @struct V3F_T2F_Quad
 *
 */
struct V3F_T2F_Quad
{
    /// bottom left
    V3F_T2F    bl;
    /// bottom right
    V3F_T2F    br;
    /// top left
    V3F_T2F    tl;
    /// top right
    V3F_T2F    tr;
};

}//End RenderEngine
}//End QMedia

#endif /* RENDERENGINE_COLORTYPES_H */
