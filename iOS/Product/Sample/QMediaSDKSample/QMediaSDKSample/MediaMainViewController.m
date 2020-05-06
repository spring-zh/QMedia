//
//  MediaMainViewController.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 03/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "MediaMainViewController.h"
#import "VideoRecorderViewController.h"
#import "GlobalXMObject.h"
#import "CommonKits.h"
#import <QMediaSDK/QMediaSDK.h>
//#import <XLMediaSDK/XMVideoReverse.h>
#import <AssetsLibrary/AssetsLibrary.h>
#import <MobileCoreServices/MobileCoreServices.h>
#import <AVFoundation/AVFoundation.h>
#import <MediaPlayer/MediaPlayer.h>

//#import "XMCommon.h"

@interface MediaMainViewController () <QEditorPlayerObserver, UINavigationControllerDelegate, UIImagePickerControllerDelegate, MPMediaPickerControllerDelegate, VideoRecorderViewControllerDelegate>
@property (nonatomic, weak) QEditorPlayer *player;
@property (nonatomic, weak) IBOutlet UINavigationItem *naviItem;
@property (nonatomic, retain) UINavigationItem *naviItemBack;

//@property (nonatomic, strong) XMFilter *particleFilter;


@end

@implementation MediaMainViewController

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    //[XMCommonUtils XMModule_init:[self getFileFromMainbundleAbsolutePath:@"video/log.log"]];
    self = [super initWithCoder:aDecoder];
    if (self) {
        NSString* testVideoFile = [self getFileFromMainbundleAbsolutePath:@"video/test.mp4"];
        NSString* testVideoFile2 = [self getFileFromMainbundleAbsolutePath:@"video/test.mp4"];
        NSString* testLRFile = [self getFileFromMainbundleAbsolutePath:@"audio/LR.mp3"];
        NSString* filterBrightness = [self getFileFromMainbundleAbsolutePath:@"video/brightness"];
        NSString* filterF1 = [self getFileFromMainbundleAbsolutePath:@"video/F1"];
        NSString* bitmap = [self getFileFromMainbundleAbsolutePath:@"video/ID_24.bmp"];
        
        NSString* testAudioFile = [self getFileFromMainbundleAbsolutePath:@"audio/audiotest.mp3"];
        NSString* testImageFile = [self getFileFromMainbundleAbsolutePath:@"image/li.jpg"];
        NSString* testImageFile2 = [self getFileFromMainbundleAbsolutePath:@"image/test.gif"];
        
        NSString* testStopwatchVideo = [self getFileFromMainbundleAbsolutePath:@"video/stopwatch.mp4"];
        
        
        [GlobalXMObject sharedInstance].selectedPixelSizeIndex = 2;
        
        self.player = [[GlobalXMObject sharedInstance] player];
        
//        XMComposedObject* composedObject = [GlobalXMObject sharedInstance];
        
//        CGSize dspSize = composedObject.pixelSize;
        
//        XMVideoObject* videoObject = [self addXMObjectOfTypeClass:[XMVideoObject class]
//                                                         filePath:testStopwatchVideo
//                                                       timeLayout:[self timeLayoutWithStartTime:0
//                                                                                  fileStartTime:0
//                                                                                       duration:4.033
//                                                                                          speed:1.0]
//                                                           layout:[self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 2)
//                                                                                    frameSize:dspSize
//                                                                                     rotation:0
//                                                                                        alpha:1.0
//                                                                              backgroundColor:nil]];
        
//        XMVideoTrack* videoTrack = [[XMMediaFactory sharedInstance] createVideoTrack:self.player filePath:testVideoFile2];
//        videoTrack.graphic.position = CGPointMake(0, 0);
//        videoTrack.graphic.contentSize = self.player.layerSize;
//        XMVideoObject* videoObject = [self addXMObjectOfTypeClass:[XMVideoObject class]
//                                                         filePath:testVideoFile2
//                                                       timeLayout:[self timeLayoutWithStartTime:0
//                                                                                  fileStartTime:0.0
//                                                                                       duration:10
//                                                                                          speed:1.0]
//                                                           layout:[self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 2)
//                                                                                    frameSize:dspSize
//                                                                                     rotation:0
//                                                                                        alpha:1.0
//                                                                              backgroundColor:nil]];

        
//        videoObject.dresser = [XMDresser new];
//        XMFilter* filter1 = [XMFilter filterWithPresetFilterId:XMPresetFILTER_ID_Beautify];
//        self.particleFilter = filter1;
//
//        //[filter1 addBitmap:0 setbitmap:bitmap2];
//        [videoObject.dresser addFilter:filter1];
        
        
        //        XMFilter* filter2 = [XMFilter filterWithPresetFilterId:XMPresetFilterId_GaussianBlur];
        //        //[filter addBitmap:0 setbitmap:bitmap];
        ////        filter.timeRange = XMTimeRangeMake(5.0, 2.0);
        //        [filter1 modifyAttr:@"texelWidthOffset" setvalue:0.0];
        //        [filter1 modifyAttr:@"texelHeightOffset" setvalue:2.0/ dspSize.height];
        //
        //        [filter2 modifyAttr:@"texelWidthOffset" setvalue:2.0/ dspSize.width];
        //        [filter2 modifyAttr:@"texelHeightOffset" setvalue:0.0];
        //[videoObject.dresser addFilter:filter1];
        
        
        
//        videoObject.performer = [XMPerformer new];
//        XMEffect* effect = [XMEffect effectWithPresetEffectId:XMPresetEffectId_ActionTrack
////
//                                                    timeRange:XMTimeRangeMake(0, 100)
//                            ];
//        [videoObject.performer addEffect:effect];
        
//        XMVideoObject* videoObject = [self addXMObjectOfTypeClass:[XMVideoObject class]
//                                                         filePath:testVideoFile
//                                                       timeLayout:[self timeLayoutWithStartTime:0
//                                                                                  fileStartTime:0.5
//                                                                                       duration:10
//                                                                                          speed:1.5]
//                                                           layout:[self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 2)
//                                                                                    frameSize:dspSize
//                                                                                     rotation:0
//                                                                                        alpha:1.0
//                                                                              backgroundColor:nil]];
//
//        videoObject.dresser = [XMDresser new];
    //filter = [XMFilter filterWithFilePath:filterF1 id:0 setbitmap:bitmap];
//        //[XMFilter filterWithFilePath:filterF1  1  filterF1];
//        
//        //[XMFilter filterWithPresetFilterId:XMPresetFilterId_Undefine];
//        
//        //filter.timeRange = XMTimeRangeMake(5.0, 2.0);
//        [videoObject.dresser addFilter:filter];

//        videoObject.performer = [XMPerformer new];
//        [videoObject.performer addEffect:[XMEffect effectWithPresetEffectId:XMPresetEffectId_Soul
//                                                                  timeRange:XMTimeRangeMake(0, 10)]];
        /*
        XMLayout* endLayout = [self layoutWithCenterPoint:CGPointMake(dspSize.width / 4, dspSize.height / 4)
                                                frameSize:CGSizeMake(dspSize.width*0.3, dspSize.height*0.3)
                                                 rotation:M_PI_4
                                                    alpha:0.5
                                          backgroundColor:[UIColor redColor]];
        XMLayout* endLayout2 = [self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 2)
                                                 frameSize:CGSizeMake(dspSize.width*1.5, dspSize.height*1.5)
                                                  rotation:M_PI
                                                     alpha:0.3
                                           backgroundColor:[UIColor blueColor]];
        XMLayout* endLayout3 = [self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 2)
                                                 frameSize:CGSizeMake(dspSize.width*0.8, dspSize.height*0.8)
                                                  rotation:-M_PI_4
                                                     alpha:0.7
                                           backgroundColor:[UIColor blackColor]];
        videoObject.animator = [XMAnimator new];
        videoObject.animator.layout = videoObject.layout;
        [videoObject.animator addAnimationWithRange:XMTimeRangeMake(3.0, 0.5)
                                     timingFuncType:XMTimingFunctionType_EasingIn
                                          endLayout:endLayout
                                    effectPropNames:nil];
        [videoObject.animator addAnimationWithRange:XMTimeRangeMake(4.0, 1.0)
                                          endLayout:endLayout2];
        [videoObject.animator addAnimationWithRange:XMTimeRangeMake(6.0, 0.5)
                                          endLayout:endLayout3];
        [videoObject.animator addAnimationWithRange:XMTimeRangeMake(8.0, 0.5)
                                     timingFuncType:XMTimingFunctionType_EasingOut
                                          endLayout:videoObject.layout
                                    effectPropNames:nil];
        */
        /*
        videoObject = [self addXMObjectOfTypeClass:[XMVideoObject class]
                                          filePath:testVideoFile2
                                        timeLayout:[self timeLayoutWithStartTime:0.0
                                                                   fileStartTime:0.0
                                                                        duration:10.0
                                                                           speed:0.9]
                                            layout:[self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 2)
                                                                     frameSize:CGSizeMake(dspSize.width, dspSize.height)
                                                                      rotation:0
                                                                         alpha:1.0
                                                               backgroundColor:nil
                                                            cropRect:CGRectZero//CGRectMake(0, 0, dspSize.width/2, dspSize.height)
                                                                flip:XMFLIP_NODEFINE//XMFLIP_HORIZONTAL
                                                            ]];
//                                            layout:[self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 3)
//                                                                     frameSize:CGSizeMake(dspSize.width*2/3, dspSize.height*2/3)
//                                                                      rotation:0.0
//                                                                         alpha:0.75
//                                                               backgroundColor:nil]];
//        videoObject.performer = [XMPerformer new];
//        XMEffect* effect = [XMEffect effectWithPresetEffectId:XMPresetEffectId_Toon
//
//                                                    timeRange:XMTimeRangeMake(0, 10)];
        
//        videoObject.timeMapper = [XMTimeMapper new];
//        [videoObject.timeMapper addSubMapper:1500 :500 :4 :XMTimingMapperMode_Repleat];
//        [videoObject.timeMapper addSubMapper:2800 :600 :3 :XMTimingMapperMode_Repleat];
//        
//        [videoObject.performer addEffect:effect];
//        NSLog(@"%@", effect.name);
//        
        

        
        */

//        videoObject.performer = [XMPerformer new];
//        XMEffect* effect = [XMEffect effectWithPresetEffectId:XMPresetEffectId_Signal
//                            
//                                                    timeRange:XMTimeRangeMake(0, 10) filePaths:@[bitmap2, bitmap3, bitmap4]];
//        [videoObject.performer addEffect:effect];
//        NSLog(@"%@", effect.name);
        
        
//        
//        float location[3] = {0.1, 0.5, 0.8};
//        float width[3] = {0.1, 0.15, 0.1};
//        float step_offset[3] = {0.1, 0.05, 0.07};
//        
//        //位移
//        videoObject.dresser = [XMDresser new];
//        XMFilter* offsetFilter = [XMFilter filterWithPresetFilterId:XMPresetFilterId_DISTURB];
//        [videoObject.dresser addFilter:offsetFilter];
        

        
//        XMImageObject* imageObject = [self addXMObjectOfTypeClass:[XMImageObject class]
//                                                         filePath:testImageFile
//                                                       timeLayout:[self timeLayoutWithStartTime:0.0
//                                                                                  fileStartTime:0.0
//                                                                                       duration:5.0
//                                                                                          speed:1.0]
//                                                           layout:[self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 2)
//                                                                                    frameSize:CGSizeMake(dspSize.width / 3, dspSize.height / 3)
//                                                                                     rotation:M_PI_4
//                                                                                        alpha:0.4
//                                                                              backgroundColor:nil]];
//        
//        [[GlobalXMObject sharedInstance] addSubObject:imageObject];
//        imageObject.duration = 5.0;
//        
//        endLayout = [self layoutWithCenterPoint:CGPointMake(dspSize.width / 4, dspSize.height / 4)
//                                      frameSize:CGSizeMake(dspSize.width*0.7, dspSize.height*0.7)
//                                       rotation:2*M_PI
//                                          alpha:0.8
//                                backgroundColor:[UIColor redColor]];
//        endLayout2 = [self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 2)
//                                       frameSize:dspSize
//                                        rotation:0
//                                           alpha:1.0
//                                 backgroundColor:[UIColor redColor]];
//        imageObject.animator = [XMAnimator new];
//        imageObject.animator.layout = imageObject.layout;
//        [imageObject.animator addAnimationWithRange:XMTimeRangeMake(1.0, 0.4)
//                                     timingFuncType:XMTimingFunctionType_Linear
//                                          endLayout:endLayout
//                                    effectPropNames:nil];
//        [imageObject.animator addAnimationWithRange:XMTimeRangeMake(2.5, 0.5)
//                                          endLayout:endLayout2];
//        
//        imageObject = [self addXMObjectOfTypeClass:[XMImageObject class]
//                                          filePath:testImageFile2
//                                        timeLayout:[self timeLayoutWithStartTime:1.5
//                                                                   fileStartTime:0.0
//                                                                        duration:13.0
//                                                                           speed:1.0]
//                                            layout:[self layoutWithCenterPoint:CGPointMake(187 / 2, - 233 / 2 + 7)
//                                                                     frameSize:CGSizeMake(187, 233)
//                                                                      rotation:0
//                                                                         alpha:0.3
//                                                               backgroundColor:nil]];
//        
//        endLayout = [self layoutWithCenterPoint:CGPointMake(187 / 2, dspSize.height - 233 / 2 + 7)
//                                      frameSize:CGSizeMake(187, 233)
//                                       rotation:6*M_PI
//                                          alpha:0.6
//                                backgroundColor:[UIColor redColor]];
//        endLayout2 = [self layoutWithCenterPoint:CGPointMake(dspSize.width - 187 / 2, dspSize.height - 233 / 2 + 7)
//                                       frameSize:CGSizeMake(187, 233)
//                                        rotation:6*M_PI
//                                           alpha:1.0
//                                 backgroundColor:[UIColor redColor]];
//        imageObject.animator = [XMAnimator new];
//        imageObject.animator.layout = imageObject.layout;
//        [imageObject.animator addAnimationWithRange:XMTimeRangeMake(0, 1.0)
//                                    timingFuncBlock:^CGFloat(CGFloat x) {
//                                        return x*x*x;
//                                    }
//                                          endLayout:endLayout
//                                    effectPropNames:nil];
//        [imageObject.animator addAnimationWithRange:XMTimeRangeMake(1.5, 9)
//                                          endLayout:endLayout2];
//        
//        imageObject = [self addXMObjectOfTypeClass:[XMImageObject class]
//                                          filePath:@""
//                                        timeLayout:[self timeLayoutWithStartTime:1.5
//                                                                   fileStartTime:0.0
//                                                                        duration:8.0
//                                                                           speed:1.0]
//                                            layout:[self layoutWithCenterPoint:CGPointMake(142, 246)
//                                                                     frameSize:CGSizeMake(142, 246)
//                                                                      rotation:0
//                                                                         alpha:1.0
//                                                               backgroundColor:nil]];
//        for (int i = 1; i < 15; ++i) {
//            NSString *filePath = [self getFileFromMainbundleAbsolutePath:[NSString stringWithFormat:@"image/fish/41864623_%d.png", i]];
//            [imageObject addImageWithFilePath:filePath duration:0.08];
//        }
//
        
//        XMTimeLayout* timeLayout = [self timeLayoutWithStartTime:0
//                                                   fileStartTime:15.3
//                                                        duration:10.0
//                                                           speed:1.0];
//        timeLayout.contentLoop = YES;
//        [self addXMObjectOfTypeClass:[XMAudioObject class]
//                            filePath:testLRFile
//                          timeLayout:timeLayout
//                              layout:nil];

//        composedObject.timeLayout = [self timeLayoutWithStartTime:0.0
//                                                    fileStartTime:0.0
//                                                         duration:2.279
//                                                            speed:1.0];
        
//        composedObject.dresser = [XMDresser new];
//        XMFilter* filter2 = [XMFilter filterWithPresetFilterId:XMPresetFilterId_ColorInvert];
//        //[filter1 addBitmap:0 setbitmap:bitmap2];
//        [composedObject.dresser addFilter:filter2];
//        composedObject.layout = [self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 2)
//                                                  frameSize:dspSize
//                                                   rotation:0
//                                                      alpha:1.0
//                                            backgroundColor:[UIColor greenColor]];
//        composedObject.performer = [XMPerformer new];
//        [composedObject.performer addEffect:[XMEffect effectWithPresetEffectId:XMPresetEffectId_Follower
//                                                                     timeRange:XMTimeRangeMake(0, 10)]];
        
//        composedObject.dresser = [XMDresser new];
//        XMFilter* filter4 = [XMFilter filterWithPresetFilterId:XMPresetFilterId_ColorInvert];
//        filter4.timeRange = XMTimeRangeMake(3.0, 2.0);
//        [composedObject.dresser addFilter:filter4];
        
//        [self.player addMediaTrack:videoTrack];
//        self.player = [XMPlayer sharedInstance];
//        self.player.objectToPlay = composedObject;
        [self.player addObserver:self];
        self.player.loopPlay = YES;
        
        //
//        [[XMPicFormater sharedInstance] setObjectToFormat:self.player];
//        [[XMPicFormater sharedInstance] setDelegate:self];
    }
    return self;
}

- (void)dealloc
{
//    [XMCommonUtils XMModule_destroy];
    //XMModule_destroy();
    [self.player removeObserver:self];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AddNewPoint" object:nil];
}

//- (XMTimeLayout*)timeLayoutWithStartTime:(CGFloat)startTime
//                           fileStartTime:(CGFloat)fileStartTime
//                                duration:(CGFloat)duration
//                                   speed:(CGFloat)speed
//{
//    XMTimeLayout* timeLayout = [XMTimeLayout new];
//    timeLayout.startTime = startTime;
//    timeLayout.fileStartTime = fileStartTime;
//    timeLayout.duration = duration;
//    timeLayout.timeScale = 1.0/speed;
//    return timeLayout;
//}

//- (XMLayout*)layoutWithCenterPoint:(CGPoint)centerPoint
//                         frameSize:(CGSize)frameSize
//                          rotation:(CGFloat)rotation
//                             alpha:(CGFloat)alpha
//                   backgroundColor:(UIColor*)backgroundColor
//{
//    XMLayout* layout = [XMLayout new];
//    layout.centerPoint = centerPoint;
//    layout.frameSize = frameSize;
//    layout.rotation = rotation;
//    layout.alpha = alpha;
//    layout.backgroundColor = backgroundColor;
//    return layout;
//}

//- (XMLayout*)layoutWithCenterPoint:(CGPoint)centerPoint
//                         frameSize:(CGSize)frameSize
//                          rotation:(CGFloat)rotation
//                             alpha:(CGFloat)alpha
//                   backgroundColor:(UIColor*)backgroundColor
//                          cropRect:(CGRect)cropRect
//{
//    XMLayout* layout = [XMLayout new];
//    layout.centerPoint = centerPoint;
//    layout.frameSize = frameSize;
//    layout.rotation = rotation;
//    layout.alpha = alpha;
//    layout.cropRect = cropRect;
//    layout.backgroundColor = backgroundColor;
//    return layout;
//}

//- (XMLayout*)layoutWithCenterPoint:(CGPoint)centerPoint
//                         frameSize:(CGSize)frameSize
//                          rotation:(CGFloat)rotation
//                             alpha:(CGFloat)alpha
//                   backgroundColor:(UIColor*)backgroundColor
//                          cropRect:(CGRect)cropRect
//                              flip:(XMFlipType)flip
//{
//    XMLayout* layout = [XMLayout new];
//    layout.centerPoint = centerPoint;
//    layout.frameSize = frameSize;
//    layout.rotation = rotation;
//    layout.alpha = alpha;
//    layout.cropRect = cropRect;
//    layout.flip = flip;
//    layout.backgroundColor = backgroundColor;
//    return layout;
//}


//- (__kindof XMObject*)addXMObjectOfTypeClass:(Class)typeClass
//                                    filePath:(NSString *)filePath
//                                  timeLayout:(XMTimeLayout*)timeLayout
//                                      layout:(XMLayout*)layout
//{
//    XMObject* object = [[typeClass alloc] initWithFilePath:filePath];
//    object.timeLayout = timeLayout;
//    object.layout = layout;
//    [[GlobalXMObject sharedInstance] addSubObject:object];
//    return object;
//}

//- (__kindof XMObject*)addXMImageObject:(XMTimeLayout*)timeLayout
//                                      layout:(XMLayout*)layout
//{
//    XMImageObject* object = [[XMImageObject alloc] init];
//    object.timeLayout = timeLayout;
//    object.layout = layout;
//    [[GlobalXMObject sharedInstance] addSubObject:object];
//    return object;
//}


- (void)onPlayerChangedObjectToPlay
{
    // 进入编辑
//    if (self.player.objectToPlay != [GlobalXMObject sharedInstance]) {
//        [self enterEditState:YES];
//    }
//    else {
//        [self enterEditState:NO];
//    }
}

- (void)enterEditState:(BOOL)edit
{
    if (edit) {
        self.naviItem.titleView = nil;
//        self.naviItem.title = self.player.objectToPlay.name;
        self.naviItem.leftBarButtonItem = nil;
        self.naviItem.rightBarButtonItems = nil;
    }
    else {
        self.naviItem.titleView = self.naviItemBack.titleView;
        self.naviItem.title = @"混合编辑";
        self.naviItem.leftBarButtonItem = self.naviItemBack.leftBarButtonItem;
        self.naviItem.rightBarButtonItems = self.naviItemBack.rightBarButtonItems;
    }
}

- (NSString*) getFileFromMainbundleAbsolutePath:(NSString*) fileCompent
{
    return [NSString stringWithFormat:@"%@/%@",[[NSBundle mainBundle] resourcePath], fileCompent];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.naviItemBack = [[UINavigationItem alloc] initWithTitle:@"正在编辑"];
    self.naviItemBack.titleView = self.naviItem.titleView;
    self.naviItemBack.leftBarButtonItem = self.naviItem.leftBarButtonItem;
    self.naviItemBack.rightBarButtonItems = self.naviItem.rightBarButtonItems;
    
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(addNewPoint:) name:@"AddNewPoint" object:nil];
    
}

- (void)addNewPoint:(NSNotification *)notiP
{
    NSValue *value = (NSValue *)[notiP object];
    CGPoint point = [value CGPointValue];
    
//    [self.particleFilter addNewPoint:point];
    
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
 #pragma mark - Navigation
 
 // In a storyboard-based application, you will often want to do a little preparation before navigation
 - (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
 // Get the new view controller using [segue destinationViewController].
 // Pass the selected object to the new view controller.
 }
 */

//获取视频文件的大小，返回的是单位是M。
- (CGFloat)getFileSize:(NSString *)path{
    NSFileManager *fileManager = [[NSFileManager alloc] init];
    float filesize = -1.0;
    if ([fileManager fileExistsAtPath:path]) {
        NSDictionary *fileDic = [fileManager attributesOfItemAtPath:path error:nil];//获取文件的属性
        unsigned long long size = [[fileDic objectForKey:NSFileSize] longLongValue];
        filesize = (1.0*size/1024)/1024.0;
        
    }
    return filesize;
}

//获取视频文件的时长。
- (CGFloat)getVideoLength:(NSURL *)URL{
    NSDictionary *opts = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:NO]
                                                     forKey:AVURLAssetPreferPreciseDurationAndTimingKey];
    AVURLAsset *urlAsset = [AVURLAsset URLAssetWithURL:URL options:opts];
    return CMTimeGetSeconds(urlAsset.duration);
}

//获取本地视频缩略图，网上说需要添加AVFoundation.framework
- (UIImage *)getImage:(NSURL *)URL{
    
    AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:URL options:nil];
    AVAssetImageGenerator *gen = [[AVAssetImageGenerator alloc] initWithAsset:asset];
    gen.appliesPreferredTrackTransform = YES;
    CMTime time = CMTimeMakeWithSeconds(0.0, 600);
    NSError *error = nil;
    CMTime actualTime;
    CGImageRef image = [gen copyCGImageAtTime:time actualTime:&actualTime error:&error];
    UIImage *thumb = [[UIImage alloc] initWithCGImage:image];
    CGImageRelease(image);
    
    return thumb;
}

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary<NSString *,id> *)info
{
    NSLog(@"%@", info);
    NSString *mediaType = [info objectForKey:UIImagePickerControllerMediaType];
    if ([mediaType isEqualToString:(NSString*) kUTTypeMovie] || [mediaType isEqualToString:(NSString*) kUTTypeVideo]) {
        NSURL *videoURL = [info objectForKey:UIImagePickerControllerMediaURL];
        NSLog(@"%@", videoURL);
        
//        XMVideoTrack* videoTrack = [[XMMediaFactory sharedInstance] createVideoTrack:self.player filePath:videoURL.absoluteString];
        QMediaTrack* videoTrack = [self.player.mediaFactory createVideoTrack:videoURL.absoluteString];
        [self.player addMediaTrack:videoTrack];
//        CGFloat duration = [self getVideoLength:videoURL];
//        CGSize dspSize = [GlobalXMObject sharedInstance].pixelSize;
//        XMObject* videoObject = [self addXMObjectOfTypeClass:[XMVideoObject class]
//                            filePath:videoURL.absoluteString
//                          timeLayout:[self timeLayoutWithStartTime:0
//                                                     fileStartTime:0.0
//                                                          duration:duration
//                                                             speed:1.0]
//                              layout:[self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 2)
//                                                       frameSize:CGSizeMake(dspSize.width, dspSize.height)
//                                                        rotation:0
//                                                           alpha:1.0
//                                                 backgroundColor:nil]];
        
//        videoObject.performer = [XMPerformer new];
//        [videoObject.performer addEffect:[XMEffect effectWithPresetEffectId:XMPresetEffectId_BWFlicker
//                                                                  timeRange:XMTimeRangeMake(0, 2)]];
//        [videoObject.performer addEffect:[XMEffect effectWithPresetEffectId:XMPresetEffectId_Split
//                                                                  timeRange:XMTimeRangeMake(2, 3)]];
//        [videoObject.performer addEffect:[XMEffect effectWithPresetEffectId:XMPresetEffectId_Shift
//                                                                  timeRange:XMTimeRangeMake(5, 2)]];
//        [videoObject.performer addEffect:[XMEffect effectWithPresetEffectId:XMPresetEffectId_Soul
//                                                                  timeRange:XMTimeRangeMake(7, 3)]];
//        [videoObject.performer addEffect:[XMEffect effectWithPresetEffectId:XMPresetEffectId_ColorShake
//                                                                  timeRange:XMTimeRangeMake(10, 2)]];
        
        
//        XMEffect* effect = [XMEffect effectWithPresetEffectId:XMPresetEffectId_Follower
//                                                    timeRange:XMTimeRangeMake(0, duration)];
//        [videoObject.performer addEffect:effect];
    }
    else if ([mediaType isEqualToString:(NSString*) kUTTypeImage]) {
        UIImage *image= [info objectForKey:UIImagePickerControllerOriginalImage];
        NSLog(@"Got a image %@", image);
        NSData* imageData = UIImageJPEGRepresentation(image, 1.0);
        NSString* imagePath = [NSString stringWithFormat:@"%@%ld.jpg", [CommonKits tmpPath], (long)[[NSDate date] timeIntervalSince1970]];
        [imageData writeToFile:imagePath atomically:NO];
//        CGSize dspSize = [GlobalXMObject sharedInstance].pixelSize;
//        XMImageObject* imageObject = [self addXMObjectOfTypeClass:[XMImageObject class]
//                                                         filePath:imagePath
//                                                       timeLayout:[self timeLayoutWithStartTime:0
//                                                                                  fileStartTime:0
//                                                                                       duration:2
//                                                                                          speed:1.0]
//                                                           layout:[self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 2)
//                                                                                    frameSize:dspSize
//                                                                                     rotation:0
//                                                                                        alpha:1.0
//                                                                              backgroundColor:nil]];
//        imageObject.duration = 2;
    }
    
    [picker dismissViewControllerAnimated:YES completion:nil];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [picker dismissViewControllerAnimated:YES completion:nil];
}

- (void)onVideoRecorderViewController:(VideoRecorderViewController*)controller
                      savedToFilePath:(NSString*)path
{
//    XMVideoTrack* videoTrack = [[XMMediaFactory sharedInstance] createVideoTrack:self.player filePath:path];
    QMediaTrack* videoTrack = [self.player.mediaFactory createVideoTrack:path];
    [self.player addMediaTrack:videoTrack];
//    CGFloat duration = [self getVideoLength:[NSURL fileURLWithPath:path]];
//    CGSize dspSize = [GlobalXMObject sharedInstance].pixelSize;
//    XMObject* videoObject = [self addXMObjectOfTypeClass:[XMVideoObject class]
//                        filePath:path
//                      timeLayout:[self timeLayoutWithStartTime:0.0
//                                                 fileStartTime:0.0
//                                                      duration:duration
//                                                         speed:1.0]
//                          layout:[self layoutWithCenterPoint:CGPointMake(dspSize.width / 2, dspSize.height / 2)
//                                                   frameSize:dspSize
//                                                    rotation:0
//                                                       alpha:1.0
//                                             backgroundColor:nil]];
    
    
    
//    videoObject.performer = [XMPerformer new];
//    [videoObject.performer addEffect:[XMEffect effectWithPresetEffectId:XMPresetEffectId_ActionTrack
//                                                              timeRange:XMTimeRangeMake(0, 10)]];
}

- (IBAction)onAddButtonPressed:(id)sender
{
    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:@"新建" message:nil preferredStyle:UIAlertControllerStyleActionSheet];
    UIAlertAction *action = [UIAlertAction actionWithTitle:@"拍摄"
                                                     style:UIAlertActionStyleDefault
                                                   handler:^(UIAlertAction * _Nonnull action) {
                                                       VideoRecorderViewController *recorderViewController = [[VideoRecorderViewController alloc] initWithNibName:@"VideoRecorderViewController" bundle:nil];
                                                       recorderViewController.delegate = self;
                                                       [self presentViewController:recorderViewController
                                                                          animated:YES
                                                                        completion:nil];
                                                       
                                                       [self.player pause];
                                                   }];
    [actionSheet addAction:action];
    
    action = [UIAlertAction actionWithTitle:@"视频"
                                      style:UIAlertActionStyleDefault
                                    handler:^(UIAlertAction * _Nonnull action) {
                                        UIImagePickerController *imagePicker = [[UIImagePickerController alloc] init];
                                        imagePicker.delegate = self;
                                        imagePicker.allowsEditing = NO;
                                        imagePicker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
                                        imagePicker.mediaTypes = @[(NSString*) kUTTypeMovie, (NSString*) kUTTypeVideo, (NSString*) kUTTypeAudio];
                                        [self presentViewController:imagePicker animated:YES completion:nil];
                                    }];
    [actionSheet addAction:action];
    
    action = [UIAlertAction actionWithTitle:@"图片"
                                      style:UIAlertActionStyleDefault
                                    handler:^(UIAlertAction * _Nonnull action) {
                                        UIImagePickerController *imagePicker = [[UIImagePickerController alloc] init];
                                        imagePicker.delegate = self;
                                        imagePicker.allowsEditing = NO;
                                        imagePicker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
                                        imagePicker.mediaTypes = @[(NSString*) kUTTypeImage];
                                        [self presentViewController:imagePicker animated:YES completion:nil];
                                    }];
    [actionSheet addAction:action];
    
    //    action = [UIAlertAction actionWithTitle:@"文字"
    //                                      style:UIAlertActionStyleDefault
    //                                    handler:^(UIAlertAction * _Nonnull action) {
    //
    //                                    }];
    //    [actionSheet addAction:action];
    
    action = [UIAlertAction actionWithTitle:@"取消"
                                      style:UIAlertActionStyleCancel
                                    handler:^(UIAlertAction * _Nonnull action) {
                                        
                                    }];
    [actionSheet addAction:action];
    
    
    //    action = [UIAlertAction actionWithTitle:@"倒序"
    //                                      style:UIAlertActionStyleDefault
    //                                    handler:^(UIAlertAction * _Nonnull action) {
    //                                        NSString* testVideoFile2 = [self getFileFromMainbundleAbsolutePath:@"video/test.mp4"];
    //                                        NSArray *pathArray = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    //                                        XMVideoReverse* reverser = [[XMVideoReverse alloc]initWithSourceFile:testVideoFile2
    //                                                                                                toOutputFile:[[pathArray objectAtIndex:0] stringByAppendingPathComponent:@"result.mp4"] range:CMTimeRangeMake(CMTimeMake(5, 1), CMTimeMake(5, 1))];
    //                                        reverser.retainAudio = YES;
    //                                        [reverser setDelegate:self callbackQueue:dispatch_get_main_queue()];
    //                                        [reverser start];
    //                                    }];
    //    [actionSheet addAction:action];
    
    
    [self presentViewController:actionSheet
                       animated:YES
                     completion:nil];
    
}

//-(void)videoReverse:(XMVideoReverse*)reverser statusChangedFrom:(XMVideoReverseStatus)oldStatus to:(XMVideoReverseStatus)newStatus withError:(NSError*)error
//{
//    //NSLog(@"reverse status changed from %d to %d", oldStatus, newStatus);
//    if (newStatus == XMVRS_complete) {
//        NSLog(@"reverse from %@ to %@ success", reverser.sourceFilePath, reverser.outputFilePath);
//        UISaveVideoAtPathToSavedPhotosAlbum(reverser.outputFilePath, nil, nil, nil);
//    }
//}

- (void)onCapture:(NSString*)fullname outImage:(UIImage*)image
{
    NSLog(@"onCapture fullname %s", [fullname UTF8String]);
    NSData * data = UIImageJPEGRepresentation(image,1.0f);
    [data writeToFile:fullname atomically:YES];
}

@end
