//
//  Drawable2D.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef RENDERENGINE_DRAWABLETEXTURE_H
#define RENDERENGINE_DRAWABLETEXTURE_H

#include "gl_platform.h"

namespace QMedia {
namespace RenderEngine {

class Drawable2D {
public:
    
    enum FlipMode : int {
        NONE = 0,
        FlipH = 1,
        FlipV = 2
    };
    
    static float RECTANGLE_COORDS[8];
    static float RECTANGLE_TEX_COORDS[8];
    static float RECTANGLE_TEX_COORDS90[8];
    static float RECTANGLE_TEX_COORDS180[8];
    static float RECTANGLE_TEX_COORDS270[8];
    static float RECTANGLE_TEX_COORDS_FLIPH[8];
    static float RECTANGLE_TEX_COORDS_FLIPV[8];

    static float MtxFlipH[16];
    static float MtxFlipV[16];

    static const float* GetTexVertex(int rotation);

};

}//End RenderEngine
}//End QMedia

#endif //RENDERENGINE_DRAWABLETEXTURE_H
