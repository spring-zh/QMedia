//
//  QFileExporter.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QExporter.h"

@interface QFileExporter : QExporter

- (instancetype)initWithPath:(NSString*)filePath;
- (instancetype)initWithPath:(NSString*)filePath cbQueue:(dispatch_queue_t)cbQueue;

@property (nonatomic, readonly) NSString* filePath;

@end
