//
//  Drawable2D.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "Drawable2D.h"

namespace QMedia {
namespace RenderEngine {

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
float Drawable2D::RECTANGLE_TEX_COORDS90[] =
{
    0.0f, 1.0f,     // 0 bottom left
    0.0f, 0.0f,     // 1 bottom right
    1.0f, 1.0f,     // 2 top left
    1.0f, 0.0f      // 3 top right
};
float Drawable2D::RECTANGLE_TEX_COORDS180[] =
{
    1.0f, 1.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
};
float Drawable2D::RECTANGLE_TEX_COORDS270[] =
{
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
};

float Drawable2D::RECTANGLE_TEX_COORDS_FLIPH[8]
{
    1.0f, 0.0f,     // 0 bottom left
    0.0f, 0.0f,     // 1 bottom right
    1.0f, 1.0f,     // 2 top left
    0.0f, 1.0f      // 3 top right
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

const float* Drawable2D::GetTexVertex(int rotation) {
    switch (rotation) {
    case 90:
        return Drawable2D::RECTANGLE_TEX_COORDS90;
        break;
    case 180:
        return Drawable2D::RECTANGLE_TEX_COORDS180;
        break;
    case 270:
        return Drawable2D::RECTANGLE_TEX_COORDS270;
        break;
    default:
        return Drawable2D::RECTANGLE_TEX_COORDS;
    }
}

}
}
