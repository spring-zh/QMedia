
## **QImageNode** 图像节点
图像节点， 用来显示图片或者水印  

### 定义如下
```objc
@interface QImageNode : QGraphicNode

- (instancetype)initWithPath:(NSString*)filePath combiner:(QCombiner*)combiner;
- (instancetype)initWithPath:(NSString*)filePath combiner:(QCombiner*)combiner uid:(NSString*)uid;
@property (nonatomic, readonly) NSString* filePath; //获取图像路径
@property (nonatomic, readonly) CGSize imgSize; //获取图像size

@end
```