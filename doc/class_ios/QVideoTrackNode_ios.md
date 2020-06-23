
## **QVideoTrackNode** 视频渲染节点
为媒体轨道QMediaTrack 生成、持有。用于展示视频渲染的节点
```objc
@interface QVideoTrackNode : QGraphicNode

- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack combiner:(QCombiner*)combiner;
- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack combiner:(QCombiner*)combiner uid:(NSString*)uid;
@property (nonatomic, readonly) QMediaTrack* mediaTrack;

@end
```