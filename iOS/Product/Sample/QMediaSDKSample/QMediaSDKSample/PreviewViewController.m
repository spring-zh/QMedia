//
//  PreviewViewController.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 03/05/2017.
//  Copyright © 2017 QMediaSDK. All rights reserved.
//

#import "PreviewViewController.h"
#import "GlobalXMObject.h"
#import <QMediaSDK/QMediaSDK.h>

@interface PreviewViewController () <QEditorPlayerObserver/*,XMKeyVideoReverserDelegate*/>
@property (nonatomic) IBOutlet QPlayerView *renderView;
@property (nonatomic) IBOutlet UIView *controlLayer;
@property (nonatomic) IBOutlet UIButton *playButton;
@property (nonatomic, weak) QEditorPlayer *player;
@end


@implementation PreviewViewController

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        self.player = [[GlobalXMObject sharedInstance] player];
        [self.player addObserver:self];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(onNotification:)
                                                     name:kGlobalXMObjectPixelSizeChanged
                                                   object:nil];
    }
    return self;
}

- (void)dealloc
{
    [self.player removeObserver:self];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSArray<QEffectConfig*>* filterConfigs = [QEffectManage getAllEffectConfig];
    QEffect* colorInvert = [QEffectManage createEffect:@"FlashEffect"];
    QEffect* polarPixellate = [QEffectManage createEffect:@"SobelEdgeDetection"];
    [polarPixellate setFloatValue:@"edgeStrength" value:2];
    colorInvert.renderRange = NSMakeRange(0, 5000);
    polarPixellate.renderRange = NSMakeRange(3000, 10000);
    
    const int targetW = 640;
    const int targetH = 480;
    
    self.player.playerView = _renderView;
//    self.player.rootNode.color4 = QColorMake(0, 0, 1, 1);

    QVideoDescribe* vdesc = [[QVideoDescribe alloc] initWithParamenters:QVideoCodecH264 framerate:25 width:targetW height:targetH bitrate:2*1024*1024];
    QAudioDescribe* adesc = [[QAudioDescribe alloc] initWithParamenters:QAudioCodecAAC rawFormat:QRawAudioFormatS16 samplerate:44100 nchannel:2 bitrate:128000];
    
    [self.player setAudioConfig:adesc];
    [self.player setVideoConfig:vdesc];
    NSString* testVideoFile2 = [QFileUtils getFileFromMainbundleAbsolutePath:@"video/test.mp4"];
    NSString* testAudioFile = [QFileUtils getFileFromMainbundleAbsolutePath:@"audio/LR.mp3"];
    
    QMediaTrack* videoTrack = [self.player.mediaFactory createVideoTrack:testVideoFile2 combiner:self.player];
    QMediaTrack* audioTrack = [self.player.mediaFactory createAudioTrack:testAudioFile combiner:self.player];

//    XMVideoTrack* videoTrack = [self.player.mediaFactory createOldVideoTrack:testVideoFile2];

//    QVideoTrackNode *videoNode = [[QVideoTrackNode alloc] initFromTrack:videoTrack];
        videoTrack.graphic.position = CGPointMake(targetW/4, targetH/4);
        videoTrack.graphic.contentSize = CGSizeMake(targetW/2, targetH/2);//self.player.layerSize;
        //    videoTrack.graphic.color4 = XMColorMaker(1, 1, 1, 0.5);
//        videoTrack.graphic.anchorPoint = CGPointMake(0.5, 0.5);
//        videoTrack.graphic.rotation = -30.0f;
    //    videoTrack.graphic.scaleX = 0.4f;
    //    videoTrack.graphic.scaleY = 0.4f;
//    videoTrack.sourceRange = NSMakeRange(5000, 10000);
//    videoTrack.timeScale = 1.2f;
    
#if 0 //don't use CaptureTrack in edit mode
    QMediaTrack* captureTrack = [self.player.mediaFactory createCaptureTrack:AVCaptureSessionPreset640x480 position:AVCaptureDevicePositionBack video:true audio:false];
    [self.player addMediaTrack:captureTrack];
    [self.player addGraphicNode:captureTrack.graphic];
    [captureTrack setDisplayTrackRange:NSMakeRange(0, 20000)];
    captureTrack.sourceRange = NSMakeRange(0, 20000);
    captureTrack.graphic.contentSize = CGSizeMake(320, 240);
    captureTrack.graphic.position = CGPointMake(400, 0);
    [self.player attachRenderNode:captureTrack.graphic parent:self.player.rootNode];
#endif
    
    QDuplicateNode* duplicatenodeL = [[QDuplicateNode alloc] initFromNode:videoTrack.graphic combiner:self.player];
    duplicatenodeL.contentSize = CGSizeMake(targetW/2, targetH/2);
    duplicatenodeL.position = CGPointMake(0, targetH/4);
    duplicatenodeL.anchorPoint = CGPointMake(0.5, 0.5);
    duplicatenodeL.rotation3d = QVectorV3(0, 90, 0);

    QDuplicateNode* duplicatenodeR = [[QDuplicateNode alloc] initFromNode:videoTrack.graphic combiner:self.player];
    duplicatenodeR.contentSize = CGSizeMake(targetW/2, targetH/2);
    duplicatenodeR.position = CGPointMake(targetW/2, targetH/4);
    duplicatenodeR.anchorPoint = CGPointMake(0.5, 0.5);
    duplicatenodeR.rotation3d = QVectorV3(0, -90, 0);
    
    //add videoTrack
    [self.player addMediaTrack:videoTrack];
    //add audioTrack
    [self.player addMediaTrack:audioTrack];
    
    QGraphicNode* composeNode = [[QGraphicNode alloc] initWithName:@"composeNode" combiner:self.player];
    composeNode.contentSize = CGSizeMake(targetW, targetH);
    composeNode.anchorPoint = CGPointMake(0.5, 0.5);
    QNodeAnimator * an1 = [[QNodeAnimator alloc] initWith:property_rotationxyz range:QTimeRangeMake(0, 5000) begin:QVectorV3(0, 0, 0) end:QVectorV3(-180, 180, 180) functype:Linear repleat:false];
    [composeNode addAnimator:an1];
    QNodeAnimator * an2 = [[QNodeAnimator alloc] initWith:property_rotationxyz range:QTimeRangeMake(5000, 10000) begin:QVectorV3(-180, 180, 180) end:QVectorV3(-360, 360, 360) functype:Linear repleat:false];
    [composeNode addAnimator:an2];
    [composeNode addChildNode:videoTrack.graphic];
    [composeNode addChildNode:duplicatenodeL];
    [composeNode addChildNode:duplicatenodeR];
    
//    QLayer *layer = [[QLayer alloc] initWithSize:CGSizeMake(targetW, targetH) combiner:self.player];
//    layer.color4 = QColorMake(1, 1, 1, 0.8);
//    layer.bkColor = QColorMake(1, 0, 1, 1);
//    layer.enable3d = true;
//    layer.anchorPoint = CGPointMake(0.5, 0.5);
//    layer.contentSize = CGSizeMake(targetW, targetH);
//    layer.renderRange = NSMakeRange(0, 10000);
//    layer.rotation = 60;
//    layer.positionZ = 10;
    
    NSString* testImageFile = [QFileUtils getFileFromMainbundleAbsolutePath:@"image/li.jpg"];
    QImageNode* imageNode = [[QImageNode alloc] initWithPath:testImageFile combiner:self.player];
    imageNode.contentSize = CGSizeMake(480, 360);
    imageNode.anchorPoint = CGPointMake(0.5, 0.5);
    imageNode.renderRange = QTimeRangeMake(0, 10000);
    imageNode.rotation = 60;
    imageNode.positionZ = 10;
//    [layer addEffect:colorInvert];
//    [layer addEffect:polarPixellate];
//    layer.bkColor = QColorMake(1, 0, 1, 1);
    
//    [layer addChildNode:imageNode];
//    [self.player.rootNode addChildNode:imageNode];
//    [layer addChildNode:composeNode];
//    [self.player.rootNode addChildNode:layer];
    self.player.rootNode.enable3d = true;
//    [self.player.rootNode addEffect:colorInvert];
    [self.player.rootNode addEffect:polarPixellate];
    [self.player.rootNode addChildNode:composeNode];
    self.player.rootNode.anchorPoint = CGPointMake(0.5, 0.5);
    
    [self.player start];
    
    UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panAction:)];
    [self.controlLayer addGestureRecognizer:panGesture];
    
    
    // Do any additional setup after loading the view.
    
}

- (void)panAction:(UIPanGestureRecognizer *)panGesture
{
    CGPoint locationPoint = [panGesture locationInView:self.controlLayer];
    
    locationPoint.x = locationPoint.x / self.controlLayer.bounds.size.width;
    locationPoint.y = 1.0 - locationPoint.y / self.controlLayer.bounds.size.height;
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"AddNewPoint" object:[NSValue valueWithCGPoint:locationPoint]];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
//    [self setRenderPixelSize:[GlobalXMObject sharedInstance].pixelSize];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)setRenderPixelSize:(CGSize)size
{
    if (size.width == 0 || size.height == 0) {
        return ;
    }
    CGSize containerSize = self.view.bounds.size;
    CGSize resultSize = containerSize;
    if (containerSize.width/containerSize.height > size.width/size.height) {
        resultSize.width = containerSize.height*size.width/size.height;
    }
    else {
        resultSize.height = containerSize.width*size.height/size.width;
    }
    self.renderView.frame = CGRectMake((containerSize.width-resultSize.width)/2,
                                       (containerSize.height-resultSize.height)/2,
                                       resultSize.width,
                                       resultSize.height);
}

-(void)onNotification:(NSNotification *)notification{
    NSString *name = [notification name];
//    if ([name isEqualToString:kGlobalXMObjectPixelSizeChanged]) {
//        [self setRenderPixelSize:[GlobalXMObject sharedInstance].pixelSize];
//    }
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

- (void)onPrepare
{
//    [self.player play];
}

- (void)onPlayerChangedFromOldState:(NSNumber*)xmsOldState
                         toNewState:(NSNumber*)xmsNewState
{
    QEditorPlayerState newState = (QEditorPlayerState)[xmsNewState integerValue];
    dispatch_async(dispatch_get_main_queue(), ^{
        if (newState == XMPlayerState_Playing) {
            [self.playButton setImage:[UIImage imageNamed:@"Edit Preview Pause_12x18_"]
                             forState:UIControlStateNormal];
        }
        else {
            [self.playButton setImage:[UIImage imageNamed:@"Edit Preview Play_13x18_"]
                             forState:UIControlStateNormal];
        }
    });
}



- (IBAction)onPlayPauseButton:(UIButton *)sender
{
    if (self.player.isPaused || self.player.state == XMPlayerState_Stopped) {
        [self.player play];
    }
    else {
        [self.player pause];
    }
    //test testVerseVideo,added by yuelei
    //[self testVerseVideo ];
    
}

- (IBAction)onPreviewViewTapped:(id)sender
{
    if (self.controlLayer.hidden) {
        self.controlLayer.alpha = 0;
        self.controlLayer.hidden = NO;
        [UIView animateWithDuration:0.3
                         animations:^{
                             self.controlLayer.alpha = 1.0;
                         }
                         completion:^(BOOL finished) {
                             
                         }];
    }
    else {
        [UIView animateWithDuration:0.3
                         animations:^{
                             self.controlLayer.alpha = 0;
                         }
                         completion:^(BOOL finished) {
                             self.controlLayer.hidden = YES;
                         }];
    }
}

- (void)onReverseProgressUpdated:(int)progress{
    NSLog(@"working *********** %d",progress);
    
}

- (void)onReverseCompleted:(BOOL)isSuccess{
    NSLog(@"working --- %@",(isSuccess?@"complete":@"failed"));
}
- (void)testVerseVideo{
    NSString* src = [NSString stringWithFormat:@"%@/%@",[[NSBundle mainBundle] resourcePath], @"video/140m.mp4"];
    NSString* dir = NSTemporaryDirectory();
    NSString* file = @"IMGorigin_out.mp4";
    NSString* dstVideo = [dir stringByAppendingString:file];
    //文件名
    BOOL blHave=[[NSFileManager defaultManager] fileExistsAtPath:dstVideo];
    if (!blHave) {
        NSLog(@"no  have");
    }else {
        NSLog(@" have");
        BOOL blDele= [[NSFileManager defaultManager] removeItemAtPath:dstVideo error:nil];
        if (blDele) {
            NSLog(@"dele success");
        }else {
            NSLog(@"dele fail");
        }
    }
    
    
//    XMKeyVideoReverser* reverser = [XMKeyVideoReverser sharedInstance];
//    [reverser cancel];
//    [reverser reverse:src dst:dstVideo beginMs:0 endMs:70000 isCopyAudio:TRUE delegate:self];
}

@end


