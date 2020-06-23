
## **QLayer** 图层节点
图层节点， 拥有自己的渲染场景管理。 可用于多种功能：
1. 用来实现先渲染子场景，再把子场景渲染结果渲染到主场景上。
2. 可以实现纹理格式统一转换， 将不同纹理格式或者渲染结果转换成rgba纹理
3. 滤镜添加和管理
4. 纹理或者pixel数据获取  

### 定义如下
```objc
@interface QLayer : QGraphicNode

- (instancetype)initWithSize:(CGSize)size combiner:(QCombiner*)combiner;
- (instancetype)initWithSize:(CGSize)size combiner:(QCombiner*)combiner uid:(NSString*)uid;
@property (nonatomic, readonly) CGSize layerSize; //图层缓冲区大小 (注: contentSize 为显示大小) 
@property (nonatomic) QColor4 bkColor; //设置图层背景颜色

@end
```