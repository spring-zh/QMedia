//
//  SerializeSetting.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "SerializeSetting.h"
#import "QAssetReaderSource.h"
#import "QCaptureSource.h"

#pragma mark QNodeAnimator serialize ---------------------
@implementation QNodeAnimator(serialize)
- (NSDictionary*)serialize {
    NSDictionary* dic = [[NSMutableDictionary alloc] init];
    [dic setValue:NSStringFromClass(self.class) forKey:@"objectType"];
    [dic setValue:self.property forKey:@"property"];
    {
        NSDictionary* dic_timeRang = [[NSMutableDictionary alloc] init];
        [dic_timeRang setValue:@(self.timeRang.location) forKey:@"location"];
        [dic_timeRang setValue:@(self.timeRang.length) forKey:@"length"];
        [dic setValue:dic_timeRang forKey:@"timeRang"];
    }
    {
        NSDictionary* dic_beginPoint = [[NSMutableDictionary alloc] init];
        [dic_beginPoint setValue:@(self.beginPoint.v0) forKey:@"v0"];
        [dic_beginPoint setValue:@(self.beginPoint.v1) forKey:@"v1"];
        [dic_beginPoint setValue:@(self.beginPoint.v2) forKey:@"v2"];
        [dic_beginPoint setValue:@(self.beginPoint.v3) forKey:@"v3"];
        [dic setValue:dic_beginPoint forKey:@"beginPoint"];
    }
    {
        NSDictionary* dic_endPoint = [[NSMutableDictionary alloc] init];
        [dic_endPoint setValue:@(self.endPoint.v0) forKey:@"v0"];
        [dic_endPoint setValue:@(self.endPoint.v1) forKey:@"v1"];
        [dic_endPoint setValue:@(self.endPoint.v2) forKey:@"v2"];
        [dic_endPoint setValue:@(self.endPoint.v3) forKey:@"v3"];
        [dic setValue:dic_endPoint forKey:@"endPoint"];
    }
    [dic setValue:self.name forKey:@"name"];
    [dic setValue:@(self.functionType) forKey:@"functionType"];
    [dic setValue:@(self.repleat) forKey:@"repleat"];
    return dic;
}

+ (QNodeAnimator*)deSerialize:(NSDictionary*)objDic{
    if(![NSClassFromString([objDic valueForKey:@"objectType"]) isEqual:QNodeAnimator.class])
        return nil;
//    QNodeAnimator* nodeAnimator = [[QNodeAnimator alloc] init];
    NSString* name = [objDic valueForKey:@"name"];
    NSString* property = [objDic valueForKey:@"property"];
    NSRange timeRang;
    {
        NSDictionary* timeRang_dic = [objDic valueForKey:@"timeRang"];
        timeRang.length = [[timeRang_dic valueForKey:@"length"] integerValue];
        timeRang.location = [[timeRang_dic valueForKey:@"location"] integerValue];
    }
    QVector beginPoint;
    {
        NSDictionary* beginPoint_dic = [objDic valueForKey:@"beginPoint"];
        beginPoint.v0 = [[beginPoint_dic valueForKey:@"v0"] floatValue];
        beginPoint.v1 = [[beginPoint_dic valueForKey:@"v1"] floatValue];
        beginPoint.v2 = [[beginPoint_dic valueForKey:@"v2"] floatValue];
        beginPoint.v3 = [[beginPoint_dic valueForKey:@"v3"] floatValue];
    }
    QVector endPoint;
    {
        NSDictionary* endPoint_dic = [objDic valueForKey:@"endPoint"];
        endPoint.v0 = [[endPoint_dic valueForKey:@"v0"] floatValue];
        endPoint.v1 = [[endPoint_dic valueForKey:@"v1"] floatValue];
        endPoint.v2 = [[endPoint_dic valueForKey:@"v2"] floatValue];
        endPoint.v3 = [[endPoint_dic valueForKey:@"v3"] floatValue];
    }
    QEaseFunction functionType = (QEaseFunction)[[objDic valueForKey:@"functionType"] intValue];
    bool repleat = [[objDic valueForKey:@"repleat"] boolValue];
    return [[QNodeAnimator alloc] initWith:property range:timeRang begin:beginPoint end:endPoint functype:functionType repleat:repleat name:name];
}

@end
#if 0
#pragma mark QGraphicNode serialize ---------------------
@implementation QGraphicNode(serialize)
- (NSDictionary*)serialize {
    NSDictionary* dic = [NSMutableDictionary new];
//    [dic setValue:@"QGraphicNode" forKey:@"objectType"];
    {
        NSDictionary* dic_renderRange = [[NSMutableDictionary alloc] init];
        [dic_renderRange setValue:@(self.origin_renderRange.location) forKey:@"location"];
        [dic_renderRange setValue:@(self.origin_renderRange.length) forKey:@"length"];
        [dic setValue:dic_renderRange forKey:@"renderRange"];
    }
    {
        NSDictionary* dic_contentSize = [[NSMutableDictionary alloc] init];
        [dic_contentSize setValue:@((float)self.origin_contentSize.width) forKey:@"width"];
        [dic_contentSize setValue:@((float)self.origin_contentSize.height) forKey:@"height"];
        [dic setValue:dic_contentSize forKey:@"contentSize"];
    }
    {
        NSDictionary* dic_position = [[NSMutableDictionary alloc] init];
        [dic_position setValue:@((float)self.origin_position.x) forKey:@"x"];
        [dic_position setValue:@((float)self.origin_position.y) forKey:@"y"];
        [dic setValue:dic_position forKey:@"position"];
    }
    {
        NSDictionary* dic_anchorPoint = [[NSMutableDictionary alloc] init];
        [dic_anchorPoint setValue:@((float)self.origin_anchorPoint.x) forKey:@"x"];
        [dic_anchorPoint setValue:@((float)self.origin_anchorPoint.y) forKey:@"y"];
        [dic setValue:dic_anchorPoint forKey:@"anchorPoint"];
    }
    {
        NSDictionary* dic_color4 = [[NSMutableDictionary alloc] init];
        [dic_color4 setValue:@(self.origin_color4.r) forKey:@"r"];
        [dic_color4 setValue:@(self.origin_color4.g) forKey:@"g"];
        [dic_color4 setValue:@(self.origin_color4.b) forKey:@"b"];
        [dic_color4 setValue:@(self.origin_color4.a) forKey:@"a"];
        [dic setValue:dic_color4 forKey:@"color4"];
    }
    [dic setValue:@(self.origin_visible) forKey:@"visible"];
    {
        NSDictionary* dic_sub = [[NSMutableDictionary alloc] init];
        [dic_sub setValue:@(self.origin_rotation3d.v0) forKey:@"x"];
        [dic_sub setValue:@(self.origin_rotation3d.v1) forKey:@"y"];
        [dic_sub setValue:@(self.origin_rotation3d.v2) forKey:@"z"];
        [dic setValue:dic_sub forKey:@"rotation3d"];
    }
    [dic setValue:@(self.origin_scaleX) forKey:@"scaleX"];
    [dic setValue:@(self.origin_scaleY) forKey:@"scaleY"];
    [dic setValue:@(self.origin_scaleZ) forKey:@"scaleZ"];
    [dic setValue:@(self.origin_positionZ) forKey:@"positionZ"];
    [dic setValue:@(self.origin_alpha) forKey:@"alpha"];
    return dic;
}

+ (void)deSerialize:(NSDictionary*)objDic output:(QGraphicNode*)outObj {
    {
        NSRange renderRange;
        NSDictionary* sub_dic = [objDic valueForKey:@"renderRange"];
        renderRange.location = [[sub_dic valueForKey:@"location"] intValue];
        renderRange.length = [[sub_dic valueForKey:@"length"] intValue];
        outObj.renderRange = renderRange;
    }
    {
        CGSize contentSize;
        NSDictionary* sub_dic = [objDic valueForKey:@"contentSize"];
        contentSize.width = [[sub_dic valueForKey:@"width"] floatValue];
        contentSize.height = [[sub_dic valueForKey:@"height"] floatValue];
        outObj.contentSize = contentSize;
    }
    {
        CGPoint position;
        NSDictionary* sub_dic = [objDic valueForKey:@"position"];
        position.x = [[sub_dic valueForKey:@"x"] floatValue];
        position.y = [[sub_dic valueForKey:@"y"] floatValue];
        outObj.position = position;
    }
    {
        CGPoint anchorPoint;
        NSDictionary* sub_dic = [objDic valueForKey:@"anchorPoint"];
        anchorPoint.x = [[sub_dic valueForKey:@"x"] floatValue];
        anchorPoint.y = [[sub_dic valueForKey:@"y"] floatValue];
        outObj.anchorPoint = anchorPoint;
    }
    {
        QColor4 color4;
        NSDictionary* sub_dic = [objDic valueForKey:@"color4"];
        color4.r = [[sub_dic valueForKey:@"r"] floatValue];
        color4.g = [[sub_dic valueForKey:@"g"] floatValue];
        color4.b = [[sub_dic valueForKey:@"b"] floatValue];
        color4.a = [[sub_dic valueForKey:@"a"] floatValue];
        outObj.color4 = color4;
    }
    {
        QVector rotation3d;
        NSDictionary* sub_dic = [objDic valueForKey:@"rotation3d"];
        rotation3d.v0 = [[sub_dic valueForKey:@"x"] floatValue];
        rotation3d.v1 = [[sub_dic valueForKey:@"y"] floatValue];
        rotation3d.v2 = [[sub_dic valueForKey:@"z"] floatValue];
        outObj.rotation3d = rotation3d;
    }
    outObj.visible = [[objDic valueForKey:@"visible"] boolValue];
//    outObj.rotation = [[objDic valueForKey:@"rotation"] floatValue];
    outObj.scaleX = [[objDic valueForKey:@"scaleX"] floatValue];
    outObj.scaleY = [[objDic valueForKey:@"scaleY"] floatValue];
    outObj.scaleZ = [[objDic valueForKey:@"scaleZ"] floatValue];
    outObj.positionZ = [[objDic valueForKey:@"positionZ"] floatValue];
//    outObj.alpha = [[objDic valueForKey:@"alpha"] floatValue];
}


- (NSDictionary*)serialize:(NSArray*)mediaTrackArray graphicArray:(NSArray*)graphicArray {
    NSDictionary* dic_node = [self serialize];
    
    [dic_node setValue:self.name forKey:@"name"];
    [dic_node setValue:NSStringFromClass(self.class) forKey:@"objectType"];
    
    if(self.animators.count > 0){ // serialize animators
        NSMutableArray *animators_array = [[NSMutableArray alloc] init];
        for (QNodeAnimator* animator in self.animators) {
            [animators_array addObject:[animator serialize]];
        }
        [dic_node setValue:animators_array forKey:@"animators"];
    }
    
    if ([self isKindOfClass:QVideoTrackNode.class]) {
        __weak QVideoTrackNode* trackNode =  (QVideoTrackNode*)self;
        [dic_node setValue:NSStringFromClass(trackNode.class) forKey:@"objectType"];
        [dic_node setValue:@([mediaTrackArray indexOfObject:trackNode.mediaTrack]) forKey:@"mediaTrackIdx"];
    }else if ([self isKindOfClass:QDuplicateNode.class]) {
        __weak QDuplicateNode* duplicateNode =  (QDuplicateNode*)self;
        [dic_node setValue:NSStringFromClass(duplicateNode.class) forKey:@"objectType"];
        [dic_node setValue:@([graphicArray indexOfObject:duplicateNode.nodeFrom]) forKey:@"graphicNodeIdx"];
    }else if ([self isKindOfClass:QLayer.class]) {
        __weak QLayer* layer =  (QLayer*)self;
        [dic_node setValue:NSStringFromClass(layer.class) forKey:@"objectType"];
        {
            NSDictionary* dic_layerSize = [[NSMutableDictionary alloc] init];
            [dic_layerSize setValue:@((float)layer.layerSize.width) forKey:@"width"];
            [dic_layerSize setValue:@((float)layer.layerSize.height) forKey:@"height"];
            [dic_node setValue:dic_layerSize forKey:@"layerSize"];
        }
        {
            NSDictionary* dic_bkColor = [[NSMutableDictionary alloc] init];
            [dic_bkColor setValue:@(layer.bkColor.r) forKey:@"r"];
            [dic_bkColor setValue:@(layer.bkColor.g) forKey:@"g"];
            [dic_bkColor setValue:@(layer.bkColor.b) forKey:@"b"];
            [dic_bkColor setValue:@(layer.bkColor.a) forKey:@"a"];
            [dic_node setValue:dic_bkColor forKey:@"bkColor"];
        }
    }
    return dic_node;
}

+ (QGraphicNode*)deSerialize:(NSDictionary*)objDic mediaArray:(NSArray*)mediaTrackArray graphicArray:(NSArray*)graphicArray {
    QGraphicNode* retNode = nil;
    Class nodeClass = NSClassFromString([objDic valueForKey:@"objectType"]);
    NSString* name = [objDic valueForKey:@"name"];
    if ([nodeClass isEqual:QVideoTrackNode.class]) {
        NSUInteger mediaTrackIdx = [[objDic valueForKey:@"mediaTrackIdx"] integerValue];
        QMediaTrack *mediaTrack = [mediaTrackArray objectAtIndex:mediaTrackIdx];
        if (mediaTrack) {
            retNode = mediaTrack.graphic;//[[QVideoTrackNode alloc] initFromTrack:mediaTrack];
        }
    }else if ([nodeClass isEqual:QDuplicateNode.class]) {
        NSUInteger graphicNodeIdx = [[objDic valueForKey:@"graphicNodeIdx"] integerValue];
        QGraphicNode *graphicNode = [graphicArray objectAtIndex:graphicNodeIdx];
        if (graphicNode) {
            retNode = [[QDuplicateNode alloc] initFromNode:graphicNode];
        }
    }else if ([nodeClass isEqual:QLayer.class]) {
        NSDictionary* dic_layerSize = [objDic valueForKey:@"layerSize"];
        if (dic_layerSize) {
            NSUInteger width = [[dic_layerSize valueForKey:@"width"] intValue];
            NSUInteger height = [[dic_layerSize valueForKey:@"height"] intValue];
            CGSize layerSize = CGSizeMake(width, height);
            QLayer* layer = [[QLayer alloc] initWithSize:layerSize name:name];
            
            NSDictionary *dic_bkColor = [objDic valueForKey:@"bkColor"];
            if (dic_bkColor) {
                QColor4 bkColor;
                bkColor.r = [[dic_bkColor valueForKey:@"r"] floatValue];
                bkColor.g = [[dic_bkColor valueForKey:@"g"] floatValue];
                bkColor.b = [[dic_bkColor valueForKey:@"b"] floatValue];
                bkColor.a = [[dic_bkColor valueForKey:@"a"] floatValue];
                layer.bkColor = bkColor;
            }
            retNode = layer;
        }
        
    }else {
        retNode = [[QGraphicNode alloc] initWithName:name];
    }
    
    [QGraphicNode deSerialize:objDic output:retNode];
    {
        //deSerialize animators
        NSMutableArray *animators_array = [objDic valueForKey:@"animators"];
        if (animators_array) {
            for (NSDictionary* dic_animator in animators_array) {
                [retNode addAnimator:[QNodeAnimator deSerialize:dic_animator]];
            }
        }
    }
    return retNode;
}

@end
#endif
#pragma mark Source serialize ---------------------
@implementation QAssetReaderSource(serialize)
- (NSDictionary*)serialize {
    NSDictionary* dic = [[NSMutableDictionary alloc] init];
    [dic setValue:NSStringFromClass(self.class) forKey:@"objectType"];
    [dic setValue:self.filePath forKey:@"path"];
    return dic;
}
@end

@implementation QCaptureSource(serialize)
- (NSDictionary*)serialize {
    NSDictionary* dic = [[NSMutableDictionary alloc] init];
    [dic setValue:NSStringFromClass(self.class) forKey:@"objectType"];
    [dic setValue:@(self.captureSession.videoFormat) forKey:@"videoFormat"];
    [dic setValue:self.captureSession.cameraPreset forKey:@"cameraPreset"];
    [dic setValue:@(self.captureSession.cameraPosition) forKey:@"cameraPosition"];
    [dic setValue:@(self.captureSession.enableVideo) forKey:@"enableVideo"];
    [dic setValue:@(self.captureSession.enableAudio) forKey:@"enableAudio"];
    return dic;
}
@end
#if 0
#pragma mark QMediaTrack serialize ---------------------
@implementation QMediaTrack(serialize)

- (NSDictionary*)serialize {
    NSDictionary* dic = [[NSMutableDictionary alloc] init];
    [dic setValue:NSStringFromClass(self.class) forKey:@"objectType"];
    [dic setValue:@([self timeScale]) forKey:@"timeScale"];
    [dic setValue:@([self repeatTimes]) forKey:@"repeatTimes"];
    {
        NSRange sRange = [self sourceRange];
        NSDictionary* dic_sourceRange = [[NSMutableDictionary alloc] init];
        [dic_sourceRange setValue:@(sRange.location) forKey:@"location"];
        [dic_sourceRange setValue:@(sRange.length) forKey:@"length"];
        [dic setValue:dic_sourceRange forKey:@"sourceRange"];
    }
    {
        NSRange dRange = [self displayRange];
        NSDictionary* dic_displayTrackRange = [[NSMutableDictionary alloc] init];
        [dic_displayTrackRange setValue:@(dRange.location) forKey:@"location"];
        [dic_displayTrackRange setValue:@(dRange.length) forKey:@"length"];
        [dic setValue:dic_displayTrackRange forKey:@"displayTrackRange"];
    }
    
    if ([self.source isKindOfClass:QAssetReaderSource.class]) {
        [dic setValue:[(QAssetReaderSource*)self.source serialize] forKey:@"mediaSource"];
    }else if ([self.source isKindOfClass:QCaptureSource.class]) {
        [dic setValue:[(QCaptureSource*)self.source serialize] forKey:@"mediaSource"];
    }
    
    return dic;
}

+ (QMediaTrack*)deSerialize:(NSDictionary*)dic factory:(QMediaFactory*)factory {
    NSDictionary* dic_source = [dic valueForKey:@"mediaSource"];
    if (!dic_source)
        return nil;
    QMediaTrack *mediaTrack = nil;
    Class sourceClass = NSClassFromString([dic_source valueForKey:@"objectType"]);
    if ([sourceClass isEqual:QAssetReaderSource.class]) {
        NSString *path = [dic_source valueForKey:@"path"];
        if (path) {
            mediaTrack = [factory createVideoTrack:path];
        }
    }else if ([sourceClass isEqual:QCaptureSource.class]) {
//        int videoFormat = [[dic valueForKey:@"videoFormat"] intValue];
        NSString* cameraPreset = [dic_source valueForKey:@"cameraPreset"];
        AVCaptureDevicePosition cameraPosition = (AVCaptureDevicePosition)[[dic_source valueForKey:@"cameraPosition"] integerValue];
        bool enableVideo = [[dic_source valueForKey:@"enableVideo"] boolValue];
        bool enableAudio = [[dic_source valueForKey:@"enableAudio"] boolValue];
        mediaTrack = [factory createCaptureTrack:cameraPreset position:cameraPosition video:enableVideo audio:enableAudio];
    }
    if (mediaTrack) {
        {
            NSDictionary* dic_sub = [dic valueForKey:@"sourceRange"];
            NSRange range;
            range.location = [[dic_sub valueForKey:@"location"] intValue];
            range.length = [[dic_sub valueForKey:@"length"] intValue];
            mediaTrack.sourceRange = range;
        }
        {
            NSDictionary* dic_sub = [dic valueForKey:@"displayTrackRange"];
            NSRange range;
            range.location = [[dic_sub valueForKey:@"location"] intValue];
            range.length = [[dic_sub valueForKey:@"length"] intValue];
            mediaTrack.displayRange = range;
        }

        mediaTrack.timeScale = [[dic valueForKey:@"timeScale"] floatValue];
        mediaTrack.repeatTimes = [[dic valueForKey:@"repeatTimes"] intValue];

    }
    return mediaTrack;
}

@end
#endif
