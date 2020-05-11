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
    
    QLayer *layer = [[QLayer alloc] initWithSize:CGSizeMake(640, 480) name:@"haha"];
    [self.player addGraphicNode:layer];
    layer.color4 = QColorMaker(1, 1, 1, 0.8);
    layer.bkColor = QColorMaker(1, 0, 1, 1);
    layer.position = CGPointMake(100, 50);
    layer.anchorPoint = CGPointMake(0.5, 0.5);
    layer.contentSize = CGSizeMake(640, 480);
    layer.scaleX = 0.6f;
    layer.scaleY = 0.6f;
    layer.rotation = 30.0f;
    layer.renderRange = NSMakeRange(0, 10000);
    
    self.player.playerView = _renderView;
    self.player.rootNode.color4 = QColorMaker(0, 0, 1, 1);
//    self.player.graphicLayer.color4 = XMColorMaker(1, 0.5, 0.5, 0.5);
//    self.player.rootNode.rotation = 30;
//    self.player.rootNode.position = CGPointMake(-100, -100);
//    self.player.rootNode.anchorPoint = CGPointMake(0.5, 0.5);
    NSString* testVideoFile2 = [self getFileFromMainbundleAbsolutePath:@"video/test.mp4"];
    
    QMediaTrack* videoTrack = [self.player.mediaFactory createVideoTrack:testVideoFile2];
//    XMVideoTrack* videoTrack = [self.player.mediaFactory createOldVideoTrack:testVideoFile2];

//    QVideoTrackNode *videoNode = [[QVideoTrackNode alloc] initFromTrack:videoTrack];
    [self.player addGraphicNode:videoTrack.graphic];
        videoTrack.graphic.position = CGPointMake(50, 100);
        videoTrack.graphic.contentSize = CGSizeMake(640, 480);//self.player.layerSize;
        //    videoTrack.graphic.color4 = XMColorMaker(1, 1, 1, 0.5);
        videoTrack.graphic.anchorPoint = CGPointMake(0.5, 0.5);
        videoTrack.graphic.rotation = -30.0f;
    //    videoTrack.graphic.scaleX = 0.4f;
    //    videoTrack.graphic.scaleY = 0.4f;
//    videoTrack.sourceRange = NSMakeRange(5000, 10000);
    
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
    
    [self.player attachRenderNode:videoTrack.graphic parent:layer];
    [self.player attachRenderNode:layer parent:self.player.rootNode];
//    [layer addChildNode:videoNode];
//    [self.player.rootNode addChildNode:layer];
//    [self.player.rootNode addChildNode:captureTrack.graphic];
    
    QDuplicateNode* duplicatenode = [[QDuplicateNode alloc] initFromNode:layer];
    [self.player addGraphicNode:duplicatenode];
    duplicatenode.contentSize = CGSizeMake(320, 240);
    duplicatenode.position = CGPointMake(50, 50);
    duplicatenode.anchorPoint = CGPointMake(0.5, 0.5);
    QNodeAnimator* animator = [[QNodeAnimator alloc] initWith:property_scalexy range:NSMakeRange(0, 5000) begin:QVectorV2(0.5,0.5) end:QVectorV2(1,1) functype:Quint_EaseOut repleat:false];
    [duplicatenode addAnimator:animator];
    duplicatenode.renderRange = NSMakeRange(1000, 15000);
//    [self.player.rootNode addChildNode:duplicatenode];
    [self.player attachRenderNode:duplicatenode parent:self.player.rootNode];
    
    
    QDuplicateNode* duplicatenodeV = [[QDuplicateNode alloc] initFromNode:videoTrack.graphic];
    [self.player addGraphicNode:duplicatenodeV];
    duplicatenodeV.contentSize = CGSizeMake(200, 200);
    duplicatenodeV.position = CGPointMake(0, 0);
    duplicatenodeV.rotation = 30.0f;
//    [self.player.rootNode addChildNode:duplicatenodeV];
    [self.player attachRenderNode:duplicatenodeV parent:self.player.rootNode];
 
    [self.player addMediaTrack:videoTrack];
    
//    NSDictionary* serialize_settings = [self.player serialize];
//    NSError * error = [NSError new];
//    NSData  *jsonData = [NSJSONSerialization dataWithJSONObject:serialize_settings options:NSJSONWritingPrettyPrinted error:&error];
//    NSString* json_str = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
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

- (NSString*) getFileFromMainbundleAbsolutePath:(NSString*) fileCompent
{
    return [NSString stringWithFormat:@"%@/%@",[[NSBundle mainBundle] resourcePath], fileCompent];
}

@end


