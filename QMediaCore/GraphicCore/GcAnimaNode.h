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
        Animator(const std::string& property, Range<int64_t> timeRang, AnimatorData beginValue, AnimatorData endValue, EaseFunctionType functionType = EaseFunctionType::Linear, bool repleat = false ,const std::string& name = ""){
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
        Range<int64_t> _timeRang;
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
    virtual void updateAllAnimations(int64_t timeStamp);
    virtual void updateAnimations(int64_t timeStamp);
    
protected:
    
    class PropretyGroup {
    public:
        PropretyGroup(std::string name): _propertyName(name), _lastTimeStamp(-1) {}
        ~PropretyGroup() = default;
        
        bool addAnimator(Animator* animator)
        {
            if (_animatorList.find(animator) != _animatorList.end())
                return false;
            _animatorList.insert(animator);
            _timeRange = calculateGroupRange();

            return true;
        }
        
        bool removeAnimator(Animator* animator) {
            auto iter = _animatorList.find(animator);
            if( iter != _animatorList.end()) {
                _animatorList.erase(iter);

                _timeRange = calculateGroupRange();
                return true;
            }
            return false;
        }
        
        void updateProperty(int64_t timeStamp) {
            if (!_timeRange.isValid()) {
                return;
            }
            
//            if (timeStamp >= _timeRange._start) {
                if (timeStamp > _timeRange._end) {
                    timeStamp = _timeRange._end;
                } else if (timeStamp < _timeRange._start)
                    timeStamp = _timeRange._start;
                
                if (timeStamp != _lastTimeStamp) {
                    _updateProperty(timeStamp);
                    _lastTimeStamp = timeStamp;
                }
//            }
        }
    private:
        void _updateProperty(int64_t timeStamp)
        {
            for (auto& animator : _animatorList) {
                Range<int64_t>& timeRang = animator->_timeRang;
                if ((timeStamp >= timeRang._start) &&
                    (timeStamp <= timeRang._end)) {
                    float t = (float)(timeStamp - timeRang._start)/(timeRang._end - timeRang._start);
                    (animator->_update)(animator,t);
                }
            }
        }

        Range<int64_t> calculateGroupRange() {
            //calcualte new timeRange
            Range<int64_t> timeRange(std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::min());//{INT_MAX,-INT_MAX};
            for (auto &iter : _animatorList){
                Range<int64_t>& tRange = iter->_timeRang;
                if (timeRange._start > tRange._start) {
                    timeRange._start = tRange._start;
                }
                if (timeRange._end < tRange._end) {
                    timeRange._end = tRange._end;
                }
            }
            if (timeRange._start > timeRange._end) {
                timeRange = {0,0};
            }
            return timeRange;
        }
        
        std::string _propertyName;
        std::set<Animator*> _animatorList;
        Range<int64_t> _timeRange;
        
        int64_t _lastTimeStamp;
    };
    
    std::map<std::string, std::shared_ptr<PropretyGroup>> _animatorsGroup;
    std::mutex _render_mutex;
    
private:
    void updateAnimator_positionxyz(Animator* animator, float t);
    
    void updateAnimator_rotationxyz(Animator* animator, float t);
    void updateAnimator_rotation(Animator* animator, float t);
    
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
    void updateAnimator_crop(Animator* animator, float t);
    
    bool checkAndSetAnimator(Animator* animator);
};
    
GRAPHICCORE_END

#endif /* GRAPHICCORE_ANIMATIONNODE_H */
