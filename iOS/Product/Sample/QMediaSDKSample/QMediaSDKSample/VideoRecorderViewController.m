

#import "VideoRecorderViewController.h"
#import <QMediaSDK/QMediaSDK.h>
#import "CommonKits.h"
#import "GlobalXMObject.h"
#import "FilterManager.h"

@interface VideoRecorderViewController ()
//@property (nonatomic) IBOutlet XMCameraView *renderView;
@property (nonatomic) IBOutlet UIButton *startButton;
@property (nonatomic) IBOutlet UILabel *timeLabel;
@property (nonatomic, strong) dispatch_queue_t captureListenerQueue;

@property (nonatomic, strong) NSTimer* timer;
@property (nonatomic, strong) NSDate *startDate;
@property (nonatomic, copy) NSString *recordingFile;

//@property (nonatomic, strong) XMCameraObject* object;
//@property (nonatomic, strong) XMFormater* formater;

@property (nonatomic) NSInteger selectedFilterIndex;
@property (nonatomic, strong) FilterManager* filterManager;

@end

@implementation VideoRecorderViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    self.filterManager = [FilterManager new];
    
//    self.object = [XMCameraObject new];
//    self.object.dresser = [XMDresser new];
//    self.selectedFilterIndex = 1;
//    [self.object startWithCaptureDevicePosition:AVCaptureDevicePositionBack];
//    self.object.cameraView = self.renderView;
//
//    self.formater = [XMFormater new];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    
}

- (void)setSelectedFilterIndex:(NSInteger)selectedFilterIndex
{
    _selectedFilterIndex = selectedFilterIndex;
    
//    [self.object.dresser removeAllFilters];
//
//    if (selectedFilterIndex >= 0 && selectedFilterIndex < self.filterManager.allFilters.count) {
//        [self.object.dresser addFilter:self.filterManager.allFilters[selectedFilterIndex]];
//    }
}

- (void)onTimer:(NSTimer*)timer
{
    NSDate* nowDate = [NSDate date];
    NSTimeInterval interval = [nowDate timeIntervalSinceDate:self.startDate];
    NSDate* date = [NSDate dateWithTimeIntervalSince1970:interval];
    NSDateFormatter* formatter = [NSDateFormatter new];
    [formatter setTimeZone:[NSTimeZone timeZoneWithName:@"GMT"]];
    formatter.dateFormat = [NSDateFormatter dateFormatFromTemplate:@"HH:mm:ss" options:0 locale:nil];
    self.timeLabel.text = [formatter stringFromDate:date];
}

//- (void)mediaRecorder:(XMMediaRecorder*)recorder
//     stateChangedFrom:(XMMediaRecorderStatus) oldState
//                   to:(XMMediaRecorderStatus) newState
//{
//    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
//        if (newState == XMMRS_didStart) {
//            self.startButton.selected = YES;
//            self.startDate = [NSDate date];
//
//            self.timer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(onTimer:) userInfo:nil repeats:YES];
//        }
//        else {
//            self.startButton.selected = NO;
//            if (newState == XMMRS_didStop) {
//                XMVideoObject* videoObject = [[XMVideoObject alloc] initWithFilePath:self.recordingFile];
//                videoObject.timeLayout = [XMTimeLayout new];
//                [[GlobalXMObject sharedInstance] addSubObject:videoObject];
//
//                [self.timer invalidate];
//
//                [self dismissViewControllerAnimated:YES completion:nil];
//            }
//        }
//    });
//}

/*
 #pragma mark - Navigation
 
 // In a storyboard-based application, you will often want to do a little preparation before navigation
 - (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
 // Get the new view controller using [segue destinationViewController].
 // Pass the selected object to the new view controller.
 }
 */

- (IBAction)onLongPressGestureRecognizer:(UILongPressGestureRecognizer *)sender
{
}

- (IBAction)onStartButtonPressed:(UIButton *)sender
{
//    if (self.formater.isExporting)
//    {
//        [self.formater stop];
//
//        [self.object stopCapturing];
//
//        ///////////////////////////////////
//        self.startButton.selected = NO;
//
//        [self.timer invalidate];
//
//        [self dismissViewControllerAnimated:YES completion:nil];
//
//        [self.delegate onVideoRecorderViewController:self
//                                     savedToFilePath:self.recordingFile];
//    }
//    else {
//        NSDate* nowDate = [NSDate date];
//        self.recordingFile = [NSString stringWithFormat:@"%@%ld.mp4", [CommonKits tmpPath], (long)[nowDate timeIntervalSince1970]];
//        NSFileManager *manager = [NSFileManager defaultManager];
//        [manager removeItemAtPath:self.recordingFile error:nil];
//
//        self.formater.objectToFormat = self.object;
//        [self.formater exportToVideoFileWithPath:self.recordingFile];
//
//        ///////////////////////////////////
//        self.startButton.selected = YES;
//        self.startDate = [NSDate date];
//
//        self.timer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(onTimer:) userInfo:nil repeats:YES];
//
//    }
}

- (IBAction)onCloseButtonPressed:(UIButton *)sender
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)onSwitchButtonPressed:(UIButton *)sender
{
//    [self.object switchCaptureDevicePosition];
}

- (IBAction)onFilterButtonPressed:(id)sender
{
//    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:@"选择滤镜" message:nil preferredStyle:UIAlertControllerStyleActionSheet];
//    
//    for (NSInteger i = 0; i < self.filterManager.allFilters.count; ++i) {
//        XMFilter* filter = self.filterManager.allFilters[i];
//        NSString* filterName = filter.name;
//        if (i == self.selectedFilterIndex) {
//            filterName = [NSString stringWithFormat:@"*%@", filter.name];
//        }
//        UIAlertAction *action = [UIAlertAction actionWithTitle:filterName
//                                                         style:UIAlertActionStyleDefault
//                                                       handler:^(UIAlertAction * _Nonnull action) {
//                                                           self.selectedFilterIndex = i;
//                                                       }];
//        [actionSheet addAction:action];
//    }
//    
//    UIAlertAction *action = [UIAlertAction actionWithTitle:(self.selectedFilterIndex == -1)?@"*不使用":@"不使用"
//                                                     style:UIAlertActionStyleDestructive
//                                                   handler:^(UIAlertAction * _Nonnull action) {
//                                                       self.selectedFilterIndex = -1;
//                                                   }];
//    [actionSheet addAction:action];
//    
//    action = [UIAlertAction actionWithTitle:@"取消"
//                                      style:UIAlertActionStyleCancel
//                                    handler:nil];
//    [actionSheet addAction:action];
//    
//    [self presentViewController:actionSheet
//                       animated:YES
//                     completion:nil];
}

@end
