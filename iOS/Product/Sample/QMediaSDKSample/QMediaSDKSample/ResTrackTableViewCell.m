//
//  ResTrackTableViewCell.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "ResTrackTableViewCell.h"
#import "ResTrackTableViewController.h"
#import "FloatingTimeView.h"
#import "GlobalXMObject.h"

@interface ResTrackTableViewCell () <UIGestureRecognizerDelegate>
@property (nonatomic) IBOutlet UIView* resView;
@property (nonatomic) IBOutlet UIView* resContentView;

@property (nonatomic, strong) UILongPressGestureRecognizer *longPressRecognizer;
@property (nonatomic, strong) UIPanGestureRecognizer *panRecognizer;

@property (nonatomic) BOOL isDragging;
@property (nonatomic) CGFloat draggingXPos;

@property (nonatomic, strong) FloatingTimeView *floatingTimeView;

@property (nonatomic, weak) QEditorPlayer* player;
@property (nonatomic, weak) GlobalXMObject* globalXMObject;
@end

@implementation ResTrackTableViewCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
    
    self.floatingTimeView = [FloatingTimeView floatingTimeView];
    
    self.longPressRecognizer = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(onLongPressGestureRecognizer:)];
    self.longPressRecognizer.minimumPressDuration = 0.3;
    self.longPressRecognizer.delegate = self;
    self.panRecognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(onResViewPanGestureRecognizer:)];
    self.panRecognizer.delegate = self;
    
    [self.resView addGestureRecognizer:self.longPressRecognizer];
    [self.resView addGestureRecognizer:self.panRecognizer];
    
    self.player = [GlobalXMObject sharedInstance].player;
    self.globalXMObject = [GlobalXMObject sharedInstance];
    
    //    // 测试数据
    //    self.globalTimeLength = 600.56;
    //    self.resTimeLength = rand()%400 + 100;
    //    self.resStartTimePoint = rand()%570;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];
    
    // Configure the view for the selected state
}

- (void)setSourceTimeLength:(CGFloat)sourceTimeLength {
    _sourceTimeLength = sourceTimeLength;
}

- (void)setResTimeLength:(CGFloat)resTimeLength
{
    if (resTimeLength <= 0 /*|| resTimeLength > self.globalTimeLength*/) {
        return ;
    }
    _resTimeLength = resTimeLength;
    
    CGRect rc = self.resView.frame;
    rc.size.width = resTimeLength*self.resView.superview.bounds.size.width/self.globalTimeLength;
    self.resView.frame = rc;
    
    CGFloat sourceDisplayLength = MIN(resTimeLength, _sourceTimeLength);
    rc = self.resThumbImageView.frame;
    rc.size.width = sourceDisplayLength*self.resView.superview.bounds.size.width/self.globalTimeLength;
    self.resThumbImageView.frame = rc;
}

- (void)setResStartTimePoint:(CGFloat)resStartTimePoint
{
    //    if (resStartTimePoint < 0)
    //    {
    //        resStartTimePoint = 0;
    //    }
    //
    CGRect rc = self.resView.frame;
    rc.origin.x = resStartTimePoint*self.resView.superview.bounds.size.width/self.globalTimeLength;
    //    if (rc.origin.x > self.resView.superview.bounds.size.width-self.resView.frame.size.width) {
    //        rc.origin.x = self.resView.superview.bounds.size.width-self.resView.frame.size.width;
    //    }
    self.resView.frame = rc;
    
    //    if (resStartTimePoint > self.globalTimeLength-self.resTimeLength) {
    //        resStartTimePoint = self.globalTimeLength-self.resTimeLength;
    //    }
    _resStartTimePoint = resStartTimePoint;
    
    self.floatingTimeView.currentTime = resStartTimePoint;
    
    [self adjustFloatingTimeViewPos];
}

- (BOOL)isEditing
{
//    return (self.player.objectToPlay != self.globalXMObject);
    return false;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    return YES;
}

- (void)adjustFloatingTimeViewPos
{
    CGRect rc = self.floatingTimeView.frame;
    rc.origin.y = -2-rc.size.height;
    rc.origin.x = 0;
    rc = [self.resView convertRect:rc toView:self.window.rootViewController.view];
    
    self.floatingTimeView.frame = rc;
}

- (void)onLongPressGestureRecognizer:(UILongPressGestureRecognizer *)sender
{
    switch (sender.state) {
        case UIGestureRecognizerStateBegan:
        {
            if (self.isEditing) {
                return ;
            }
            self.isDragging = YES;
//            self.resContentView.alpha = 0.75;
            
            [self adjustFloatingTimeViewPos];
            
            [self.window.rootViewController.view addSubview:self.floatingTimeView];
            
            self.floatingTimeView.hidden = NO;
            
            [self.superview bringSubviewToFront:self];
        }
            break;
            
        case UIGestureRecognizerStateChanged:
        {
        }
            break;
            
        case UIGestureRecognizerStateEnded:
        case UIGestureRecognizerStateCancelled:
        case UIGestureRecognizerStateFailed:
        {
            
            self.isDragging = NO;
//            self.resContentView.alpha = 1.0;
            
            self.floatingTimeView.hidden = YES;
        }
            break;
            
        default:
            break;
    }
}

- (void)onResViewPanGestureRecognizer:(UIPanGestureRecognizer *)sender
{
    switch (sender.state) {
        case UIGestureRecognizerStateBegan:
        {
            CGPoint pt = [sender locationInView:self.resView];
            self.draggingXPos = pt.x;
        }
            break;
            
        case UIGestureRecognizerStateChanged:
        {
            if (self.isDragging) {
                CGPoint pt = [sender locationInView:self.resView.superview];
                self.resStartTimePoint = self.globalTimeLength*(pt.x-self.draggingXPos)/self.resView.superview.bounds.size.width;
                NSLog(@"%f, %f, %f", self.resStartTimePoint, self.resTimeLength, self.globalTimeLength);
                
                int64_t start_point = self.resStartTimePoint * 1000;
                
                id<QTrack> track = [self.globalXMObject.tracks objectAtIndex:self.cellIndex];
                int64_t trackLenght = QTimeRangeGetLenght( track.displayRange);
                track.displayRange = QTimeRangeMake(start_point, start_point + trackLenght);
                
                [self.player seekTo:self.player.position :0];
            }
        }
            break;
            
        case UIGestureRecognizerStateEnded:
        case UIGestureRecognizerStateCancelled:
        case UIGestureRecognizerStateFailed:
        {
            int64_t start_point = self.resStartTimePoint * 1000;
//            QMediaTrack* subObject = self.globalXMObject.player.subObjects.allValues[self.cellIndex];
//            int64_t trackLenght = QTimeRangeGetLenght( subObject.displayRange);
//            subObject.displayRange = QTimeRangeMake(start_point, start_point + trackLenght);
            id<QTrack> track = [self.globalXMObject.tracks objectAtIndex:self.cellIndex];
            int64_t trackLenght = QTimeRangeGetLenght( track.displayRange);
            track.displayRange = QTimeRangeMake(start_point, start_point + trackLenght);
            [self.player seekTo:self.player.position :0];
        }
            break;
            
        default:
            break;
    }
}

- (IBAction)onResViewButtonPressed:(id)sender
{
//    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:self.resNameLabel.text
//                                                                         message:nil
//                                                                  preferredStyle:UIAlertControllerStyleActionSheet];
//    UIAlertAction *action = nil;
    
    [_tableViewController selectRowByIndex:self.cellIndex];
    
    
//    if (self.isEditing) {
//        if (self.globalXMObject.subObjects[self.cellIndex] != self.player.objectToPlay) {
//            return ;
//        }
//        action = [UIAlertAction actionWithTitle:@"退出编辑"
//                                          style:UIAlertActionStyleDefault
//                                        handler:^(UIAlertAction * _Nonnull action) {
//                                            [self.player.objectToPlay.layout removeObserver:self
//                                                                                 forKeyPath:@"duration"];
//                                            [self.player setObjectToPlay:self.globalXMObject];
//                                            self.backgroundColor = [UIColor clearColor];
//                                            self.layer.borderColor = [UIColor clearColor].CGColor;
//                                            self.layer.borderWidth = 0;
//                                        }];
//        [actionSheet addAction:action];
//    }
//    else {
//        action = [UIAlertAction actionWithTitle:@"↑图层调低"
//                                          style:UIAlertActionStyleDefault
//                                        handler:^(UIAlertAction * _Nonnull action) {
//                                            if (self.cellIndex <= 0) {
//                                                return ;
//                                            }
//
//                                            [self.globalXMObject moveSubObjectFromIndex:self.cellIndex toIndex:self.cellIndex-1];
//                                        }];
//        [actionSheet addAction:action];
//
//        action = [UIAlertAction actionWithTitle:@"↓图层调高"
//                                          style:UIAlertActionStyleDefault
//                                        handler:^(UIAlertAction * _Nonnull action) {
//                                            if (self.cellIndex >= self.globalXMObject.subObjects.count-1) {
//                                                return ;
//                                            }
//
//                                            [self.globalXMObject moveSubObjectFromIndex:self.cellIndex toIndex:self.cellIndex+1];
//                                        }];
//        [actionSheet addAction:action];
//
//        XMObject* object = self.globalXMObject.subObjects[self.cellIndex];
//        if ([object isKindOfClass:[XMImageObject class]]) {
//            action = [UIAlertAction actionWithTitle:@"设置图片原始时长"
//                                              style:UIAlertActionStyleDefault
//                                            handler:^(UIAlertAction * _Nonnull action) {
//                                                UIAlertController* alert = [UIAlertController alertControllerWithTitle:nil message:@"设置图片原始时长" preferredStyle:UIAlertControllerStyleAlert];
//                                                [alert addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
//                                                    textField.placeholder = @"请设置一个浮点秒数";
//                                                    textField.text = [NSString stringWithFormat:@"%.2f", object.duration];
//                                                }];
//
//                                                [alert addAction:[UIAlertAction actionWithTitle:@"确定"
//                                                                                          style:UIAlertActionStyleDefault
//                                                                                        handler:^(UIAlertAction * _Nonnull action) {
//                                                                                            CGFloat duration = [alert.textFields[0].text floatValue];
//                                                                                            if (duration > 1) {
//                                                                                                [(XMImageObject*)object setDuration:duration forImageAtIndex:0];
//                                                                                                 [[NSNotificationCenter defaultCenter] postNotificationName:kGlobalXMObjectSubObjectArrayChanged object:self];
//                                                                                            }
//                                                                                        }]];
//
//                                                [self.window.rootViewController presentViewController:alert
//                                                                                             animated:YES
//                                                                                           completion:nil];
//                                            }];
//            [actionSheet addAction:action];
//        }
//
//        action = [UIAlertAction actionWithTitle:@"编辑"
//                                          style:UIAlertActionStyleDefault
//                                        handler:^(UIAlertAction * _Nonnull action) {
//                                            XMObject* object = self.globalXMObject.subObjects[self.cellIndex];
//                                            XMComposedObject* container = [XMComposedObject new];
//                                            container.pixelSize = self.globalXMObject.pixelSize;
//                                            container.layout = self.globalXMObject.layout;
//                                            container.duration = object.duration;
//                                            container.timeLayout = object.timeLayout;
//                                            [container addSubObject:object];
//                                            [self.player setObjectToPlay:self.globalXMObject.subObjects[self.cellIndex]];
//                                            [self.player.objectToPlay.layout addObserver:self
//                                                                              forKeyPath:@"duration"
//                                                                                 options:NSKeyValueObservingOptionNew
//                                                                                 context:nil];
//                                            self.backgroundColor = [UIColor cyanColor];
//                                            self.layer.borderColor = [UIColor cyanColor].CGColor;
//                                            self.layer.borderWidth = 2;
//                                        }];
//        [actionSheet addAction:action];
        
        //    action = [UIAlertAction actionWithTitle:@"复制"
        //                                      style:UIAlertActionStyleDefault
        //                                    handler:^(UIAlertAction * _Nonnull action) {
        //
        //                                    }];
        //    [actionSheet addAction:action];
        
//        action = [UIAlertAction actionWithTitle:@"删除"
//                                          style:UIAlertActionStyleDestructive
//                                        handler:^(UIAlertAction * _Nonnull action) {
//                                            [self.globalXMObject removeSubObjectAtIndex:self.cellIndex];
//                                        }];
//        [actionSheet addAction:action];
//    }
//    
//    action = [UIAlertAction actionWithTitle:@"取消"
//                                      style:UIAlertActionStyleCancel
//                                    handler:^(UIAlertAction * _Nonnull action) {
//                                        
//                                    }];
//    [actionSheet addAction:action];
//    
//    [[UIApplication sharedApplication].delegate.window.rootViewController
//     presentViewController:actionSheet
//     animated:YES
//     completion:nil];
}

- (void)observeValueForKeyPath:(nullable NSString *)keyPath ofObject:(nullable id)object change:(nullable NSDictionary<NSKeyValueChangeKey, id> *)change context:(nullable void *)context
{
//    if ([keyPath isEqualToString:@"duration"]) {
//        XMObject* subObject = self.player.objectToPlay;
//        self.globalTimeLength = self.globalXMObject.duration;
//        self.resStartTimePoint = subObject.timeLayout.startTime;
//        self.resTimeLength = (subObject.timeLayout.duration > 0) ? subObject.timeLayout.duration : subObject.duration;
//    }
}

@end
