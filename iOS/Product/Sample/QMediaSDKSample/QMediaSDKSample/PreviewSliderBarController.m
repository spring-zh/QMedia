//
//  PreviewSliderBarController.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "PreviewSliderBarController.h"
#import "FloatingTimeView.h"
#import "CommonKits.h"
#import "GlobalXMObject.h"

@interface PreviewSliderBarController () <UIGestureRecognizerDelegate, GlobalMeidaManageObserver>

@property (nonatomic) IBOutlet UIView *minTrackView;

@property (nonatomic) IBOutlet UILabel *leftTimeLabel;
@property (nonatomic) IBOutlet UILabel *rightTimeLabel;

@property (nonatomic) IBOutlet UIView *thumbnailView;

@property (nonatomic, strong) FloatingTimeView *floatingTimeView;

@end

@implementation PreviewSliderBarController

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
    }
    return self;
}

- (void)dealloc
{
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.floatingTimeView = [FloatingTimeView floatingTimeView];
    
    UITapGestureRecognizer *tapGesturRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(tapAction:)];
    [self.view addGestureRecognizer:tapGesturRecognizer];
    
    self.progress = 0.0;
    self.trimLeftProgress = 0.0;
    self.trimRightProgress = 0.0;
    self.trimMinRemainPercent = 0.00;
    
    [self.delegate onPreviewSliderBarControllerDidLoad:self];
    
    if (self.disableProgress) {
        self.thumbnailView.hidden = YES;
    }
    
    [[GlobalXMObject sharedInstance].observers addObject:self];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)setDisableProgress:(BOOL)disableProgress
{
    _disableProgress = disableProgress;
    
    self.thumbnailView.hidden = _disableProgress;
}

- (void)setTotalTimeLength:(CGFloat)totalTimeLength
{
    _totalTimeLength = totalTimeLength;
    
    self.rightTimeLabel.text = [CommonKits formattedTimeStringWithFloat:_totalTimeLength];
}

- (void)setProgress:(CGFloat)progress
{
    if (progress < 0 || progress > 1)
    {
        return ;
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        _progress = progress;
        self.minTrackView.frame = self.thumbnailView.superview.frame;
//        CGFloat totalLength = (1-self.trimLeftProgress-self.trimRightProgress)*self.thumbnailView.superview.bounds.size.width/*-self.thumbnailView.frame.size.width*/;
        self.thumbnailView.center = CGPointMake(progress * self.minTrackView.frame.size.width, self.thumbnailView.center.y) ;
        
        if (!self.disableProgress) {
//            CGFloat currentTimePoint = self.totalTimeLength*(self.trimLeftProgress+(1-self.trimLeftProgress-self.trimRightProgress)*progress);
            self.leftTimeLabel.text = [CommonKits formattedTimeStringWithFloat:progress];
        }
    });
}


-(void)onSelectTrackAtIndex:(NSInteger)index {
    if (index != -1) {
        self.view.backgroundColor = nil;
        self.view.layer.borderColor = nil;
        self.view.layer.borderWidth = 0;
    }
}

-(void)tapAction:(id)tap
 {
     [GlobalXMObject sharedInstance].selectTrackIndex = -1;
     self.view.backgroundColor = [UIColor greenColor];
     self.view.layer.borderColor = [UIColor cyanColor].CGColor;
     self.view.layer.borderWidth = 2;
 }

//- (void)setTrimLeftProgress:(CGFloat)trimLeftProgress
//{
//    if (trimLeftProgress < 0 || _trimRightProgress > 1)
//    {
//        return ;
//    }
//    
//    if (1-trimLeftProgress-_trimRightProgress < _trimMinRemainPercent)
//    {
//        return ;
//    }
//    
//    _trimLeftProgress = trimLeftProgress;
//    
//    CGFloat totalLength = self.trimLeftHandleView.superview.bounds.size.width;
//    
//    CGRect rc = self.trimLeftHandleView.frame;
//    rc.origin.x = totalLength*trimLeftProgress-1;
//    self.trimLeftHandleView.frame = rc;
//    
//    rc = self.trimLeftTrackView.frame;
//    rc.size.width = totalLength*trimLeftProgress;
//    self.trimLeftTrackView.frame = rc;
//    
//    self.progress = _progress;
//    
//    self.floatingTimeView.currentTime = self.totalTimeLength*trimLeftProgress;
//    
//    if (self.disableProgress) {
//        self.leftTimeLabel.text = [CommonKits formattedTimeStringWithFloat:self.totalTimeLength*trimLeftProgress];
//        self.rightTimeLabel.text = [CommonKits formattedTimeStringWithFloat:self.totalTimeLength*(1-self.trimRightProgress)];
//    }
//}

//- (void)setTrimRightProgress:(CGFloat)trimRightProgress
//{
//    if (trimRightProgress < 0 || trimRightProgress > 1)
//    {
//        return ;
//    }
//    
//    if (1-_trimLeftProgress-trimRightProgress < _trimMinRemainPercent)
//    {
//        return ;
//    }
//    
//    _trimRightProgress = trimRightProgress;
//    
//    CGFloat totalLength = self.trimRightHandleView.superview.bounds.size.width;
//    
//    CGRect rc = self.trimRightHandleView.frame;
//    rc.origin.x = totalLength*(1-trimRightProgress)-rc.size.width+1;
//    self.trimRightHandleView.frame = rc;
//    
//    rc = self.trimRightTrackView.frame;
//    rc.origin.x = totalLength*(1-trimRightProgress);
//    rc.size.width = totalLength*trimRightProgress;
//    self.trimRightTrackView.frame = rc;
//    
//    self.progress = _progress;
//    
//    self.floatingTimeView.currentTime = self.totalTimeLength*(1-trimRightProgress);
//    
//    if (self.disableProgress) {
//        self.leftTimeLabel.text = [CommonKits formattedTimeStringWithFloat:self.totalTimeLength*self.trimLeftProgress];
//        self.rightTimeLabel.text = [CommonKits formattedTimeStringWithFloat:self.totalTimeLength*(1-trimRightProgress)];
//    }
//}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

- (IBAction)onThumbnailPanGestureRecognizer:(UIPanGestureRecognizer *)sender
{
    switch (sender.state) {
        case UIGestureRecognizerStateBegan:
        {
            [self.delegate onPreviewSliderBarControllerProgressBegin:self];
        }
            break;
            
        case UIGestureRecognizerStateChanged:
        {
            CGPoint pt = [sender locationInView:self.thumbnailView.superview];
            CGFloat totalLength = (1-self.trimLeftProgress-self.trimRightProgress)*self.thumbnailView.superview.bounds.size.width;
//            CGFloat currentLength = pt.x - self.trimLeftHandleView.frame.origin.x;
//
//            self.progress = currentLength/totalLength;
            self.progress = pt.x / self.minTrackView.frame.size.width;
            
            [self.delegate onPreviewSliderBarControllerProgressChanged:self];
        }
            break;
            
        case UIGestureRecognizerStateEnded:
        case UIGestureRecognizerStateCancelled:
        case UIGestureRecognizerStateFailed:
        {
            CGPoint pt = [sender locationInView:self.thumbnailView.superview];
            CGFloat totalLength = (1-self.trimLeftProgress-self.trimRightProgress)*self.thumbnailView.superview.bounds.size.width;
//            CGFloat currentLength = pt.x - self.trimLeftHandleView.frame.origin.x;
//            
//            self.progress = currentLength/totalLength;
            self.progress = pt.x / self.minTrackView.frame.size.width;
            
            [self.delegate onPreviewSliderBarControllerProgressRelease:self];

        }
            break;
            
        default:
            break;
    }
}

- (IBAction)onTrimLeftHandlePanGestureRecognizer:(UIPanGestureRecognizer *)sender
{
//    switch (sender.state) {
//        case UIGestureRecognizerStateBegan:
//        {
//            CGRect rc = self.floatingTimeView.frame;
//            rc.origin.y = -2-rc.size.height;
//            rc.origin.x = 0;
//            self.floatingTimeView.frame = rc;
//
//            self.floatingTimeView.hidden = NO;
//            [self.trimLeftHandleView addSubview:self.floatingTimeView];
//        }
//            break;
//
//        case UIGestureRecognizerStateChanged:
//        {
//            CGPoint pt = [sender locationInView:self.trimLeftHandleView.superview];
//            CGFloat totalLength = self.trimLeftHandleView.superview.bounds.size.width;
//            CGFloat currentLength = pt.x-self.trimLeftHandleView.frame.size.width/2;
//
//            self.trimLeftProgress = currentLength/totalLength;
//        }
//            break;
//
//        case UIGestureRecognizerStateEnded:
//        case UIGestureRecognizerStateCancelled:
//        case UIGestureRecognizerStateFailed:
//        {
//            self.floatingTimeView.hidden = YES;
//
//            [self.delegate onPreviewSliderBarControllerTrimLeftEnded:self];
//        }
//            break;
//
//        default:
//            break;
//    }
}

- (IBAction)onTrimRightHandlePanGestureRecognizer:(UIPanGestureRecognizer *)sender
{
//    switch (sender.state) {
//        case UIGestureRecognizerStateBegan:
//        {
//            CGRect rc = self.floatingTimeView.frame;
//            rc.origin.y = -2-rc.size.height;
//            rc.origin.x = self.trimRightHandleView.frame.size.width-rc.size.width;
//            self.floatingTimeView.frame = rc;
//
//            self.floatingTimeView.hidden = NO;
//            [self.trimRightHandleView addSubview:self.floatingTimeView];
//        }
//            break;
//
//        case UIGestureRecognizerStateChanged:
//        {
//            CGPoint pt = [sender locationInView:self.trimRightHandleView.superview];
//            CGFloat totalLength = self.trimRightHandleView.superview.bounds.size.width;
//            CGFloat currentLength = pt.x+self.trimRightHandleView.frame.size.width/2;
//
//            self.trimRightProgress = (totalLength-currentLength)/totalLength;
//        }
//            break;
//
//        case UIGestureRecognizerStateEnded:
//        case UIGestureRecognizerStateCancelled:
//        case UIGestureRecognizerStateFailed:
//        {
//            self.floatingTimeView.hidden = YES;
//
//            [self.delegate onPreviewSliderBarControllerTrimRightEnded:self];
//        }
//            break;
//
//        default:
//            break;
//    }
}


@end
