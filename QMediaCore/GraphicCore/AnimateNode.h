//
//  AnimateNode.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef QMEDIA_ANIMATE_NODE_H
#define QMEDIA_ANIMATE_NODE_H

#include <mutex>
#include <unordered_map>
#include "ease_function.h"
#include "BaseNode.h"

namespace QMedia { namespace Api {

typedef float (*ease_function)(float);
typedef void (*update_func)(BaseNode* node, float t, const Animator& animator, ease_function ef);

class AnimateNode : public BaseNode {
public:
    AnimateNode() ;
    virtual ~AnimateNode() {}
    
    virtual std::vector<Animator> getAnimators() override;

    virtual bool addAnimator(const Animator & animator) override;

    virtual bool removeAnimator(const Animator & animator) override;
    
    void updateNodeByAnimations(int64_t time_ms);
    
protected:
    using AnimatorBind = std::tuple<Animator, update_func, int64_t>;
    
    std::mutex mutex_;
    std::vector<AnimatorBind> animator_binds_;
};

}
}

#endif /* QMEDIA_ANIMATE_NODE_H */
