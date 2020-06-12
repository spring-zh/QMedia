//
//  IOSGLTextureDrawable.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "IOSTexture.h"

// Audio codec types
typedef NS_ENUM(NSInteger, QFilpMode) {
    QFilpNone = 0,
    QFilpH,
    QFilpV
};

@interface IOSGLTextureDrawable : NSObject

- (instancetype)initWithTexture:(id<IOSTexture>)iosTexture;
- (bool)useAsFrameBuffer;
- (void)releaseFb;

+ (void)savePrevStatus;
+ (void)loadPrevStatus;

@property (nonatomic, readonly) id<IOSTexture> texture;
@property (nonatomic, readonly) GLuint glFbId;

- (bool)draw:(QFilpMode)filpMode;

@end
