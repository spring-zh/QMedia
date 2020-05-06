//
//  Drawable2D.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "Drawable2D.h"

GRAPHICCORE_BEGIN

float Drawable2D::RECTANGLE_COORDS[8]                =
{
    -1.0f, -1.0f,   // 0 bottom left
    1.0f, -1.0f,   // 1 bottom right
    -1.0f,  1.0f,   // 2 top left
    1.0f,  1.0f,   // 3 top right
//        -1.0f, -0.0f,   // 0 bottom left
//        0.0f, -0.0f,   // 1 bottom right
//        -1.0f,  1.0f,   // 2 top left
//        0.0f,  1.0f,   // 3 top right
};

float Drawable2D::RECTANGLE_TEX_COORDS[8]            =
{
//    0.0f, 1.0f,     // 0 bottom left
//    1.0f, 1.0f,     // 1 bottom right
//    0.0f, 0.0f,     // 2 top left
//    1.0f, 0.0f      // 3 top right
    
        0.0f, 0.0f,     // 0 bottom left
        1.0f, 0.0f,     // 1 bottom right
        0.0f, 1.0f,     // 2 top left
        1.0f, 1.0f      // 3 top right
};

float Drawable2D::RECTANGLE_TEX_COORDS_FLIPV[8]
{
    0.0f, 1.0f,     // 0 bottom left
    1.0f, 1.0f,     // 1 bottom right
    0.0f, 0.0f,     // 2 top left
    1.0f, 0.0f      // 3 top right
};

float Drawable2D::MtxFlipH[16] =
{
    -1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    1, 0, 0, 1,
};

float Drawable2D::MtxFlipV[16] =
{
    1, 0, 0, 0,
    0, -1, 0, 0,
    0, 0, 1, 0,
    0, 1, 0, 1,
};

GRAPHICCORE_END
