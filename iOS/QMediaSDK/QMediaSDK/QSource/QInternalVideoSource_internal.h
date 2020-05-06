//
//  XMVideoSource.h
//  XLMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 Xunlei. All rights reserved.
//

#import "XMOldVideoSource.h"
#include "EffectEditor/MediaSource.h"

@interface QInternalVideoSource (native)
@property (nonatomic, readonly) MediaSourceRef native;
@end
