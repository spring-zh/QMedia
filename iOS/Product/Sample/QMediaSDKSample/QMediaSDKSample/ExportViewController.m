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
    QVideoDescribe* vdesc = [[QVideoDescribe alloc] initWithParamenters:QVideoCodecH264 framerate:25 width:640 height:480 bitrate:2*1024*1024];
    QAudioDescribe* adesc = [[QAudioDescribe alloc] initWithParamenters:QAudioCodecAAC rawFormat:QRawAudioFormatS16 samplerate:44100 nchannel:2 bitrate:128000];
    QFileExporterTarget* exporterTarget = [[QFileExporterTarget alloc] initWithPath:self.filePath];

//    exporterTarget.videoDesc = vdesc;
//    exporterTarget.audioDesc = adesc;
    self.exporter = [[QExporter alloc] initWithTarget:exporterTarget audioTarget:exporterTarget];
    [self.exporter addObserver:self];
    [self.exporter setAudioConfig:adesc];
    [self.exporter setVideoConfig:vdesc];
//    self.formater = [XMFormater sharedInstance];
//    self.formater.delegate = self;
//    self.formater.objectToFormat = [GlobalXMObject sharedInstance];
//    self.formater.bitRate = 8*1024*1024;
//    self.formater.frameRate = 36;
//    self.formater.pixelSize = [GlobalXMObject sharedInstance].pixelSize;
//
//    self.filePath = [NSString stringWithFormat:@"%@/ExportedVideo.mp4", [CommonKits tmpPath]];
//    [self.formater exportToVideoFileWithPath:self.filePath];
    
     QLayer *layer = [[QLayer alloc] initWithSize:CGSizeMake(750,618) name:@"haha"];
        layer.color4 = QColorMaker(1, 1, 1, 1);
        layer.bkColor = QColorMaker(1, 0, 1, 1);
        layer.position = CGPointMake(100, 50);
        layer.anchorPoint = CGPointMake(0.5, 0.5);
        layer.contentSize = CGSizeMake(750, 618);
//        layer.rotation = -30.0f;
        
        self.exporter.rootNode.color4 = QColorMaker(0, 0, 1, 1);
    //    self.player.graphicLayer.color4 = XMColorMaker(1, 0.5, 0.5, 0.5);
    //    self.player.rootNode.rotation = 30;
    //    self.player.rootNode.position = CGPointMake(-100, -100);
    //    self.player.rootNode.anchorPoint = CGPointMake(0.5, 0.5);
        NSString* testVideoFile2 = [self getFileFromMainbundleAbsolutePath:@"video/test.mp4"];
        
    QMediaTrack* videoTrack = [self.exporter.mediaFactory createVideoTrack:testVideoFile2];
//        XMVideoTrack* videoTrack = [[XMMediaFactory sharedInstance] createVideoTrackE:self.exporter filePath:testVideoFile2];
        videoTrack.graphic.position = CGPointMake(50, 100);
        videoTrack.graphic.contentSize = CGSizeMake(750, 618);//self.player.layerSize;
    //    videoTrack.graphic.color4 = XMColorMaker(1, 1, 1, 0.5);
        videoTrack.graphic.anchorPoint = CGPointMake(0.5, 0.5);
        videoTrack.graphic.rotation = -30.0f;
        [layer addChildNode:videoTrack.graphic];
    
        [self.exporter.rootNode addChildNode:layer];
        
        if ([videoTrack prepare]) {
//            XMVideoDescribe* vdesc = videoTrack.source.videoDesc;
//            videoTrack.graphic.contentSize = CGSizeMake(vdesc.width, vdesc.height);
//            XMAudioDescribe* adesc = videoTrack.source.audioDesc;
            [self.exporter addMediaTrack:videoTrack];
        }
    
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

- (void)onExporterStarted:(QExporterRetCode)code
{
    
}
- (void)onExporterStoped:(QExporterRetCode)code
{
    
}
- (void)onExporterProgressUpdated:(CGFloat)progress
{
    float value = (float)progress * 1000/ (_exporter.mediaTimeRange.location + _exporter.mediaTimeRange.length);
    self.progressLabel.text = [NSString stringWithFormat:@"%d%%", (int)(value*100)];
}
- (void)onExporterCanceled:(QExporterRetCode)code
{
    
}
- (void)onExporterCompleted
{
    self.progressLabel.text = @"导出成功！";
    
    self.buttonLayer.hidden = NO;
    [self.activityIndicator stopAnimating];
    self.activityIndicator.hidden = YES;
    
    self.playerCtrl = [[AVPlayerViewController alloc] init];
    self.playerCtrl.player = [AVPlayer playerWithURL:[NSURL fileURLWithPath:self.filePath]];
    self.playerCtrl.videoGravity = AVLayerVideoGravityResizeAspect;
    self.playerCtrl.view.frame = self.playerViewContainer.bounds;
    [self.playerViewContainer addSubview:self.playerCtrl.view];
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

- (NSString*) getFileFromMainbundleAbsolutePath:(NSString*) fileCompent
{
    return [NSString stringWithFormat:@"%@/%@",[[NSBundle mainBundle] resourcePath], fileCompent];
}

@end
