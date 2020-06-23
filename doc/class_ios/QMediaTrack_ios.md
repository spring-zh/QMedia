## **QMediaTrack** 媒体轨道资源
建议使用 mediaFactory createVideoTrack/createAudioTrack 创建
```objc
@interface QMediaTrack : NSObject

- (instancetype)initWithMediaSource:(id<QMediaSource>)mediaSource; //用媒体数据源初始化
- (bool) prepare;

- (bool)isPrepare;
- (int64_t)getSourceDuration;//获取数据源总时长

//生成音频数据节点， 成功后audio属性会被设置
- (bool)generateAudioTrackNode:(QCombiner*)combiner;
//生成视频图像节点， 成功后graphic属性会被设置
- (bool)generateVideoTrackNode:(QCombiner*)combiner;

@property (nonatomic, readonly) NSString* uid; //uuid资源索引用， 不需要关心
@property (nonatomic) NSRange sourceRange; //设置数据源的区间，可以使用这个参数实现视频片段提取
@property (nonatomic) NSRange displayRange; //设置媒体轨道展示时间区间， 不设置的话会用数据源的值
@property (nonatomic) float timeScale; //时间缩放参数(默认1.0f), 用来实现变速
@property (nonatomic) int repeatTimes; //设置重复播放(未实现)
@property (nonatomic, readonly) id<QMediaSource> source; //媒体数据源
@property (nonatomic, readonly) QVideoTrackNode* graphic; //视频节点， generateVideoTrackNode成功后设置。
@property (nonatomic, readonly) QAudioTrackNode* audio; //音频节点， generateAudioTrackNode成功后设置。

@end
```