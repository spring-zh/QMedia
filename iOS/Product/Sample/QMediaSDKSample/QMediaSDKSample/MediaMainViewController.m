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

@interface MediaMainViewController () <GlobalMeidaManageObserver, UINavigationControllerDelegate, UIImagePickerControllerDelegate, MPMediaPickerControllerDelegate, VideoRecorderViewControllerDelegate>
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
        [[GlobalXMObject sharedInstance].observers addObject:self];
        self.player = [GlobalXMObject sharedInstance].player;
        self.player.loopPlay = YES;
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

-(void)onTrackChange {
    
}

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
        
        QMediaTrack* videoTrack = [self.player.mediaFactory createVideoTrack:videoURL.absoluteString combiner:self.player];
        videoTrack.graphic.contentSize = CGSizeMake([self.player.videoTarget getWidth]/2, [self.player.videoTarget getHeight]/2);
        [self.player.rootNode addChildNode:videoTrack.graphic];
        
        videoTrack.displayName = [QFileUtils fileComponentOfPath:videoURL.absoluteString];
        [self.player addMediaTrack:videoTrack];
        [[GlobalXMObject sharedInstance] addTrack:videoTrack];
    }
    else if ([mediaType isEqualToString:(NSString*) kUTTypeImage]) {
        UIImage *image= [info objectForKey:UIImagePickerControllerOriginalImage];
        NSLog(@"Got a image %@", image);
        NSData* imageData = UIImageJPEGRepresentation(image, 1.0);
        NSString* imagePath = [NSString stringWithFormat:@"%@%ld.jpg", [CommonKits tmpPath], (long)[[NSDate date] timeIntervalSince1970]];
        [imageData writeToFile:imagePath atomically:NO];
        QImageNode* imageNode = [[QImageNode alloc] initWithPath:imagePath combiner:self.player];
        imageNode.contentSize = CGSizeMake([self.player.videoTarget getWidth]/2, [self.player.videoTarget getHeight]/2);
        imageNode.displayRange = QTimeRangeMake(0, 10000);
        imageNode.name = [QFileUtils fileComponentOfPath:imagePath];
        [self.player.rootNode addChildNode:imageNode];
        [[GlobalXMObject sharedInstance] addTrack:imageNode];
    }
    [self.player seekTo:self.player.position :0];
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
    QMediaTrack* videoTrack = [self.player.mediaFactory createVideoTrack:path combiner:self.player];
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
