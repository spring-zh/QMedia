//
//  EffectGroup.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_EFFECTGROUP_H
#define GRAPHICCORE_EFFECTGROUP_H

#include <vector>
#include "Utils/Comm.h"
#include "Effect.h"

namespace QMedia {
namespace RenderEngine {

class EffectGroup {
public:
    EffectGroup();
    ~EffectGroup() {};
    
    void addEffect(EffectRef effectRef);
    void removeEffect(EffectRef effectRef);
    
    bool has_effect() { return _effects.size() > 0;}
    
    void release();
    
    const Texture2D * drawEffects(int64_t duration, RenderDeviceGL* gle, const Texture2D *inputTexture );
    
private:
    std::shared_ptr<Texture2D> _textureCache[2];
    std::shared_ptr<FrameBufferGL> _framebuffer;
    std::vector<EffectRef> _effects;
};

}//End RenderEngine
}//End QMedia

#endif /* GRAPHICCORE_EFFECTGROUP_H */
