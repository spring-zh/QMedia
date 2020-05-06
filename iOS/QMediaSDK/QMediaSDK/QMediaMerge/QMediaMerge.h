//
//  QMediaMerge.h
//  QMediaSDK
//  合并视频文件，暂时不支持合并中途增删原始数据文件
//  Created by zhen zhang on 2017/4/28.
//  Copyright © 2017年 QMedia. All rights reserved.
//

#ifndef QMEDIAMERGE_H
#define QMEDIAMERGE_H

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@protocol QMediaMergeDelegate;

@class QSourceFileForMerge;

@interface QMediaMerge : NSObject

@property(readonly) NSString* outputFilePath; // 合并输出文件格式

// 初始化
- (instancetype)initWithOutputFilePath:(NSString*)outputFilePath;

// 添加要合并的源文件描述
- (BOOL) addSourceFile:(QSourceFileForMerge*)sourceFile;

// 设置delegate以监听合并结果，需要在merge开始前设置
- (void) setDelegate:(id<QMediaMergeDelegate>)delegate
       callbackQueue:(dispatch_queue_t)delegateCallbackQueue;

// 开始合并
- (void) merge;

// 取消合并
- (void) cancel;
@end

@protocol QMediaMergeDelegate <NSObject>
// 合并完成，duration表示合并消耗的时间，单位：S
- (void) merge:(QMediaMerge*)merger finishedWithDuration:(double)duration;
// 取消
- (void) mergeCanceled:(QMediaMerge*)merger;
// 失败
- (void) merge:(QMediaMerge*)merger failedWithError:(NSError*)error;
@end

#endif /* QMEDIAMERGE_H */
