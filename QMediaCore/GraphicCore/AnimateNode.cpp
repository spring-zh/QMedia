//
//  AnimateNode.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "AnimateNode.h"

namespace QMedia { namespace Api {

int64_t GetTimeLength(MediaRange range) { return range.end - range.start;}

#ifndef M_PI_X_2
#define M_PI_X_2 (float)M_PI * 2.0f
#endif
// Linear
static float linear(float time)
{
    return time;
}

// Sine Ease
static float sineEaseIn(float time)
{
    return -1 * cosf(time * (float)M_PI_2) + 1;
}

static float sineEaseOut(float time)
{
    return sinf(time * (float)M_PI_2);
}

static float sineEaseInOut(float time)
{
    return -0.5f * (cosf((float)M_PI * time) - 1);
}


// Quad Ease
static float quadEaseIn(float time)
{
    return time * time;
}

static float quadEaseOut(float time)
{
    return -1 * time * (time - 2);
}

static float quadEaseInOut(float time)
{
    time = time*2;
    if (time < 1)
        return 0.5f * time * time;
    --time;
    return -0.5f * (time * (time - 2) - 1);
}



// Cubic Ease
static float cubicEaseIn(float time)
{
    return time * time * time;
}
static float cubicEaseOut(float time)
{
    time -= 1;
    return (time * time * time + 1);
}
static float cubicEaseInOut(float time)
{
    time = time*2;
    if (time < 1)
        return 0.5f * time * time * time;
    time -= 2;
    return 0.5f * (time * time * time + 2);
}


// Quart Ease
static float quartEaseIn(float time)
{
    return time * time * time * time;
}

static float quartEaseOut(float time)
{
    time -= 1;
    return -(time * time * time * time - 1);
}

static float quartEaseInOut(float time)
{
    time = time*2;
    if (time < 1)
        return 0.5f * time * time * time * time;
    time -= 2;
    return -0.5f * (time * time * time * time - 2);
}


// Quint Ease
static float quintEaseIn(float time)
{
    return time * time * time * time * time;
}

static float quintEaseOut(float time)
{
    time -=1;
    return (time * time * time * time * time + 1);
}

static float quintEaseInOut(float time)
{
    time = time*2;
    if (time < 1)
        return 0.5f * time * time * time * time * time;
    time -= 2;
    return 0.5f * (time * time * time * time * time + 2);
}


// Expo Ease
static float expoEaseIn(float time)
{
    return time == 0 ? 0 : powf(2, 10 * (time/1 - 1)) - 1 * 0.001f;
}
static float expoEaseOut(float time)
{
    return time == 1 ? 1 : (-powf(2, -10 * time / 1) + 1);
}
static float expoEaseInOut(float time)
{
    if(time == 0 || time == 1)
        return time;
    
    if (time < 0.5f)
        return 0.5f * powf(2, 10 * (time * 2 - 1));
    
    return 0.5f * (-powf(2, -10 * (time * 2 - 1)) + 2);
}


// Circ Ease
static float circEaseIn(float time)
{
    return -1 * (sqrt(1 - time * time) - 1);
}
static float circEaseOut(float time)
{
    time = time - 1;
    return sqrt(1 - time * time);
}
static float circEaseInOut(float time)
{
    time = time * 2;
    if (time < 1)
        return -0.5f * (sqrt(1 - time * time) - 1);
    time -= 2;
    return 0.5f * (sqrt(1 - time * time) + 1);
}


// Elastic Ease
static float elasticEaseIn(float time, float period)
{
    
    float newT = 0;
    if (time == 0 || time == 1)
    {
        newT = time;
    }
    else
    {
        float s = period / 4;
        time = time - 1;
        newT = -powf(2, 10 * time) * sinf((time - s) * M_PI_X_2 / period);
    }
    
    return newT;
}
static float elasticEaseOut(float time, float period)
{
    
    float newT = 0;
    if (time == 0 || time == 1)
    {
        newT = time;
    }
    else
    {
        float s = period / 4;
        newT = powf(2, -10 * time) * sinf((time - s) * M_PI_X_2 / period) + 1;
    }
    
    return newT;
}
static float elasticEaseInOut(float time, float period)
{
    
    float newT = 0;
    if (time == 0 || time == 1)
    {
        newT = time;
    }
    else
    {
        time = time * 2;
        if (! period)
        {
            period = 0.3f * 1.5f;
        }
        
        float s = period / 4;
        
        time = time - 1;
        if (time < 0)
        {
            newT = -0.5f * powf(2, 10 * time) * sinf((time -s) * M_PI_X_2 / period);
        }
        else
        {
            newT = powf(2, -10 * time) * sinf((time - s) * M_PI_X_2 / period) * 0.5f + 1;
        }
    }
    return newT;
}


// Back Ease
static float backEaseIn(float time)
{
    float overshoot = 1.70158f;
    return time * time * ((overshoot + 1) * time - overshoot);
}
static float backEaseOut(float time)
{
    float overshoot = 1.70158f;
    
    time = time - 1;
    return time * time * ((overshoot + 1) * time + overshoot) + 1;
}
static float backEaseInOut(float time)
{
    float overshoot = 1.70158f * 1.525f;
    
    time = time * 2;
    if (time < 1)
    {
        return (time * time * ((overshoot + 1) * time - overshoot)) / 2;
    }
    else
    {
        time = time - 2;
        return (time * time * ((overshoot + 1) * time + overshoot)) / 2 + 1;
    }
}



// Bounce Ease
static float bounceTime(float time)
{
    if (time < 1 / 2.75f)
    {
        return 7.5625f * time * time;
    }
    else if (time < 2 / 2.75f)
    {
        time -= 1.5f / 2.75f;
        return 7.5625f * time * time + 0.75f;
    }
    else if(time < 2.5f / 2.75f)
    {
        time -= 2.25f / 2.75f;
        return 7.5625f * time * time + 0.9375f;
    }
    
    time -= 2.625f / 2.75f;
    return 7.5625f * time * time + 0.984375f;
}
static float bounceEaseIn(float time)
{
    return 1 - bounceTime(1 - time);
}

static float bounceEaseOut(float time)
{
    return bounceTime(time);
}

static float bounceEaseInOut(float time)
{
    float newT = 0;
    if (time < 0.5f)
    {
        time = time * 2;
        newT = (1 - bounceTime(1 - time)) * 0.5f;
    }
    else
    {
        newT = bounceTime(time * 2 - 1) * 0.5f + 0.5f;
    }
    
    return newT;
}


// Custom Ease
static float customEase(float time, float *easingParam)
{
    if (easingParam)
    {
        float tt = 1-time;
        return easingParam[1]*tt*tt*tt + 3*easingParam[3]*time*tt*tt + 3*easingParam[5]*time*time*tt + easingParam[7]*time*time*time;
    }
    return time;
}

static float easeIn(float time, float rate)
{
    return powf(time, rate);
}

static float easeOut(float time, float rate)
{
    return powf(time, 1 / rate);
}

static float easeInOut(float time, float rate)
{
    time *= 2;
    if (time < 1)
    {
        return 0.5f * powf(time, rate);
    }
    else
    {
        return (1.0f - 0.5f * powf(2 - time, rate));
    }
}

static float quadraticIn(float time)
{
    return   powf(time,2);
}

static float quadraticOut(float time)
{
    return -time*(time-2);
}

static float quadraticInOut(float time)
{
    
    float resultTime = time;
    time = time*2;
    if (time < 1)
    {
        resultTime = time * time * 0.5f;
    }
    else
    {
        --time;
        resultTime = -0.5f * (time * (time - 2) - 1);
    }
    return resultTime;
}

static float bezieratFunction( float a, float b, float c, float d, float t )
{
    return (powf(1-t,3) * a + 3*t*(powf(1-t,2))*b + 3*powf(t,2)*(1-t)*c + powf(t,3)*d );
}

static ease_function EaseMap[(int)EaseFunction::Bounce_EaseInOut + 1] = {
    linear,
    sineEaseIn,
    sineEaseOut,
    sineEaseInOut,
    quadEaseIn,
    quadEaseOut,
    quadEaseInOut,
    cubicEaseIn,
    cubicEaseOut,
    cubicEaseInOut,
    quartEaseIn,
    quartEaseOut,
    quartEaseInOut,
    quintEaseIn,
    quintEaseOut,
    quintEaseInOut,
    expoEaseIn,
    expoEaseOut,
    expoEaseInOut,
    circEaseIn,
    circEaseOut,
    circEaseInOut,
    [](float t)->float{return elasticEaseIn(t,0.3f);},
    [](float t)->float{return elasticEaseOut(t,0.3f);},
    [](float t)->float{return elasticEaseInOut(t,0.3f);},
    backEaseIn,
    backEaseOut,
    backEaseInOut,
//    bounceTime,
    bounceEaseIn,
    bounceEaseOut,
    bounceEaseInOut,
};

template <typename T>
T calculateTweenValue(const T start,const T end, float t, float (*tweenfunction)(float)){
    float tw = (*tweenfunction)(t);
    return (1-tw) * start + tw * end;
}

template <>
Vec4f calculateTweenValue(const Vec4f start,const Vec4f end, float t, float (*tweenfunction)(float)){
    Vec4f ret;
    float tw = (*tweenfunction)(t);
    ret.v1 = (1-tw) * start.v1 + tw * end.v1;
    ret.v2 = (1-tw) * start.v2 + tw * end.v2;
    ret.v3 = (1-tw) * start.v3 + tw * end.v3;
    ret.v4 = (1-tw) * start.v4 + tw * end.v4;
    return ret;
}

#pragma mark -----------  AnimateNode -----------

class UpdateFuncMap {
public:
    static UpdateFuncMap* instance();
    
    update_func GetFunc(std::string property) { return update_func_map_[property]; }
private:
    UpdateFuncMap() {
        update_func_map_[Animator::POSITIONXYZ] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            node->setPosition(Point(value.v1, value.v2));
            node->setPositionZ(value.v3);
        };
        
        update_func_map_[Animator::ROTATION] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            node->setRotation(value.v1);
        };
        
        update_func_map_[Animator::ROTATIONXYZ] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            node->setRotation3d(Vec3f(value.v1, value.v2, value.v3));
        };
        
        update_func_map_[Animator::SCALEX] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            node->setScaleX(value.v1);
        };
        update_func_map_[Animator::SCALEY] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            node->setScaleY(value.v1);
        };
        update_func_map_[Animator::SCALEZ] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            node->setScaleZ(value.v1);
        };
        update_func_map_[Animator::SCALEXY] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            node->setScaleX(value.v1);
            node->setScaleY(value.v2);
        };
        update_func_map_[Animator::SCALEXYZ] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            node->setScaleX(value.v1);
            node->setScaleY(value.v2);
            node->setScaleZ(value.v3);
        };
        
        update_func_map_[Animator::CONTENTSIZEW] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            Size cs = node->getContentSize();
            cs.width = value.v1;
            node->setContentSize(cs);
        };
        update_func_map_[Animator::CONTENTSIZEH] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            Size cs = node->getContentSize();
            cs.height = value.v1;
            node->setContentSize(cs);
        };
        
        update_func_map_[Animator::CONTENTSIZE] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            node->setContentSize(Size(value.v1, value.v2));
        };
        
        update_func_map_[Animator::ALPHA] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            node->setAlpha(value.v1);
        };
        
        update_func_map_[Animator::MIXCOLOR] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            node->setColor4F(Color4f(value.v1, value.v2, value.v3, value.v4));
        };
        
        update_func_map_[Animator::CROP] = [](BaseNode* node, float t, const Animator& animator, ease_function ef){
            Vec4f value = calculateTweenValue(animator.startpoint,animator.endpoint, t, ef);
            node->setCrop(Rect4f(value.v1, value.v2, value.v3, value.v4));
        };
    }
    
    std::unordered_map<std::string, update_func> update_func_map_;
};

UpdateFuncMap* UpdateFuncMap::instance() {
    static UpdateFuncMap funcmap;
    return &funcmap;
}

#pragma mark ----------- AnimateNode --------------

AnimateNode::AnimateNode() {
    
}

std::vector<Animator> AnimateNode::getAnimators() {
    std::unique_lock<std::mutex> lck(mutex_);
    std::vector<Animator> animators;
    for (auto& ab : animator_binds_) {
        animators.push_back(std::get<0>(ab));
    }
    return animators;
}

bool AnimateNode::addAnimator(const Animator & animator) {
    std::unique_lock<std::mutex> lck(mutex_);
    animator_binds_.push_back(std::make_tuple(animator, UpdateFuncMap::instance()->GetFunc(animator.property), -1));
    return true;
}

bool AnimateNode::removeAnimator(const Animator & animator) {
    return false;
}

void AnimateNode::updateNodeByAnimations(int64_t time_ms) {
    std::unique_lock<std::mutex> lock(mutex_);
    for (auto animator_bind : animator_binds_) {
        auto& animator = std::get<0>(animator_bind);
        MediaRange time_range = animator.timerang;
        if (time_ms > time_range.end) {
            time_ms = time_range.end;
        } else if (time_ms < time_range.start)
            time_ms = time_range.start;
        
        if (time_ms != std::get<2>(animator_bind)) {
            float t = (float)(time_ms - time_range.start)/GetTimeLength(time_range);
            (*std::get<1>(animator_bind))(this, t, animator, EaseMap[(int)animator.easefunc]);
            std::get<2>(animator_bind) = time_ms;
        }
    }
}

}
}
