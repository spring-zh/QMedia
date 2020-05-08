//
//  GcAnimaNode.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_ANIMANODE_H
#define GRAPHICCORE_ANIMANODE_H

#include <functional>
#include <memory>
#include <set>
#include "Utils/Comm.h"
#include "GcNode.h"

GRAPHICCORE_BEGIN

enum class EaseFunctionType : int
{
    CUSTOM_EASING = -1,
    
    Linear,
    
    Sine_EaseIn,
    Sine_EaseOut,
    Sine_EaseInOut,
    
    
    Quad_EaseIn,
    Quad_EaseOut,
    Quad_EaseInOut,
    
    Cubic_EaseIn,
    Cubic_EaseOut,
    Cubic_EaseInOut,
    
    Quart_EaseIn,
    Quart_EaseOut,
    Quart_EaseInOut,
    
    Quint_EaseIn,
    Quint_EaseOut,
    Quint_EaseInOut,
    
    Expo_EaseIn,
    Expo_EaseOut,
    Expo_EaseInOut,
    
    Circ_EaseIn,
    Circ_EaseOut,
    Circ_EaseInOut,
    
    Elastic_EaseIn,
    Elastic_EaseOut,
    Elastic_EaseInOut,
    
    Back_EaseIn,
    Back_EaseOut,
    Back_EaseInOut,
    
    Bounce_EaseIn,
    Bounce_EaseOut,
    Bounce_EaseInOut,
    
    TWEEN_EASING_MAX = 10000
};
    
typedef float (*ease_function)(float);
extern ease_function getTweenFunctionByType(EaseFunctionType functionType);

class AnimaNode : public Node{
public:
    
    union AnimatorData{
        AnimatorData():_v4(){}
        AnimatorData(float v1):_v1(v1){}
        AnimatorData(const Vec2& v2):_v2(v2){}
        AnimatorData(const Vec3& v3):_v3(v3){}
        AnimatorData(const Vec4& v4):_v4(v4){}
        AnimatorData(const AnimatorData& copy){
            *this = copy;
        }
        ~AnimatorData() {}
        float _v1;
        Vec2 _v2;
        Vec3 _v3;
        Vec4 _v4;
    };
    
    class Animator {
    public:
        Animator():_repleat(false), _ease_function(NULL),_update(nullptr) {}
        Animator(const std::string& property, Range<int> timeRang, AnimatorData beginValue, AnimatorData endValue, EaseFunctionType functionType = EaseFunctionType::Linear, bool repleat = false ,const std::string& name = ""){
            _property = property;
            _name = name;
            _beginValue = beginValue;
            _endValue = endValue;
            _timeRang = timeRang;
            _repleat = repleat;
            _functionType = functionType;
        }
        std::string _name;
        std::string _property;
        AnimatorData _beginValue;
        AnimatorData _endValue;
        Range<int> _timeRang;
        EaseFunctionType _functionType;
        bool _repleat;
    private:
        friend class AnimaNode;
        ease_function _ease_function;
        std::function<void(Animator*, float)> _update;
    };
    using AnimatorRef = std::shared_ptr<Animator>;
    
    AnimaNode();
    virtual ~AnimaNode();
    
    bool addAnimator(Animator* animator);
    bool removeAnimator(Animator* animator);
    
    //update self and sub nodes 's animations
    virtual void updateAnimations(int timeStamp);
    
protected:
    
    std::set<Animator*> _animatorList;
    
    void updateNodeProperty(int timeStamp);
    
private:
    void updateAnimator_positionxyz(Animator* animator, float t);
    
    void updateAnimator_rotationxyz(Animator* animator, float t);
    
    void updateAnimator_scalex(Animator* animator, float t);
    void updateAnimator_scaley(Animator* animator, float t);
    void updateAnimator_scalez(Animator* animator, float t);
    void updateAnimator_scalexy(Animator* animator, float t);
    void updateAnimator_scalexyz(Animator* animator, float t);
    
    void updateAnimator_contentsizew(Animator* animator, float t);
    void updateAnimator_contentsizeh(Animator* animator, float t);
    void updateAnimator_contentsize(Animator* animator, float t);
    
    void updateAnimator_alpha(Animator* animator, float t);
    void updateAnimator_mixcolor(Animator* animator, float t);
};
    
GRAPHICCORE_END

#endif /* GRAPHICCORE_ANIMATIONNODE_H */
