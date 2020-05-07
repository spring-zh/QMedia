//
//  QCombiner.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QCombiner.h"
#include "EffectEditor/EffectCombiner.h"

@interface QCombiner(internal)
- (instancetype)initWithNative:(EffectCombiner*)combiner_native;
@property (nonatomic, readonly) EffectCombiner* native;
@end
