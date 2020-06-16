# **资源概述**
sdk所使用的渲染资源有图像资源 **QGraphicNode** , 音频资源 **QAudioTrackNode**。
sdk提供了一套比较自由的渲染框架。图形渲染引擎是基于 **QGraphicNode** 组成的树状渲染结构 , 使用 addChildNode 添加子节点。  
媒体轨道资源 **QMediaTrack** ， 持有视频资源 **QVideoTrackNode** 和 音频资源 **QAudioTrackNode** (或者为其中任一)。
**QMediaTrack** 可以使用自定义数据源 **QMediaSource** 进行扩展。

# **使用**

## **预览使用例子**
```objc
const int targetW = 640;
const int targetH = 480;
    
QEditorPlayer* player = [[QEditorPlayer alloc] init]; //新建预览播放器
[player addObserver:self]; //设置回调

QVideoDescribe* vdesc = [[QVideoDescribe alloc] initWithParamenters:QVideoCodecH264 framerate:25 width:targetW height:targetH bitrate:2*1024*1024]; //视频描述

[player setVideoConfig:vdesc]; //通过视频描述设置渲染目标画布大小以及渲染帧率

//创建视频轨道
NSString* videoFile = ...
QMediaTrack* videoTrack = [player.mediaFactory createVideoTrack:videoFile combiner:player];
//创建音频轨道，背景音
NSString* audioFile = ...
QMediaTrack* audioTrack = [player.mediaFactory createAudioTrack:audioFile combiner:player];

//添加轨道
[player addMediaTrack:videoTrack];
[player addMediaTrack:audioTrack];

//设置视频渲染对象参数
videoTrack.graphic.position = CGPointMake(targetW/4, targetH/4); 
videoTrack.graphic.contentSize = CGSizeMake(targetW/2, targetH/2); 设置显示大小

//定义图片对象
NSString* imageFile = ...
QImageNode* imageNode = [[QImageNode alloc] initWithPath:imageFile combiner:player];
imageNode.contentSize = CGSizeMake(320, 240);
imageNode.renderRange = NSMakeRange(0, 10000); //设置图象渲染时间区间
imageNode.alpha = 0.8;

//渲染结构组成
[player.rootNode addChildNode:videoTrack.graphic];
[player.rootNode addChildNode:imageNode];

//开始预览
[player start];
```
## **合成使用例子**
```objc
NSString* filePath = ...
QFileExporter* exporter = [[QFileExporter alloc] initWithPath:filePath]; //新建文件合成器
[exporter addObserver:self]; //设置合成回调
//设置合成输出参数， 一般使用和预览同样的参数
[exporter setAudioConfig:adesc];
[exporter setVideoConfig:vdesc];

[exporter copyFrom:player]; //把预览状态轨道以及资源复制到合成中 (也可以手动设置合成轨道以及资源)
[exporter start]; //启动合成
```
# **API类详解**
## [QCombiner](class_ios/QCombiner_ios.md)
## [QEditorPlayer](class_ios/QEditorPlayer_ios.md)
## [QFileExporter](class_ios/QFileExporter_ios.md)
## [QMediaTrack](class_ios/QMediaTrack_ios.md)
## [QGraphicNode以及媒体渲染](class_ios/QGraphicNode_ios.md)
## [QNodeAnimator](class_ios/QNodeAnimator_ios.md)
## [QAudioTrackNode](class_ios/QAudioTrackNode_ios.md)


