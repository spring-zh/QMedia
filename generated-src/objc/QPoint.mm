// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#import "QPoint.h"


@implementation QPoint

- (nonnull instancetype)initWithX:(float)x
                                y:(float)y
{
    if (self = [super init]) {
        _x = x;
        _y = y;
    }
    return self;
}

+ (nonnull instancetype)pointWithX:(float)x
                                 y:(float)y
{
    return [(QPoint*)[self alloc] initWithX:x
                                          y:y];
}

- (BOOL)isEqual:(id)other
{
    if (![other isKindOfClass:[QPoint class]]) {
        return NO;
    }
    QPoint *typedOther = (QPoint *)other;
    return self.x == typedOther.x &&
            self.y == typedOther.y;
}

- (NSUInteger)hash
{
    return NSStringFromClass([self class]).hash ^
            (NSUInteger)self.x ^
            (NSUInteger)self.y;
}

// consturct with json
- (nonnull instancetype)initWithJson:(nonnull NSDictionary*)json {
    if (self = [super init]) {
        NSNumber* x = [json valueForKey:@"x"];
        if (x != nil) {
            _x = [x floatValue];
        }
        NSNumber* y = [json valueForKey:@"y"];
        if (y != nil) {
            _y = [y floatValue];
        }
    }
    return self;
}
// serialize to json
- (nonnull NSDictionary*) toJson {
    NSDictionary* root = [NSMutableDictionary new];
    [root setValue:@(self.x) forKey:@"x"];
    [root setValue:@(self.y) forKey:@"y"];
    return root;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@ %p x:%@ y:%@>", self.class, (void *)self, @(self.x), @(self.y)];
}

@end
