//
//  QCombiner.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QCombiner_internal.h"
#import "QCommon.h"
#import "QObserverHoster.h"
#import "QAudioTarget_internal.h"
#import "QVideoTarget_internal.h"
#import "QGraphicNode_internal.h"
#import "QMediaTrack_internal.h"
#import "QMediaFactory_internal.h"
#import "QAudioTrackNode_internal.h"
#import "QEffect_internal.h"
#import "SerializeSetting.h"
#import "QEffectManage.h"

extern const struct VideoDescribe XMToVideoDescribe(QVideoDescribe* xmdesc);
extern const struct AudioDescribe XMToAudioDescribe(QAudioDescribe* xmdesc);

@interface QLayer(internal)
- (instancetype)initWithNative:(GraphicCore::LayerRef)layer combiner:(QCombiner*)combiner uid:(NSString*)uid;
@end

@implementation QDisplayLayer
@end

@implementation QCombiner {
    NSMutableDictionary<NSString*, QMediaTrack*>* _subObjectArray;
    NSMutableDictionary<NSString*, QGraphicNode*>* _graphicNodesArray;
    NSMutableDictionary<NSString*, QGraphicNode*>* _duplicateNodesArray;
    EffectCombiner* _combiner;
    QMediaFactory *_mediaFactory;
    QDisplayLayer* _rootNode;
}

- (instancetype)initWithNative:(EffectCombiner* )combiner_native
{
    if ((self = [super init]) != nil) {
        _combiner = combiner_native;
        _mediaFactory = [QMediaFactory new];
        _subObjectArray = [NSMutableDictionary new];
        _graphicNodesArray = [NSMutableDictionary new];
        _duplicateNodesArray = [NSMutableDictionary new];
        _rootNode = [[QDisplayLayer alloc] initWithNative:_combiner->getRootNode() combiner:self uid:@"0"];
    }
    return self;
}

- (EffectCombiner*)native {
    return _combiner;
}

- (id<QVideoTarget>)videoTarget {
    return _mediaFactory.videoTarget;
}

- (id<QAudioTarget>)audioTarget {
    return _mediaFactory.audioTarget;
}

- (void)setVideoTarget:(id<QVideoTarget>)videoTarget {
    [_mediaFactory setVideoTarget:videoTarget];
    _combiner->setVideoTarget([_mediaFactory nativeVideoTarget]);
}

- (void)setAudioTarget:(id<QAudioTarget>)audioTarget {
    [_mediaFactory setAudioTarget:audioTarget];
    _combiner->setAudioTarget([_mediaFactory nativeAudioTarget]);
}

- (void)setVideoConfig:(QVideoDescribe*)describe
{
    _combiner->setvideoConfig(XMToVideoDescribe(describe));
}
- (void)setAudioConfig:(QAudioDescribe*)describe
{
    _combiner->setAudioConfig(XMToAudioDescribe(describe));
}

- (NSDictionary<NSString*, QMediaTrack*>*)subObjects
{
    return _subObjectArray;
}

- (NSDictionary<NSString*, QGraphicNode*>*)graphicNodes
{
    return _graphicNodesArray;
}

- (NSDictionary<NSString*, QGraphicNode*>*)duplicateNodes
{
    return _duplicateNodesArray;
}

#pragma mark - Player Control
- (void)addMediaTrack:(QMediaTrack*)track
{
    if (! [_subObjectArray objectForKey:track.uid]) {
        _combiner->addMediaTrack(track.native);
        [_subObjectArray setValue:track forKey:track.uid];
    }
}
- (void)removeMediaTrack:(QMediaTrack*)track
{
    if ([_subObjectArray objectForKey:track.uid]) {
        [self detachRenderNode:track.graphic];
        _combiner->detachAudioNode(track.audio.native);
        _combiner->removeMediaTrack(track.native);
        [_subObjectArray removeObjectForKey:track.uid];
    }
}

- (void)addGraphicNodeIndex:(QGraphicNode*)node {
    if ([node isKindOfClass:QDuplicateNode.class]) {
        [self addDuplicateNodeIndex:node];
    }else {
        if ([_graphicNodesArray objectForKey:node.uid])
        return;
        [_graphicNodesArray setValue:node forKey:node.uid];
    }
}
- (void)removeGraphicNodeIndex:(QGraphicNode*)node {
    if ([node isKindOfClass:QDuplicateNode.class]) {
        [self removeDuplicateNodeIndex:node];
    }else {
        if ([_graphicNodesArray objectForKey:node.uid])
            [_graphicNodesArray removeObjectForKey:node.uid];
    }
}

- (bool)addDuplicateNodeIndex:(QGraphicNode*)node {
    if (![_duplicateNodesArray objectForKey:node.uid]) {
        [_duplicateNodesArray setValue:node forKey:node.uid];
        return true;
    }
    return false;
}

- (bool)removeDuplicateNodeIndex:(QGraphicNode*)node {
    if ([_duplicateNodesArray objectForKey:node.uid]) {
        [_duplicateNodesArray removeObjectForKey:node.uid];
        return true;
    }
    return false;
}

- (bool)attachRenderNode:(QGraphicNode*)child parent:(QGraphicNode*)parent {
    _combiner->attachRenderNode(child.native, parent.native);
    return true;
}
- (bool)detachRenderNode:(QGraphicNode*)current {
    _combiner->detachRenderNode(current.native);
    return true;
}

- (bool)attachEffect:(QGraphicNode*)graphic effect:(QEffect*)effect {
    _combiner->attachEffect(graphic.native, effect.native);
    return true;
}
- (bool)detachEffect:(QGraphicNode*)graphic effect:(QEffect*)effect {
    _combiner->detachEffect(graphic.native, effect.native);
    return true;
}

- (bool)attachAudioNode:(QAudioTrackNode*)child parent:(QAudioTrackNode*)parent {
    _combiner->attachAudioNode(child.native, nullptr);
    return true;
}

- (bool)detachAudioNode:(QAudioTrackNode*)current {
    _combiner->detachAudioNode(current.native);
    return true;
}

- (void)setMediaTimeRange:(NSRange)timeRange {
    Range<int64_t> tRange = {static_cast<int64_t>(timeRange.location) , static_cast<int64_t>(timeRange.location + timeRange.length)};
    _combiner->setValidTimeRange(tRange);
}
- (NSRange)mediaTimeRange
{
    NSRange nsRange;
    Range<int64_t> tRange = _combiner->getValidTimeRange();
    nsRange.location = static_cast<NSUInteger>(tRange._start);
    nsRange.length = static_cast<NSUInteger>(tRange.getLength());
    return nsRange;
}

- (QDisplayLayer*)rootNode {
    return _rootNode;
}

- (QMediaFactory*)mediaFactory {
//    static XMMediaFactory* instance = nil;
//    static dispatch_once_t onceToken;
//    dispatch_once(&onceToken, ^{
//        instance = [[XMMediaFactory alloc] initWithTarget:_videoTarget audio:_audioTarget];
//    });
//    return instance;
    return _mediaFactory;
}

#pragma mark copy settings
- (void)copyFrom:(QCombiner*)from {
    self.mediaTimeRange = from.mediaTimeRange;
    
    [_graphicNodesArray removeAllObjects];
    //copy rootNode
    [_rootNode copyFrom:from.rootNode];
    _rootNode.bkColor = from.rootNode.bkColor;
//    _rootNode.flipMode = from.rootNode.flipMode;
    _rootNode.enable3d = from.rootNode.enable3d;
    [self addGraphicNodeIndex:_rootNode];
    
    //copy mediaTracks
    for (QMediaTrack* fromTrack in from.subObjects.allValues) {
        if ([fromTrack.source isKindOfClass:QAssetReaderSource.class]) {
            QAssetReaderSource* fromSource = (QAssetReaderSource*)fromTrack.source;
            QAssetReaderSource* mediaSource = [[QAssetReaderSource alloc] initWithFilePath:fromSource.filePath audio:fromSource.enableAudio video:fromSource.enableVideo];
            mediaSource.video_frame_format = fromSource.video_frame_format;
            [mediaSource setVideoTarget:_mediaFactory.videoTarget];
            [mediaSource setAudioTarget:_mediaFactory.audioTarget];
            QMediaTrack* mediaTrack = [[QMediaTrack alloc] initWithMediaSource:mediaSource uid:fromTrack.uid];
            if ([mediaTrack prepare]) {
                [self addMediaTrack:mediaTrack];
                if (fromTrack.graphic) {
                    [mediaTrack generateVideoTrackNode:self uid:fromTrack.graphic.uid];
                    [mediaTrack.graphic copyFrom:fromTrack.graphic];
                }
                [mediaTrack generateAudioTrackNode:self uid:fromTrack.audio.uid];
            }
        }
    }
    
    //copy graphic nodes
    for (QGraphicNode *fromNode in from.graphicNodes.allValues) {
        QGraphicNode* newNode;
        if ([fromNode isKindOfClass:QDisplayLayer.class]) {
            continue;
        }else if ([fromNode isKindOfClass:QVideoTrackNode.class]) {
            continue;
        }else if ([fromNode isKindOfClass:QLayer.class]) {
            QLayer* fromLayer = (QLayer*)fromNode;
            QLayer* layer = [[QLayer alloc] initWithSize:fromLayer.layerSize combiner:self uid:fromLayer.uid];
            layer.bkColor = fromLayer.bkColor;
            layer.enable3d = fromLayer.enable3d;
            newNode = layer;
        }else if ([fromNode isKindOfClass:QImageNode.class]) {
            QImageNode* fromImageNode = (QImageNode*)fromNode;
            QImageNode* imageNode = [[QImageNode alloc] initWithPath:fromImageNode.filePath combiner:self uid:fromNode.uid];
            newNode = imageNode;
        }
        else /*QGraphicNode*/{
            newNode = [[QGraphicNode alloc] initWithName:fromNode.name combiner:self uid:fromNode.uid];
        }
        
        [newNode copyFrom:fromNode];
    }
    
    //copy duplicate nodes
    for (QGraphicNode *fromNode in from.duplicateNodes.allValues) {
        QDuplicateNode* fromDuplicateNode = (QDuplicateNode*)fromNode;
        QGraphicNode* dependentNode = [_graphicNodesArray objectForKey:fromDuplicateNode.nodeFrom.uid];
        QDuplicateNode* newNode = [[QDuplicateNode alloc] initFromNode:dependentNode combiner:self uid:fromNode.uid];
        [newNode copyFrom:fromNode];
    }
    
    [self copyRenderTrees:from.rootNode currentNode:_rootNode];
}

- (void)copyRenderTrees:(QGraphicNode*)fromNode currentNode:(QGraphicNode*)currentNode {
    for (QGraphicNode* graphicNode in fromNode.childrens) {
        QGraphicNode* childNode = nil;
        if ([graphicNode isKindOfClass:QDuplicateNode.class]) {
            childNode = [_duplicateNodesArray objectForKey:graphicNode.uid];
        }else {
            childNode = [_graphicNodesArray objectForKey:graphicNode.uid];
        }
        
        if (childNode != nil) {
            [currentNode addChildNode:childNode];
            [self copyRenderTrees:graphicNode currentNode:childNode];
        }
    }
}

#if 0
#pragma mark serialize ------------------------------------------
#pragma mark serialize_graphicTree
- (NSDictionary*)serialize_graphicTree:(QGraphicNode*)root isRoot:(bool)isRoot{
    NSDictionary* dic = [NSMutableDictionary new];
    if (isRoot) {
        [dic setValue:@(-100) forKey:@"nodeIdx"];
    }else
        [dic setValue:@([_graphicNodesArray indexOfObject:root]) forKey:@"nodeIdx"];
    if (root.childrens.count > 0) {
        NSMutableArray *childrens_array = [[NSMutableArray alloc] init];
        for (QGraphicNode* childern in root.childrens) {
            [childrens_array addObject:[self serialize_graphicTree:childern isRoot:false]];
        }
        [dic setValue:childrens_array forKey:@"childrens"];
    }
    return dic;
}

#pragma mark serialize combiner
- (NSDictionary*)serialize
{
    NSDictionary* dic = [NSMutableDictionary new];
    [dic setValue:NSStringFromClass(QCombiner.class) forKey:@"objectType"];
    [dic setValue:@"v1" forKey:@"version"];
    //serialize mediaTracks
    if (_subObjectArray.count > 0) {
        NSMutableArray *mediaArray = [NSMutableArray new];
        for (QMediaTrack* mediaTrack : _subObjectArray) {
            [mediaArray addObject:[mediaTrack serialize]];
        }
        [dic setValue:mediaArray forKey:@"mediaTracks"];
    }
    //serialize graphicNodes
    if (_graphicNodesArray.count > 0) {
        NSMutableArray *nodeArray = [NSMutableArray new];
        for (QGraphicNode* graphicNode : _graphicNodesArray) {
            [nodeArray addObject:[graphicNode serialize:_subObjectArray graphicArray:_graphicNodesArray]];
        }
        [dic setValue:nodeArray forKey:@"graphicNodes"];
    }
    {
        //serialize render trees
        NSDictionary* dic_graphicTree = [self serialize_graphicTree:_rootNode isRoot:true];
        {
            NSDictionary *dic_tree_color = [NSMutableDictionary new];
            [dic_tree_color setValue:@(_rootNode.color4.r) forKey:@"r"];
            [dic_tree_color setValue:@(_rootNode.color4.g) forKey:@"g"];
            [dic_tree_color setValue:@(_rootNode.color4.b) forKey:@"b"];
            [dic_tree_color setValue:@(_rootNode.color4.a) forKey:@"a"];
            [dic_graphicTree setValue:dic_tree_color forKey:@"color"];
        }
        [dic setValue:dic_graphicTree forKey:@"graphicTree"];
    }
    return dic;
}

#pragma mark deSerialize ------------------------------------------

- (void)deSerialize_graphicTree:(NSDictionary*)dic_root root:(QGraphicNode*)rootNode{
    NSArray* childrens = [dic_root valueForKey:@"childrens"];
    if (childrens) {
        for (NSDictionary* children : childrens) {
            int nodeIdx = [[children valueForKey:@"nodeIdx"] intValue];
            QGraphicNode* childrenNode = [_graphicNodesArray objectAtIndex:nodeIdx];
            if (childrenNode) {
                [self attachRenderNode:childrenNode parent:rootNode];
                [self deSerialize_graphicTree:children root:childrenNode];
            }
        }
    }
}
- (bool)loadSerializeSettings:(NSDictionary*)settings
{
    if(![NSClassFromString([settings valueForKey:@"objectType"]) isEqual:QCombiner.class])
        return false;
    {//deserialize mediaTracks
        NSMutableArray *mediaArray = [settings valueForKey:@"mediaTracks"];
        if (mediaArray) {
            for (NSDictionary* dic_track in mediaArray) {
                QMediaTrack * mediaTrack = [QMediaTrack deSerialize:dic_track factory:_mediaFactory];
                if(mediaTrack)
                    [self addMediaTrack:mediaTrack];
            }
        }
    }
    {//deserialize graphicNodes
        NSMutableArray *nodeArray = [settings valueForKey:@"graphicNodes"];
        if (nodeArray) {
            for (NSDictionary* dic_node in nodeArray) {
                QGraphicNode* graphicNode = [QGraphicNode deSerialize:dic_node mediaArray:_subObjectArray graphicArray:_graphicNodesArray];
                if (graphicNode)
                    [self addGraphicNode:graphicNode];
            }
        }
    }
    {
        //deserialize render trees
        NSDictionary* dic_tree = [settings valueForKey:@"graphicTree"];
        if (dic_tree) {
            NSDictionary *dic_tree_color = [dic_tree valueForKey:@"color"];
            if (dic_tree_color) {
                QColor4 bkColor;
                bkColor.r = [[dic_tree_color valueForKey:@"r"] floatValue];
                bkColor.g = [[dic_tree_color valueForKey:@"g"] floatValue];
                bkColor.b = [[dic_tree_color valueForKey:@"b"] floatValue];
                bkColor.a = [[dic_tree_color valueForKey:@"a"] floatValue];
                _rootNode.color4 = bkColor;
            }
            [self deSerialize_graphicTree:dic_tree root:_rootNode];
        }
    }
    return true;
}
#endif
@end
