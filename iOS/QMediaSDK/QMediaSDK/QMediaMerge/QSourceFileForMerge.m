
#import <Foundation/Foundation.h>
#import "QSourceFileForMerge.h"

@interface QSourceFileForMerge()
{
    XMStreamInfoForMerge _videoStreamInfo;
    XMStreamInfoForMerge _audioStreamInfo;
}

@end

@implementation QSourceFileForMerge

@synthesize url;
@synthesize videoStreamInfo;
@synthesize audioStreamInfo;

-(instancetype)init
{
    self = [super init];
    _path = nil;
    _videoStreamInfo.enable = NO;
    _videoStreamInfo.isAbsTime = NO;
    _videoStreamInfo.destStartTime = kCMTimeZero;
    _videoStreamInfo.sourceTimeRange = CMTimeRangeMake(kCMTimeZero, kCMTimeIndefinite);
    _audioStreamInfo = _videoStreamInfo;
    return self;
}

-(NSURL*)url
{
    return [[NSURL alloc] initFileURLWithPath:_path];
}

-(XMStreamInfoForMerge*)videoStreamInfo
{
    return &_videoStreamInfo;
}

-(void) setVideoStreamInfo:(XMStreamInfoForMerge *)streamInfo
{
    _videoStreamInfo = *streamInfo;
}

-(XMStreamInfoForMerge*)audioStreamInfo
{
    return &_audioStreamInfo;
}

-(void) setAudioStreamInfo:(XMStreamInfoForMerge *)streamInfo
{
    _audioStreamInfo = *streamInfo;
}

@end
