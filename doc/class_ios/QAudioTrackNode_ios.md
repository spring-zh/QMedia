
## **QAudioTrackNode** 音频节点
为媒体轨道QMediaTrack 生成、持有。用于输出和设置音频参数。包括  
音量控制，音频变调
```objc
@interface QAudioTrackNode : NSObject

- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack combiner:(QCombiner*)combiner;
- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack combiner:(QCombiner*)combiner uid:(NSString*)uid;
@property (nonatomic, readonly) QMediaTrack* mediaTrack;

//uuid for index
@property (nonatomic, readonly) NSString* uid;

@property (nonatomic) bool enable;
@property (nonatomic) float volume; //设置音量

@end
```