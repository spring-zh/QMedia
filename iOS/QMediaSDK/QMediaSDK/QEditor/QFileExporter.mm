//
//  QExporter.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QFileExporter.h"
#import "QFileExporterTarget.h"

@implementation QFileExporter {
    QFileExporterTarget* _fileTarget;
    NSString *_filePath;
}

- (instancetype)initWithPath:(NSString*)filePath {
    _filePath = [filePath copy];
    _fileTarget = [[QFileExporterTarget alloc] initWithPath:filePath];
    return [self initWithTarget:_fileTarget audioTarget:_fileTarget];
}
- (instancetype)initWithPath:(NSString*)filePath cbQueue:(dispatch_queue_t)cbQueue {
    _filePath = [filePath copy];
    _fileTarget = [[QFileExporterTarget alloc] initWithPath:filePath];
    return [self initWithTarget:_fileTarget audioTarget:_fileTarget cbQueue:cbQueue];
}

- (void)dealloc {
    NSLog(@"%@ dealloc ", self);
}

- (NSString*)filePath {
    return _filePath;
}

@end
