## **QCombiner** 组合器， 定义如下
**QCombiner**组合器类，sdk的核心模块。负责管理媒体资源和图像资源。 是QEditorPlayer和QExporter的基类。
```objc
@interface QCombiner : NSObject <Serializable>

@property (nonatomic) id<QVideoTarget> videoTarget; //视频输出目标
@property (nonatomic) id<QAudioTarget> audioTarget; //音频输出目标

//资源管理
@property (nonatomic, readonly) NSDictionary<NSString*, QMediaTrack*>* subObjects;
@property (nonatomic, readonly) NSDictionary<NSString*, QGraphicNode*>* graphicNodes;
@property (nonatomic, readonly) NSDictionary<NSString*, QGraphicNode*>* duplicateNodes;

//渲染场景根节点，使用 addChildNode添加子节点
@property (nonatomic, readonly) DisplayRootNode* rootNode;

@property (nonatomic, readonly) QMediaFactory* mediaFactory; //用于创建媒体轨道

- (void)setVideoConfig:(QVideoDescribe*)describe; // 设置视频输出参数
- (void)setAudioConfig:(QAudioDescribe*)describe; // 设置音频输出参数

- (void)addMediaTrack:(QMediaTrack*)track; //添加媒体轨道
- (void)removeMediaTrack:(QMediaTrack*)track; //删除媒体轨道

@property (nonatomic, readonly) int64_t position; //当前进度，单位ms

//媒体资源时长，如果不设置会根据当前添加的媒体资源自动计算
@property (nonatomic) NSRange mediaTimeRange; 

//复制另一个组合器的资源以及设置状态， 可以用于预览的数据复制到合成器
- (void)copyFrom:(QCombiner*)from;

@end
```