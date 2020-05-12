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
#import "SerializeSetting.h"

extern const struct VideoDescribe XMToVideoDescribe(QVideoDescribe* xmdesc);
extern const struct AudioDescribe XMToAudioDescribe(QAudioDescribe* xmdesc);

@implementation QCombiner {
    NSMutableArray<QMediaTrack*>* _subObjectArray;
    NSMutableArray<QGraphicNode*>* _graphicNodesArray;
    EffectCombiner* _combiner;
    QMediaFactory *_mediaFactory;
    QGraphicNode* _rootNode;
}

- (instancetype)initWithNative:(EffectCombiner* )combiner_native
{
    if ((self = [super init]) != nil) {
        _combiner = combiner_native;
        _mediaFactory = [QMediaFactory new];
        _subObjectArray = [NSMutableArray new];
        _graphicNodesArray = [NSMutableArray new];
        _rootNode = [[QGraphicNode alloc] initWithNode:_combiner->getRootNode()];
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

- (NSArray<QMediaTrack*>*)subObjects
{
    return _subObjectArray;
}

#pragma mark - Player Control
- (void)addMediaTrack:(QMediaTrack*)track
{
    _combiner->addMediaTrack(track.native);
    _combiner->attachAudioNode(track.audio.native, nullptr);
    [_subObjectArray addObject:track];
}
- (void)removeMediaTrack:(QMediaTrack*)track
{
    [self detachRenderNode:track.graphic];
    _combiner->detachAudioNode(track.audio.native);
    _combiner->removeMediaTrack(track.native);
    [_subObjectArray removeObject:track];
}

- (void)addGraphicNode:(QGraphicNode*)node {
    if ([_graphicNodesArray containsObject:node])
        return;
    
    int index = 0;
    if (! [node isKindOfClass:QDuplicateNode.class]) {
        for (QGraphicNode *graphicNode in _graphicNodesArray) {
            if ([graphicNode isKindOfClass:QDuplicateNode.class]) {
                [_graphicNodesArray insertObject:node atIndex:index];
                return;
            }
            index ++;
        }
    }
    
    [_graphicNodesArray addObject:node];
}
- (void)removeGraphicNode:(QGraphicNode*)node {
    if ([_graphicNodesArray containsObject:node]) {
        [_graphicNodesArray removeObject:node];
    }
}

- (bool)attachRenderNode:(QGraphicNode*)child parent:(QGraphicNode*)parent {
    if ([parent addChildNodeDirect:child]) {
        _combiner->attachRenderNode(child.native, parent.native);
        return true;
    }
    return false;
}
- (bool)detachRenderNode:(QGraphicNode*)current {
    if ([current.parent removeChildNodeDirect:current]) {
        _combiner->detachRenderNode(current.native);
        return true;
    }
    return false;
}

- (NSRange)mediaTimeRange
{
    NSRange nsRange;
    Range<int64_t> tRange = _combiner->getMediaTimeRange();
    nsRange.location = static_cast<NSUInteger>(tRange._start);
    nsRange.length = static_cast<NSUInteger>(tRange.getLength());
    return nsRange;
}

- (QGraphicNode*)rootNode {
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

@end
