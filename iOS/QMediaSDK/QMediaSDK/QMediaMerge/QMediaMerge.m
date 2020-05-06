
#import "QMediaMerge.h"
#import "QSourceFileForMerge.h"

@interface QMediaMerge()
{
    NSMutableArray< QSourceFileForMerge* >* _sourceFileInfoArray;
    __weak id<QMediaMergeDelegate> _delegate;
    dispatch_queue_t _delegateCallbackQueue;
    dispatch_queue_t _workQueue;
    AVAssetExportSession* _assetExport;
}

@end

@implementation QMediaMerge

- (instancetype)initWithOutputFilePath:(NSString*)outputFilePath
{
    self = [super init];
    _outputFilePath = outputFilePath;
    _workQueue = dispatch_queue_create("com.apple.merge.workqueue", DISPATCH_QUEUE_SERIAL);
    _sourceFileInfoArray = [[NSMutableArray< QSourceFileForMerge* > alloc] init];
    _assetExport = nil;
    return self;
}

- (BOOL) addSourceFile:(QSourceFileForMerge*)sourceFile
{
    if (sourceFile == nil) {
        return NO;
    }
    
    [_sourceFileInfoArray addObject:sourceFile];
    return YES;
}

- (void) setDelegate:(id<QMediaMergeDelegate>)delegate
       callbackQueue:(dispatch_queue_t)delegateCallbackQueue
{
    _delegate = delegate;
    _delegateCallbackQueue = delegateCallbackQueue;
}

- (BOOL) addStream:(XMStreamInfoForMerge*)streamInfo
                             andURLAsset:(AVURLAsset*)urlAsset
                      toCompositionTrack:(AVMutableCompositionTrack**)compositionTrack
                          andMixComposition:(AVMutableComposition*)mixComposition
                           withMediaType:(NSString*)mediaType
                                  atTime:(CMTime*)nextFileTime
{
    if (streamInfo->enable) {
        if (CMTimeCompare(streamInfo->sourceTimeRange.duration, kCMTimeZero) == 0) {
            streamInfo->sourceTimeRange.duration = urlAsset.duration;
        }
        
        if (!streamInfo->isAbsTime) {
            streamInfo->isAbsTime = NO;
            streamInfo->destStartTime = CMTimeAdd(*nextFileTime, streamInfo->offsetFromLastStreamFile);
        }

        NSError* error = nil;
        BOOL addTrackOk = NO;
        NSArray<AVAssetTrack *>* videoTracks = [urlAsset tracksWithMediaType:mediaType];
        if (videoTracks && videoTracks.count > 0) {
            if (*compositionTrack == nil) {
                *compositionTrack = [mixComposition addMutableTrackWithMediaType:mediaType preferredTrackID:kCMPersistentTrackID_Invalid];
            }
            
            addTrackOk = [*compositionTrack insertTimeRange:streamInfo->sourceTimeRange ofTrack:[videoTracks objectAtIndex:0]
                                                         atTime:streamInfo->destStartTime error:&error];
            if (!addTrackOk) {
                if (_delegateCallbackQueue && _delegate) {
                    dispatch_async(_delegateCallbackQueue, ^{
                        [_delegate merge:self failedWithError:error];
                    });
                }
                return NO;
            }
            else
            {
                *nextFileTime = CMTimeAdd(streamInfo->destStartTime, streamInfo->sourceTimeRange.duration);
            }
        }
    }
    return YES;
}

- (void) merge
{
    NSDate* startDate = [NSDate date];

    dispatch_async(_workQueue, ^{
        // 构建AVMutableComposition
        AVMutableComposition* mixComposition = [AVMutableComposition composition];
        AVMutableCompositionTrack* videoCompositionTrack = nil;
        AVMutableCompositionTrack* audioCompositionTrack = nil;
        CMTime nextFileVideoStartTime = kCMTimeZero;
        CMTime nextFileAudioStartTime = kCMTimeZero;
        
        for (NSInteger i = 0; i < _sourceFileInfoArray.count; i++) {
            AVURLAsset* urlAsset = [[AVURLAsset alloc] initWithURL:_sourceFileInfoArray[i].url options:nil];
            XMStreamInfoForMerge* videoStreamInfo = _sourceFileInfoArray[i].videoStreamInfo;
            XMStreamInfoForMerge* audioStreamInfo = _sourceFileInfoArray[i].audioStreamInfo;
            if (![self addStream:videoStreamInfo andURLAsset:urlAsset toCompositionTrack:&videoCompositionTrack andMixComposition:mixComposition withMediaType:AVMediaTypeVideo atTime:&nextFileVideoStartTime]) {
                return;
            }

            if (![self addStream:audioStreamInfo andURLAsset:urlAsset toCompositionTrack:&audioCompositionTrack andMixComposition:mixComposition withMediaType:AVMediaTypeAudio atTime:&nextFileAudioStartTime]) {
                return;
            }
        }
        
        // 开始存储
        _assetExport = [[AVAssetExportSession alloc] initWithAsset:mixComposition presetName:AVAssetExportPresetPassthrough];
        _assetExport.outputURL = [[NSURL alloc] initFileURLWithPath:_outputFilePath];
        _assetExport.outputFileType = AVFileTypeMPEG4;
        [_assetExport exportAsynchronouslyWithCompletionHandler:^{
            switch (_assetExport.status) {
                case AVAssetExportSessionStatusFailed:
                    if (_delegateCallbackQueue && _delegate) {
                        dispatch_async(_delegateCallbackQueue, ^{
                            [_delegate merge:self failedWithError:_assetExport.error];
                        });
                    }
                    break;
                case AVAssetExportSessionStatusCompleted:
                    if (_delegateCallbackQueue && _delegate) {
                        dispatch_async(_delegateCallbackQueue, ^{
                            double deltaTime = [[NSDate date] timeIntervalSinceDate:startDate];
                            [_delegate merge:self finishedWithDuration:deltaTime];
                        });
                    }
                    break;
                case AVAssetExportSessionStatusCancelled:
                    if (_delegateCallbackQueue && _delegate) {
                        dispatch_async(_delegateCallbackQueue, ^{
                            [_delegate mergeCanceled:self];
                        });
                    }
                    break;
                default:
                    NSAssert(NO, @"invalid status when mediamerge");
                    break;
            }
        }];
    });
}

- (void) cancel
{
    if (_assetExport) {
        dispatch_async(_workQueue, ^{
            if (_assetExport) {
                [_assetExport cancelExport];
            }
        });
    }
}

@end
