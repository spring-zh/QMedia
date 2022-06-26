// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#import "QVec3f.h"


@implementation QVec3f

- (nonnull instancetype)initWithV1:(float)v1
                                v2:(float)v2
                                v3:(float)v3
{
    if (self = [super init]) {
        _v1 = v1;
        _v2 = v2;
        _v3 = v3;
    }
    return self;
}

+ (nonnull instancetype)vec3fWithV1:(float)v1
                                 v2:(float)v2
                                 v3:(float)v3
{
    return [(QVec3f*)[self alloc] initWithV1:v1
                                          v2:v2
                                          v3:v3];
}

- (BOOL)isEqual:(id)other
{
    if (![other isKindOfClass:[QVec3f class]]) {
        return NO;
    }
    QVec3f *typedOther = (QVec3f *)other;
    return self.v1 == typedOther.v1 &&
            self.v2 == typedOther.v2 &&
            self.v3 == typedOther.v3;
}

- (NSUInteger)hash
{
    return NSStringFromClass([self class]).hash ^
            (NSUInteger)self.v1 ^
            (NSUInteger)self.v2 ^
            (NSUInteger)self.v3;
}

// consturct with json
- (nonnull instancetype)initWithJson:(nonnull NSDictionary*)json {
    if (self = [super init]) {
        NSNumber* v1 = [json valueForKey:@"v1"];
        if (v1 != nil) {
            _v1 = [v1 floatValue];
        }
        NSNumber* v2 = [json valueForKey:@"v2"];
        if (v2 != nil) {
            _v2 = [v2 floatValue];
        }
        NSNumber* v3 = [json valueForKey:@"v3"];
        if (v3 != nil) {
            _v3 = [v3 floatValue];
        }
    }
    return self;
}
// serialize to json
- (nonnull NSDictionary*) toJson {
    NSDictionary* root = [NSMutableDictionary new];
    [root setValue:@(self.v1) forKey:@"v1"];
    [root setValue:@(self.v2) forKey:@"v2"];
    [root setValue:@(self.v3) forKey:@"v3"];
    return root;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@ %p v1:%@ v2:%@ v3:%@>", self.class, (void *)self, @(self.v1), @(self.v2), @(self.v3)];
}

@end