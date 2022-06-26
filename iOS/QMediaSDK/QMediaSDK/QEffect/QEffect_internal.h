//
//  QEffect_internal.h
//  QMediaSDK
//
//  Created by spring on 2020/6/18.
//  Copyright © 2020 QMedia. All rights reserved.
//

#include "RenderEngine/effect/Effect.h"
#import "QEffect.h"

@interface QEffect(internal)

- (instancetype)initWithNative:(QMedia::RenderEngine::EffectRef)effect;
@property (nonatomic, readonly) QMedia::RenderEngine::EffectRef native;

@end
