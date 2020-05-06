//
//  GcAnimaNode.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GcAnimaNode.h"
#include <math.h>

GRAPHICCORE_BEGIN

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

ease_function getTweenFunctionByType(EaseFunctionType functionType)
{
    ease_function func = &linear;
    switch (functionType) {
        case EaseFunctionType::Linear:
            func = &linear;
            break;
        case EaseFunctionType::Sine_EaseIn:
            func = &sineEaseIn;
            break;
        case EaseFunctionType::Sine_EaseOut:
            func = &sineEaseOut;
            break;
        case EaseFunctionType::Sine_EaseInOut:
            func = &sineEaseInOut;
            break;
        case EaseFunctionType::Quad_EaseIn:
            func = &quadEaseIn;
            break;
        case EaseFunctionType::Quad_EaseOut:
            func = &quadEaseOut;
            break;
        case EaseFunctionType::Quad_EaseInOut:
            func = &quadEaseInOut;
            break;
        case EaseFunctionType::Cubic_EaseIn:
            func = &cubicEaseIn;
            break;
        case EaseFunctionType::Cubic_EaseOut:
            func = &cubicEaseOut;
            break;
        case EaseFunctionType::Cubic_EaseInOut:
            func = &cubicEaseInOut;
            break;
        case EaseFunctionType::Quart_EaseIn:
            func = &quartEaseIn;
            break;
        case EaseFunctionType::Quart_EaseOut:
            func = &quartEaseOut;
            break;
        case EaseFunctionType::Quart_EaseInOut:
            func = &quartEaseInOut;
            break;
        case EaseFunctionType::Quint_EaseIn:
            func = &quintEaseIn;
            break;
        case EaseFunctionType::Quint_EaseOut:
            func = &quintEaseOut;
            break;
        case EaseFunctionType::Quint_EaseInOut:
            func = &quintEaseInOut;
            break;
        case EaseFunctionType::Expo_EaseIn:
            func = &expoEaseIn;
            break;
        case EaseFunctionType::Expo_EaseOut:
            func = &expoEaseOut;
            break;
        case EaseFunctionType::Expo_EaseInOut:
            func = &expoEaseInOut;
            break;
        case EaseFunctionType::Circ_EaseIn:
            func = &circEaseIn;
            break;
        case EaseFunctionType::Circ_EaseOut:
            func = &circEaseOut;
            break;
        case EaseFunctionType::Circ_EaseInOut:
            func = &circEaseInOut;
            break;
        case EaseFunctionType::Elastic_EaseIn:
            func = [](float t)->float{return elasticEaseIn(t,3);};
            break;
        case EaseFunctionType::Elastic_EaseOut:
            func = [](float t)->float{return elasticEaseOut(t,3);};
            break;
        case EaseFunctionType::Elastic_EaseInOut:
            func = [](float t)->float{return elasticEaseInOut(t,3);};
            break;
        case EaseFunctionType::Back_EaseIn:
            func = &backEaseIn;
            break;
        case EaseFunctionType::Back_EaseOut:
            func = &backEaseOut;
            break;
        case EaseFunctionType::Back_EaseInOut:
            func = &backEaseInOut;
            break;
        case EaseFunctionType::Bounce_EaseIn:
            func = &bounceEaseIn;
            break;
        case EaseFunctionType::Bounce_EaseOut:
            func = &bounceEaseOut;
            break;
        case EaseFunctionType::Bounce_EaseInOut:
            func = &bounceEaseInOut;
            break;
        default:
            break;
    }
    return func;
}

template <typename T>
T calculateTweenValue(const T& start,const T& end, float t, float (*tweenfunction)(float)){
    float tw = (*tweenfunction)(t);
    return (1-tw) * start + tw * end;
}


#pragma mark - AnimaNode

AnimaNode::AnimaNode()
{
    
}

AnimaNode::~AnimaNode(){
    
}

void AnimaNode::addAnimator(const std::string& property, Range<int> timeRang, AnimatorData beginData, AnimatorData endData, EaseFunctionType functionType, const std::string& name)
{
#define CHECK_AND_SET_PROPERTY_FUNCTION(proprety_compare) \
if (property.compare(#proprety_compare) == 0) { \
    animator->_update = std::bind(&AnimaNode::updateAnimator_##proprety_compare,this,\
                    std::placeholders::_1,std::placeholders::_2);\
    break;\
}
    
    AnimatorRef animator = AnimatorRef(new Animator());
    animator->_name = name;
    animator->_timeRang = timeRang;
    animator->_beginData = beginData;
    animator->_endData = endData;
    animator->_property = property;
    animator->_ease_function = getTweenFunctionByType(functionType);
    
    do {
        
        CHECK_AND_SET_PROPERTY_FUNCTION(positionxyz)
        
        CHECK_AND_SET_PROPERTY_FUNCTION(rotationxyz)
        
        CHECK_AND_SET_PROPERTY_FUNCTION(scalex)
        CHECK_AND_SET_PROPERTY_FUNCTION(scaley)
        CHECK_AND_SET_PROPERTY_FUNCTION(scalexy)
        CHECK_AND_SET_PROPERTY_FUNCTION(scalexyz)
        
        CHECK_AND_SET_PROPERTY_FUNCTION(contentsize)
        CHECK_AND_SET_PROPERTY_FUNCTION(contentsizew)
        CHECK_AND_SET_PROPERTY_FUNCTION(contentsizeh)
        
        CHECK_AND_SET_PROPERTY_FUNCTION(alpha)
        CHECK_AND_SET_PROPERTY_FUNCTION(mixcolor)
    }while (0);
    
    if (animator->_update != nullptr) {
        _animatorList.push_back(animator);
    }
}

void AnimaNode::updateAnimations(int timeStamp)
{
    if(!_children.empty()){
        for (auto& node : _children) {
            AnimaNode* animaNode = dynamic_cast<AnimaNode*>(node);
            if (animaNode) {
                animaNode->updateNodeProperty(timeStamp);
            }
        }
    }
    updateNodeProperty(timeStamp);
}

void AnimaNode::updateNodeProperty(int timeStamp)
{
    const int diff = 50;
    for (auto& animator : _animatorList) {
        Range<int>& timeRang = animator->_timeRang;
        if ((timeStamp >= timeRang._start - diff) &&
            (timeStamp <= timeRang._end + diff)) {
            float t = (float)(timeStamp - timeRang._start)/(timeRang._end - timeRang._start);
            if(t > 1.0f) t = 1.0f;
            else if(t < 0.0f) t = 0.0f;
            (animator->_update)(animator.get(),t);
        }
    }
}

void AnimaNode::updateAnimator_positionxyz(Animator* animator, float t)
{
    Vec3 value = calculateTweenValue(animator->_beginData._v3,animator->_endData._v3,t,animator->_ease_function);
    
    setPosition3D(value);
}

void AnimaNode::updateAnimator_rotationxyz(Animator* animator, float t)
{
    Vec3 value = calculateTweenValue(animator->_beginData._v3,animator->_endData._v3,t,animator->_ease_function);
    
    setRotation3D(value);
}

void AnimaNode::updateAnimator_scalex(Animator* animator, float t)
{
    float value = calculateTweenValue(animator->_beginData._v1,animator->_endData._v1,t,animator->_ease_function);
    
    setScaleX(value);
}
void AnimaNode::updateAnimator_scaley(Animator* animator, float t)
{
    float value = calculateTweenValue(animator->_beginData._v1,animator->_endData._v1,t,animator->_ease_function);
    
    setScaleY(value);
}
void AnimaNode::updateAnimator_scalez(Animator* animator, float t)
{
    float value = calculateTweenValue(animator->_beginData._v1,animator->_endData._v1,t,animator->_ease_function);
    
    setScaleZ(value);
}
void AnimaNode::updateAnimator_scalexy(Animator* animator, float t)
{
    Vec2 value = calculateTweenValue(animator->_beginData._v2,animator->_endData._v2,t,animator->_ease_function);
    
    setScaleX(value.x);
    setScaleY(value.y);
}
void AnimaNode::updateAnimator_scalexyz(Animator* animator, float t)
{
    Vec3 value = calculateTweenValue(animator->_beginData._v3,animator->_endData._v3,t,animator->_ease_function);
    
    setScaleX(value.x);
    setScaleY(value.y);
    setScaleZ(value.z);
}

void AnimaNode::updateAnimator_contentsizew(Animator* animator, float t)
{
    float value = calculateTweenValue(animator->_beginData._v1,animator->_endData._v1,t,animator->_ease_function);
    const Size& csize = getContentSize();
    setContentSize(Size(value, csize.height));
}
void AnimaNode::updateAnimator_contentsizeh(Animator* animator, float t)
{
    float value = calculateTweenValue(animator->_beginData._v1,animator->_endData._v1,t,animator->_ease_function);
    const Size& csize = getContentSize();
    setContentSize(Size(csize.width, value));
}
void AnimaNode::updateAnimator_contentsize(Animator* animator, float t)
{
    Vec2 value = calculateTweenValue(animator->_beginData._v2,animator->_endData._v2,t,animator->_ease_function);
    
    setContentSize(Size(value));
}

void AnimaNode::updateAnimator_alpha(Animator* animator, float t)
{
    float value = calculateTweenValue(animator->_beginData._v1,animator->_endData._v1,t,animator->_ease_function);
    
    const Color4F& color4 = getColor();
    setColor(Color4F(color4.r,color4.g,color4.b,value));
}
void AnimaNode::updateAnimator_mixcolor(Animator* animator, float t)
{
    Vec4 value = calculateTweenValue(animator->_beginData._v4,animator->_endData._v4,t,animator->_ease_function);
    
    setColor(Color4F(value.x,value.y,value.z,value.w));
}

GRAPHICCORE_END
