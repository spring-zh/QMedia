//
//  FlashEffect.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EffectGroup.h"

GRAPHICCORE_BEGIN

EffectGroup::EffectGroup() : _framebuffer(nullptr){
    _textureCache[0] = nullptr;
    _textureCache[1] = nullptr;
}

void EffectGroup::addEffect(EffectRef effectRef) {
    _effects.push_back(effectRef);
}
void EffectGroup::removeEffect(EffectRef effectRef) {
    std::remove(_effects.begin(), _effects.end(), effectRef);
}

void EffectGroup::release() {
    _framebuffer.reset();
    _textureCache[0].reset();
    _textureCache[1].reset();
}

const Texture2D * EffectGroup::drawEffects(int64_t duration, GraphicCore::GLEngine* gle, const Texture2D *inputTexture){
    int idx = 0;
    const Texture2D* current = inputTexture;
    gle->saveStatus();
    if (_framebuffer == nullptr) {
        _framebuffer = std::shared_ptr<FrameBuffer>(FrameBuffer::createNew());
    }
    for (auto& effect : _effects) {
        if (effect->_timeRange.isContain(duration)) {
            if(_textureCache[idx] == nullptr) {
                _textureCache[idx] = std::shared_ptr<Texture2D>(GeneralTexture2D::createTexture(Texture2D::RGBA, inputTexture->width(), inputTexture->height()));
            }
            const Texture2D* next = _textureCache[idx].get();
            gle->setCurrentFrameBuffer(_framebuffer.get());
            _framebuffer->attachTexture2D(FrameBuffer::COLOR, next);
            
            Rect layerViewPort(0, 0, next->width(), next->height());
            gle->setViewPort(layerViewPort);
            effect->drawEffect(duration - effect->_timeRange._start, current, gle);
            
            current = next;
            idx = (idx + 1) & 1;
        }
    }
    gle->recoverStatus();
    return current;
}

GRAPHICCORE_END
