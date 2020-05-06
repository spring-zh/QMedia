//
//  ColorTypes.cpp
//  QMediaCore
//
//  Created by spring on 2019/2/24.
//  Copyright © 2019 QMedia. All rights reserved.
//

#include "ColorTypes.h"

GRAPHICCORE_BEGIN

//const ssize_t CC_INVALID_INDEX = -1;

/**
 * Color3B
 */

Color3B::Color3B()
: r(0)
, g(0)
, b(0)
{}

Color3B::Color3B(Ubyte _r, Ubyte _g, Ubyte _b)
: r(_r)
, g(_g)
, b(_b)
{}

Color3B::Color3B(const Color4B& color)
: r(color.r)
, g(color.g)
, b(color.b)
{}

Color3B::Color3B(const Color4F& color)
: r(color.r * 255.0f)
, g(color.g * 255.0f)
, b(color.b * 255.0f)
{}

bool Color3B::operator==(const Color3B& right) const
{
    return (r == right.r && g == right.g && b == right.b);
}

bool Color3B::operator==(const Color4B& right) const
{
    return (r == right.r && g == right.g && b == right.b && 255 == right.a);
}

bool Color3B::operator==(const Color4F& right) const
{
    return (right.a == 1.0f && Color4F(*this) == right);
}

bool Color3B::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color3B::operator!=(const Color4B& right) const
{
    return !(*this == right);
}

bool Color3B::operator!=(const Color4F& right) const
{
    return !(*this == right);
}

/**
 * Color4B
 */

Color4B::Color4B()
: r(0)
, g(0)
, b(0)
, a(0)
{}

Color4B::Color4B(Ubyte _r, Ubyte _g, Ubyte _b, Ubyte _a)
: r(_r)
, g(_g)
, b(_b)
, a(_a)
{}

Color4B::Color4B(const Color3B& color, Ubyte _a)
: r(color.r)
, g(color.g)
, b(color.b)
, a(_a)
{}

Color4B::Color4B(const Color4F& color)
: r(color.r * 255)
, g(color.g * 255)
, b(color.b * 255)
, a(color.a * 255)
{}

bool Color4B::operator==(const Color4B& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == right.a);
}

bool Color4B::operator==(const Color3B& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == 255);
}

bool Color4B::operator==(const Color4F& right) const
{
    return (*this == Color4B(right));
}

bool Color4B::operator!=(const Color4B& right) const
{
    return !(*this == right);
}

bool Color4B::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color4B::operator!=(const Color4F& right) const
{
    return !(*this == right);
}

/**
 * Color4F
 */

Color4F::Color4F()
: r(0.0f)
, g(0.0f)
, b(0.0f)
, a(0.0f)
{}

Color4F::Color4F(float _r, float _g, float _b, float _a)
: r(_r)
, g(_g)
, b(_b)
, a(_a)
{}

Color4F::Color4F(const Color3B& color, float _a)
: r(color.r / 255.0f)
, g(color.g / 255.0f)
, b(color.b / 255.0f)
, a(_a)
{}

Color4F::Color4F(const Color4B& color)
: r(color.r / 255.0f)
, g(color.g / 255.0f)
, b(color.b / 255.0f)
, a(color.a / 255.0f)
{}

bool Color4F::operator==(const Color4F& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == right.a);
}

bool Color4F::operator==(const Color3B& right) const
{
    return (a == 1.0f && Color3B(*this) == right);
}

bool Color4F::operator==(const Color4B& right) const
{
    return (*this == Color4F(right));
}

bool Color4F::operator!=(const Color4F& right) const
{
    return !(*this == right);
}

bool Color4F::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color4F::operator!=(const Color4B& right) const
{
    return !(*this == right);
}

Color4F& operator+=(Color4F& lhs, const Color4F& rhs) {
    lhs.r += rhs.r;
    lhs.g += rhs.g;
    lhs.b += rhs.b;
    lhs.a += rhs.a;
    return lhs;
}
Color4F operator+(Color4F lhs, const Color4F& rhs) {
    return lhs += rhs;
}
Color4F& operator-=(Color4F& lhs, const Color4F& rhs) {
    lhs.r -= rhs.r;
    lhs.g -= rhs.g;
    lhs.b -= rhs.b;
    lhs.a -= rhs.a;
    return lhs;
}
Color4F operator-(Color4F lhs, const Color4F& rhs) {
    return lhs -= rhs;
}

Color4F& operator*=(Color4F& lhs, const Color4F& rhs) {
    lhs.r *= rhs.r;
    lhs.g *= rhs.g;
    lhs.b *= rhs.b;
    lhs.a *= rhs.a;
    return lhs;
}
Color4F& operator*=(Color4F& lhs, float rhs) {
    lhs.r *= rhs;
    lhs.g *= rhs;
    lhs.b *= rhs;
    lhs.a *= rhs;
    return lhs;
}
Color4F operator*(Color4F lhs, const Color4F& rhs) {
    return lhs *= rhs;
}

Color4F operator*(Color4F lhs, float rhs) {
    return lhs *= rhs;
}

Color4F& operator/=(Color4F& lhs, const Color4F& rhs) {
    lhs.r /= rhs.r;
    lhs.g /= rhs.g;
    lhs.b /= rhs.b;
    lhs.a /= rhs.a;
    return lhs;
}
Color4F& operator/=(Color4F& lhs, float rhs) {
    lhs.r /= rhs;
    lhs.g /= rhs;
    lhs.b /= rhs;
    lhs.a /= rhs;
    return lhs;
}
Color4F operator/(Color4F lhs, const Color4F& rhs) {
    return lhs /= rhs;
}

Color4F operator/(Color4F lhs, float rhs) {
    return lhs /= rhs;
}

/**
 * Color constants
 */

const Color3B Color3B::WHITE  (255, 255, 255);
const Color3B Color3B::YELLOW (255, 255,   0);
const Color3B Color3B::GREEN  (  0, 255,   0);
const Color3B Color3B::BLUE   (  0,   0, 255);
const Color3B Color3B::RED    (255,   0,   0);
const Color3B Color3B::MAGENTA(255,   0, 255);
const Color3B Color3B::BLACK  (  0,   0,   0);
const Color3B Color3B::ORANGE (255, 127,   0);
const Color3B Color3B::GRAY   (166, 166, 166);

const Color4B Color4B::WHITE  (255, 255, 255, 255);
const Color4B Color4B::YELLOW (255, 255,   0, 255);
const Color4B Color4B::GREEN  (  0, 255,   0, 255);
const Color4B Color4B::BLUE   (  0,   0, 255, 255);
const Color4B Color4B::RED    (255,   0,   0, 255);
const Color4B Color4B::MAGENTA(255,   0, 255, 255);
const Color4B Color4B::BLACK  (  0,   0,   0, 255);
const Color4B Color4B::ORANGE (255, 127,   0, 255);
const Color4B Color4B::GRAY   (166, 166, 166, 255);

const Color4F Color4F::WHITE  (    1,     1,     1, 1);
const Color4F Color4F::YELLOW (    1,     1,     0, 1);
const Color4F Color4F::GREEN  (    0,     1,     0, 1);
const Color4F Color4F::BLUE   (    0,     0,     1, 1);
const Color4F Color4F::RED    (    1,     0,     0, 1);
const Color4F Color4F::MAGENTA(    1,     0,     1, 1);
const Color4F Color4F::BLACK  (    0,     0,     0, 1);
const Color4F Color4F::ORANGE (    1,  0.5f,     0, 1);
const Color4F Color4F::GRAY   (0.65f, 0.65f, 0.65f, 1);

const BlendFunc BlendFunc::DISABLE = {GL_ONE, GL_ZERO};
const BlendFunc BlendFunc::ALPHA_PREMULTIPLIED = {GL_ONE, GL_ONE_MINUS_SRC_ALPHA};
const BlendFunc BlendFunc::ALPHA_NON_PREMULTIPLIED = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
const BlendFunc BlendFunc::ADDITIVE = {GL_SRC_ALPHA, GL_ONE};

GRAPHICCORE_END
