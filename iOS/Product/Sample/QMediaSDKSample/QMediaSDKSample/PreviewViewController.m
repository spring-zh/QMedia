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
    
//    NSArray<QEffectConfig*>* filterConfigs = [QEffectManage getAllEffectConfig];
//    QEffect* colorInvert = [QEffectManage createEffect:@"FlashEffect"];
//    QEffect* polarPixellate = [QEffectManage createEffect:@"SobelEdgeDetection"];
//    [polarPixellate setFloatValue:@"edgeStrength" value:2];
//    colorInvert.renderRange = NSMakeRange(0, 5000);
//    polarPixellate.renderRange = NSMakeRange(3000, 10000);
    
    const int targetW = 640;
    const int targetH = 480;
    QAudioDescription* audio_setting = [[QAudioDescription alloc] initWithSmapleFormat:QAudioDescriptionFormatS16 nchannel:2 sampleRate:44100 sampleBits:16];
    [QMediaEngine configAudioOut: audio_setting];
    self.player.playerView = _renderView;
//    self.player.rootNode.color4 = QColorMake(0, 0, 1, 1);

    NSString* testVideoFile2 = [QFileUtils getFileFromMainbundleAbsolutePath:@"video/test.mp4"];
    NSString* testAudioFile = [QFileUtils getFileFromMainbundleAbsolutePath:@"audio/LR.mp3"];
    
    QMediaSegment* video_segment = [_player cresteMediaSegment:testVideoFile2 flag:QMediaSegmentFlagAll];
    [video_segment setDisplayRange:[QMediaRange mediaRangeWithStart:5000 end:15000]];
    [video_segment setTimescale:2];
    
    QMediaSegment* audio_segment = [_player cresteMediaSegment:testAudioFile flag:QMediaSegmentFlagAll];
//    [audio_segment setTimescale:0.6];
    
    QMediaSegment* video_segment2 = [_player cresteMediaSegment:testVideoFile2 flag:QMediaSegmentFlagAll];
    [video_segment2 setSourceRange:[QMediaRange mediaRangeWithStart:5000 end:10000]];
//    [video_segment2 setDisplayRange:[QMediaRange mediaRangeWithStart:5000 end:15000]];
    
    QVideoRenderNode* graphic = [video_segment getVideo];
    [graphic setPosition:[QPoint pointWithX:targetW/4 y:targetH/4]];
    [graphic setAnchorPoint:[QPoint pointWithX:0.5 y:0.5]];
    [graphic setContentSize:[QSize sizeWithWidth:targetW/2 height:targetH/2]];
//    [graphic setRotation:45];
    QAnimator* animator = [QAnimator animatorWithName:@"" property:QAnimatorRotation timerang:[QMediaRange mediaRangeWithStart:0 end:5000] startpoint:[QVec4f vec4fWithV1:0 v2:0 v3:0 v4:0] endpoint:[QVec4f vec4fWithV1:180 v2:0 v3:0 v4:0] easefunc:QEaseFunctionElasticEaseOut repleat:false];
    
    QAnimator* animator_alpha = [QAnimator animatorWithName:@"" property:QAnimatorAlpha timerang:[QMediaRange mediaRangeWithStart:0 end:2000] startpoint:[QVec4f vec4fWithV1:0 v2:0 v3:0 v4:0] endpoint:[QVec4f vec4fWithV1:1.0 v2:0 v3:0 v4:0] easefunc:QEaseFunctionLinear repleat:false];
    
    [graphic addAnimator:animator];
    [graphic addAnimator:animator_alpha];
    
    QVideoRenderNode* graphic2 = [video_segment2 getVideo];
    [graphic2 setContentSize:[QSize sizeWithWidth:targetW/2 height:targetH/2]];

    
    //add videoTrack
    [self.player addMediaSegment:video_segment];
    [self.player addMediaSegment:video_segment2];

    [self.player addMediaSegment:audio_segment];
    
    /*--------------------  GlobalXMObject  -----------------------*/
    for (QMediaSegment* segment in _player.segments) {
        [[GlobalXMObject sharedInstance] addTrack:segment];
    }
    
    [self.player setDisplayLayerSize:[QSize sizeWithWidth:targetW height:targetH]];
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
    if ([self.player isUserPaused] || self.player.state == XMPlayerState_Stopped) {
        [self.player play];
        [self.playButton setImage:[UIImage imageNamed:@"Edit Preview Pause_12x18_"]
        forState:UIControlStateNormal];
    }
    else {
        [self.player pause];
        
        [self.playButton setImage:[UIImage imageNamed:@"Edit Preview Play_13x18_"]
        forState:UIControlStateNormal];
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


