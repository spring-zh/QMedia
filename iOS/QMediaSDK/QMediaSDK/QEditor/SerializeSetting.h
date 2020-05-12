//
//  SerializeSetting.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QGraphicNode_internal.h"
#import "QMediaTrack_internal.h"
#import "QAssetReaderSource.h"
#import "QCaptureSource.h"
#import "QDuplicateNode.h"
#import "QLayer.h"
#import "QMediaFactory.h"


@interface QNodeAnimator(serialize) <Serializable>
- (NSDictionary*)serialize;
+ (QNodeAnimator*)deSerialize:(NSDictionary*)objDic;
@end


@interface QGraphicNode(serialize) <Serializable>
- (NSDictionary*)serialize;
+ (void)deSerialize:(NSDictionary*)objDic output:(QGraphicNode*)outObj;

- (NSDictionary*)serialize:(NSArray*)mediaTrackArray graphicArray:(NSArray*)graphicArray;
+ (QGraphicNode*)deSerialize:(NSDictionary*)objDic mediaArray:(NSArray*)mediaTrackArray graphicArray:(NSArray*)graphicArray;
@end

@interface QAssetReaderSource(serialize) <Serializable>
- (NSDictionary*)serialize;
@end

@interface QCaptureSource(serialize) <Serializable>
- (NSDictionary*)serialize;
@end

@interface QMediaTrack(serialize) <Serializable>
//serialize media track settings
- (NSDictionary*)serialize;
+ (QMediaTrack*)deSerialize:(NSDictionary*)dic factory:(QMediaFactory*)factory;
@end
