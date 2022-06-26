//
//  ExportViewController.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 25/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "ExportViewController.h"
#import <QMediaSDK/QMediaSDK.h>
#import <AVFoundation/AVFoundation.h>
#import <AVKit/AVKit.h>

#import "CommonKits.h"
#import "GlobalXMObject.h"

@interface ExportViewController () <QExporterObserver>
@property (nonatomic) IBOutlet UILabel* progressLabel;
@property (nonatomic) IBOutlet UIView* buttonLayer;
@property (nonatomic) IBOutlet UIView* playerViewContainer;
@property (nonatomic) IBOutlet UIActivityIndicatorView* activityIndicator;
@property (nonatomic, strong) AVPlayerViewController* playerCtrl;
@property (nonatomic, strong) QExporter* exporter;
@property (nonatomic, copy) NSString* filePath;
@end

@implementation ExportViewController

- (void)dealloc
{
    [self.playerCtrl.player pause];
    [self.playerCtrl.view removeFromSuperview];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.filePath = [NSString stringWithFormat:@"%@/ExportedVideo.mp4", [CommonKits tmpPath]];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    bool exists = [fileManager fileExistsAtPath:self.filePath];
    if (exists) {
        [fileManager removeItemAtPath:self.filePath error:nil];
    }
    self.buttonLayer.hidden = YES;
//    QVideoDescribe* vdesc = [[QVideoDescribe alloc] initWithParamenters:QVideoCodecH264 framerate:25 width:640 height:480 bitrate:2*1024*1024];
//    QAudioDescribe* adesc = [[QAudioDescribe alloc] initWithParamenters:QAudioCodecAAC rawFormat:QRawAudioFormatS16 samplerate:44100 nchannel:2 bitrate:128000];
//
    int targetW = 640;
    int targetH = 480;
    
    QVideoEncodeOption* vopt = [QVideoEncodeOption videoEncodeOptionWithCodectype:QVideoEncodeOptionKH264 width:targetW height:targetH bitrate:2*1024*1024 framerate:25 gop:120 profile:QVideoEncodeOptionProfileHigh level:31 ext:nil];
    QAudioEncodeOption* aopt = [QAudioEncodeOption audioEncodeOptionWithCodectype:QAudioEncodeOptionKAAC nchannel:2 samplerate:44100 bitrate:128000 sampleformat:QAudioDescriptionFormatS16];
    self.exporter = [[QExporter alloc] initWithPath:self.filePath];
    [self.exporter addObserver:self];
    [self.exporter setAudioOption:aopt];
    [self.exporter setVideoOption:vopt];
    
    NSString* testVideoFile2 = [QFileUtils getFileFromMainbundleAbsolutePath:@"video/test.mp4"];
    QMediaSegment* video_segment = [_exporter cresteMediaSegment:testVideoFile2 flag:QMediaSegmentFlagAll];
    [video_segment setDisplayRange:[QMediaRange mediaRangeWithStart:0 end:10000]];
//    [video_segment setTimescale:2];
    QVideoRenderNode* graphic = [video_segment getVideo];
    [graphic setPosition:[QPoint pointWithX:targetW/4 y:targetH/4]];
    [graphic setAnchorPoint:[QPoint pointWithX:0.5 y:0.5]];
    [graphic setContentSize:[QSize sizeWithWidth:targetW/2 height:targetH/2]];
//
////    [self.exporter loadSerializeSettings:[[GlobalXMObject sharedInstance].player serialize]];
//    [self.exporter copyFrom:[GlobalXMObject sharedInstance].player];
//
    [self.exporter addMediaSegment:video_segment];
    [self.exporter start];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}

- (void)onExporterProgressUpdated:(nonnull QExporter*)exporter time:(NSNumber*)cgfTime {
    CGFloat value = [cgfTime floatValue]*1000 / QMediaRangeGetLenght([_exporter getTotalTimeRange]);
    self.progressLabel.text = [NSString stringWithFormat:@"%d%%", (int)(value*100)];
}
- (void)onExporterStarted:(nonnull QExporter*)exporter error:(int)errcode {
    
}
- (void)onExporterStoped:(nonnull QExporter*)exporter {
    [exporter removeObserver:self];
}
- (void)onExporterCanceled:(nonnull QExporter*)exporter {
    
}
- (void)onExporterCompleted:(nonnull QExporter*)exporter error:(int)errcode {
    self.progressLabel.text = @"导出成功！";
    
    self.buttonLayer.hidden = NO;
    [self.activityIndicator stopAnimating];
    self.activityIndicator.hidden = YES;
    
    self.playerCtrl = [[AVPlayerViewController alloc] init];
    self.playerCtrl.player = [AVPlayer playerWithURL:[NSURL fileURLWithPath:self.filePath]];
    self.playerCtrl.videoGravity = AVLayerVideoGravityResizeAspect;
    self.playerCtrl.view.frame = self.playerViewContainer.bounds;
    [self.playerViewContainer addSubview:self.playerCtrl.view];
    
    [self.exporter removeAllObservers];
     self.exporter = nil;
}
- (void)onExporterEvent:(nonnull QExporter*)exporter eventid:(int32_t)eventid msg:(nonnull NSDictionary<NSString *, NSString *> *)msg {
    
}


//- (void)onExportProgressUpdated:(CGFloat)progress
//{
//    self.progressLabel.text = [NSString stringWithFormat:@"%d%%", (int)(progress*100)];
//}


//-(void)onExportError:(XMFormaterErrorType)type
//{
//    NSLog(@"MediaErrorType onError onError onError");
//}

- (IBAction)onCloseButtonPressed:(id)sender
{
    [self.navigationController dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)onSaveToAlbumButtonPressed:(id)sender
{
    UISaveVideoAtPathToSavedPhotosAlbum(self.filePath,
                                        self,
                                        @selector(video:didFinishSavingWithError:contextInfo:), nil);
}

- (void)video:(NSString *)videoPath didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInf{
    UIAlertController* alert = nil;
    if (error) {
        alert = [UIAlertController alertControllerWithTitle:nil
                                            message:@"保存失败!"
                                     preferredStyle:UIAlertControllerStyleAlert];
        
    }
    else {
        alert = [UIAlertController alertControllerWithTitle:nil
                                                                       message:@"保存成功，请到相册中查看！"
                                                                preferredStyle:UIAlertControllerStyleAlert];
    }
    
    [alert addAction:[UIAlertAction actionWithTitle:@"知道了" style:UIAlertActionStyleCancel handler:nil]];
    [self presentViewController:alert animated:YES completion:nil];
}

@end
