// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#import "QColor4f.h"


@implementation QColor4f

- (nonnull instancetype)initWithR:(float)r
                                g:(float)g
                                b:(float)b
                                a:(float)a
{
    if (self = [super init]) {
        _r = r;
        _g = g;
        _b = b;
        _a = a;
    }
    return self;
}

+ (nonnull instancetype)color4fWithR:(float)r
                                   g:(float)g
                                   b:(float)b
                                   a:(float)a
{
    return [(QColor4f*)[self alloc] initWithR:r
                                            g:g
                                            b:b
                                            a:a];
}

- (BOOL)isEqual:(id)other
{
    if (![other isKindOfClass:[QColor4f class]]) {
        return NO;
    }
    QColor4f *typedOther = (QColor4f *)other;
    return self.r == typedOther.r &&
            self.g == typedOther.g &&
            self.b == typedOther.b &&
            self.a == typedOther.a;
}

- (NSUInteger)hash
{
    return NSStringFromClass([self class]).hash ^
            (NSUInteger)self.r ^
            (NSUInteger)self.g ^
            (NSUInteger)self.b ^
            (NSUInteger)self.a;
}

// consturct with json
- (nonnull instancetype)initWithJson:(nonnull NSDictionary*)json {
    if (self = [super init]) {
        NSNumber* r = [json valueForKey:@"r"];
        if (r != nil) {
            _r = [r floatValue];
        }
        NSNumber* g = [json valueForKey:@"g"];
        if (g != nil) {
            _g = [g floatValue];
        }
        NSNumber* b = [json valueForKey:@"b"];
        if (b != nil) {
            _b = [b floatValue];
        }
        NSNumber* a = [json valueForKey:@"a"];
        if (a != nil) {
            _a = [a floatValue];
        }
    }
    return self;
}
// serialize to json
- (nonnull NSDictionary*) toJson {
    NSDictionary* root = [NSMutableDictionary new];
    [root setValue:@(self.r) forKey:@"r"];
    [root setValue:@(self.g) forKey:@"g"];
    [root setValue:@(self.b) forKey:@"b"];
    [root setValue:@(self.a) forKey:@"a"];
    return root;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@ %p r:%@ g:%@ b:%@ a:%@>", self.class, (void *)self, @(self.r), @(self.g), @(self.b), @(self.a)];
}

@end