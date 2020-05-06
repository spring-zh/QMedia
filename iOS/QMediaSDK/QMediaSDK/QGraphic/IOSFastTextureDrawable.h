//
//  IOSFastTextureDrawable.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "IOSFastTexture.h"

@interface IOSFastTextureDrawable : NSObject

- (instancetype)initWithTexture:(IOSFastTexture*)iosTexture;
- (bool)useAsFrameBuffer;
- (void)releaseFb;

+ (void)savePrevStatus;
+ (void)loadPrevStatus;

@property (nonatomic, readonly) IOSFastTexture* texture;
@property (nonatomic, readonly) GLuint glFbId;

- (bool)draw;

@end
