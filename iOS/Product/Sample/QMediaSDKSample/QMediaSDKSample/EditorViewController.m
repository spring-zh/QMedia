//
//  EditorViewController.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "EditorViewController.h"
#import "PreviewSliderBarController.h"
#import "GlobalXMObject.h"
#import <QMediaSDK/QMediaSDK.h>

@interface EditorViewController () <PreviewSliderBarControllerDelegate, QEditorPlayerObserver>
@property (nonatomic, strong) QEditorPlayer *player;
@property (nonatomic, weak) PreviewSliderBarController* sliderBarCtrl;
@property (atomic) BOOL isDraging;
@end

@implementation EditorViewController

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        self.player = [GlobalXMObject sharedInstance].player;
        [self.player addObserver:self];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(onNotification:)
                                                     name:kGlobalXMObjectSubObjectArrayChanged
                                                   object:nil];
        _isDraging = FALSE;
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
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)onNotification:(NSNotification *)notification{
    NSString *name = [notification name];
    if ([name isEqualToString:kGlobalXMObjectSubObjectArrayChanged]) {
        [self resetTrimLeftAndTrimRightState];
    }
}


#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
    if ([segue.identifier isEqualToString:@"PreviewSliderBarController"]) {
        self.sliderBarCtrl = segue.destinationViewController;
        self.sliderBarCtrl.delegate = self;
    }
}

- (void)resetTrimLeftAndTrimRightState
{
    float fileStartTime = self.player.mediaTimeRange.startPoint/1000.0f;
    float fileEndTime = self.player.mediaTimeRange.endPoint/1000.0f;
    float fileTotalTime = QTimeRangeGetLenght(self.player.mediaTimeRange)/1000.0f;
    self.sliderBarCtrl.totalTimeLength = fileEndTime;
    if (self.sliderBarCtrl.totalTimeLength == 0) {
        self.sliderBarCtrl.trimLeftProgress = 0;
        self.sliderBarCtrl.trimRightProgress = 0;
        return ;
    }
    self.sliderBarCtrl.trimLeftProgress = fileStartTime/self.sliderBarCtrl.totalTimeLength;
    self.sliderBarCtrl.trimRightProgress = (self.sliderBarCtrl.totalTimeLength-fileStartTime)/self.sliderBarCtrl.totalTimeLength;
}

- (void)onPrepare
{
    NSLog(@"onPrepare");
}

- (void)onPlayerChangedObjectToPlay
{
    [self resetTrimLeftAndTrimRightState];
}

- (void)onPlayerTimeProgressUpdated:(NSNumber*)cgfTime
{
    if(_isDraging)
        return;
    CGFloat currentTime = [cgfTime floatValue];
    self.sliderBarCtrl.progress = (float)_player.position/QTimeRangeGetLenght(_player.mediaTimeRange);
}
-(void)onCompleted
{
    NSLog(@"onCompleted");
}

- (void)onPreviewSliderBarControllerDidLoad:(PreviewSliderBarController*)sliderBarCtrl
{
    self.sliderBarCtrl.trimMinRemainPercent = 0.05;
    [self resetTrimLeftAndTrimRightState];
}

- (void)onPreviewSliderBarControllerProgressBegin:(PreviewSliderBarController*)sliderBarCtrl
{
    _isDraging = TRUE;
}

- (void)onPreviewSliderBarControllerProgressChanged:(PreviewSliderBarController*)sliderBarCtrl
{
    float seekTime = self.sliderBarCtrl.progress * QTimeRangeGetLenght(_player.mediaTimeRange);
    [self.player seekTo:seekTime :0];
//    [self.player seekTo:self.sliderBarCtrl.progress*((self.player.objectToPlay.layoutedEndTime-self.player.objectToPlay.timeLayout.startTime))+self.player.objectToPlay.timeLayout.startTime :0];

}

- (void)onPreviewSliderBarControllerProgressRelease:(PreviewSliderBarController*)sliderBarCtrl
{
    float seekTime = self.sliderBarCtrl.progress * QTimeRangeGetLenght(_player.mediaTimeRange);
    [self.player seekTo:seekTime :1];
//    [self.player seekTo:self.sliderBarCtrl.progress*((self.player.objectToPlay.layoutedEndTime-self.player.objectToPlay.timeLayout.startTime))+self.player.objectToPlay.timeLayout.startTime :1];
    _isDraging = FALSE;
}

- (void)onPreviewSliderBarControllerTrimLeftEnded:(PreviewSliderBarController*)sliderBarCtrl
{
//    XMObject* objectToPlay = self.player.objectToPlay;
//    objectToPlay.timeLayout.fileStartTime = self.sliderBarCtrl.totalTimeLength*self.sliderBarCtrl.trimLeftProgress;
//    objectToPlay.timeLayout.duration = self.sliderBarCtrl.totalTimeLength*(1-self.sliderBarCtrl.trimRightProgress)-objectToPlay.timeLayout.fileStartTime;
    [self resetTrimLeftAndTrimRightState];
}

- (void)onPreviewSliderBarControllerTrimRightEnded:(PreviewSliderBarController*)sliderBarCtrl
{
//    XMObject* objectToPlay = self.player.objectToPlay;
//    objectToPlay.timeLayout.duration = self.sliderBarCtrl.totalTimeLength*(1-self.sliderBarCtrl.trimRightProgress)-objectToPlay.timeLayout.fileStartTime;
    [self resetTrimLeftAndTrimRightState];
}

@end
