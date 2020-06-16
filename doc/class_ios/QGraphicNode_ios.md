# 媒体渲染
图形渲染引擎是基于 **QGraphicNode** 组成的树状渲染结构 , 使用 **addChildNode** 添加子节点。 
使用者可以通过直接设置 **QGraphicNode** 属性参数或者使用动画器 **[QNodeAnimator](QNodeAnimator_ios.md)** 来改变或者实现自定义渲染过程  
目前可使用的不同类型节点有:   
视频渲染节点 **[QVideoTrackNode](QVideoTrackNode_ios.md)**  
图像节点 **[QImageNode](QImageNode_ios.md)**   
图层节点 **[QLayer](QLayer_ios.md)**   
复制节点 **[QDuplicateNode](QDuplicateNode_ios.md)**   

## **QGraphicNode** 图形节点资源
**QGraphicNode** : 所有图形节点的基类。 可以作为管理子节点单独存在。
```objc
@interface QGraphicNode : NSObject

- (instancetype)initWithName:(NSString*)name combiner:(QCombiner*)combiner;
- (instancetype)initWithName:(NSString*)name combiner:(QCombiner*)combiner uid:(NSString*)uid;
@property (nonatomic, readonly) NSString* name;

@property (nonatomic, weak, readonly) QGraphicNode* parent;
//for children nodes
@property (nonatomic, readonly) NSArray* childrens;

- (bool)addChildNode:(QGraphicNode*)childNode;
- (bool)removeChildNode:(QGraphicNode*)childNode;
- (void)clearAllChildrens;

//for animator
@property (nonatomic, readonly) NSArray<QNodeAnimator*>* animators;
- (bool)addAnimator:(QNodeAnimator*)animator;
- (bool)removeAnimator:(QNodeAnimator*)animator;
- (void)clearAllAnimators;

- (void)copyFrom:(QGraphicNode*)fromNode;

//uuid for index
@property (nonatomic, readonly) NSString* uid;
//设置渲染时间区间
@property (nonatomic) NSRange renderRange;

//transform propertys setting
@property (nonatomic) bool visible; //设置是否可见
@property (nonatomic) float rotation; //z轴旋转角度 (360一周)
@property (nonatomic) QVector rotation3d; //3d 转角度 (360一周)
@property (nonatomic) float scaleX; //x轴缩放, 默认1.0f
@property (nonatomic) float scaleY; //y轴缩放, 默认1.0f
@property (nonatomic) float scaleZ; //z轴缩放, 默认1.0f

//display size
@property (nonatomic) CGSize contentSize; //设置显示大小
@property (nonatomic) CGPoint position; //设置显示起点， 左上角原点
@property (nonatomic) float positionZ; //z轴坐标， 默认0

/**
 * 设置锚点, 影响旋转和缩放，默认(0,0) 左上角。
 * 设置为 (0.5,0.5) 为物体中心
 */
@property (nonatomic) CGPoint anchorPoint; 

//设置混合颜色, 也可以调节alpha
@property (nonatomic) QColor4 color4;
@property (nonatomic) float alpha;

//设置纹理裁剪区域， 取值范围0 ~ 1.0f. (0,0,1,1)表示全部
@property (nonatomic) QVector crop;

//自定义混合模式
@property (nonatomic) QBlendFunc blendFunc;

@end

```