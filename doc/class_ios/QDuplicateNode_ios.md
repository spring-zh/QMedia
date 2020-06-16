
## **QDuplicateNode** 复制节点
复制节点， 可以复制其他 QGraphicNode 的渲染结果。 实现同个图形节点多重渲染

### 定义如下
```objc
@interface QDuplicateNode : QGraphicNode

/**
 * @nodeFrom : 渲染引用的节点对象
*/
- (instancetype)initFromNode:(QGraphicNode*)nodeFrom combiner:(QCombiner*)combiner;
- (instancetype)initFromNode:(QGraphicNode*)nodeFrom combiner:(QCombiner*)combiner uid:(NSString*)uid;
@property (nonatomic, readonly) QGraphicNode* nodeFrom; //引用的节点对象
@property (nonatomic) QColor4 bkColor;

@end
```