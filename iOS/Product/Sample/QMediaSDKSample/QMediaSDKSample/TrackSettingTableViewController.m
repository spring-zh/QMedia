//
//  TrackSettingTableViewController.m
//  QMediaSDKSample
//
//  Created by spring on 2020/7/16.
//  Copyright © 2020 QMedia. All rights reserved.
//

#import "TrackSettingTableViewController.h"
#import "GlobalXMObject.h"

@interface TrackSettingTableViewController() <UITextFieldDelegate>
@property (weak, nonatomic) IBOutlet UITableViewCell *trackInfoCell;
@property (weak, nonatomic) IBOutlet UITableViewCell *renderCell;
@property (weak, nonatomic) IBOutlet UILabel *mediaDurationLB;
@property (weak, nonatomic) IBOutlet UITextField *displayStartTF;
@property (weak, nonatomic) IBOutlet UITextField *displayEndTF;
@property (weak, nonatomic) IBOutlet UITextField *sourceStartTF;
@property (weak, nonatomic) IBOutlet UITextField *sourceEndTF;

@property (weak, nonatomic) IBOutlet UISlider *speedSlider;
@property (weak, nonatomic) IBOutlet UILabel *speedTF;
@property (weak, nonatomic) IBOutlet UITextField *positionX;
@property (weak, nonatomic) IBOutlet UITextField *positionY;
@property (weak, nonatomic) IBOutlet UITextField *contentSizeX;
@property (weak, nonatomic) IBOutlet UITextField *contentSizeY;
@property (weak, nonatomic) IBOutlet UITextField *rotationTF;
@property (weak, nonatomic) IBOutlet UITextField *scaleXTF;
@property (weak, nonatomic) IBOutlet UITextField *scaleYTF;
@property (weak, nonatomic) IBOutlet UISlider *alphaSlider;
@property (weak, nonatomic) IBOutlet UILabel *alphaTF;
@property (weak, nonatomic) IBOutlet UIButton *deleteBTN;
@end

@implementation TrackSettingTableViewController {
    id<QTrack> _track;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.tableView.allowsSelection = false;
    self.view.subviews[0].clipsToBounds = NO;
    _displayStartTF.returnKeyType = UIReturnKeyDone;
//    _displayStartTF.keyboardType = UIKeyboardTypeDecimalPad;
    _displayStartTF.delegate = self;
    _displayEndTF.returnKeyType = UIReturnKeyDone;
    _displayEndTF.delegate = self;
    
    _sourceStartTF.returnKeyType = UIReturnKeyDone;
    _sourceStartTF.delegate = self;
    _sourceEndTF.returnKeyType = UIReturnKeyDone;
    _sourceEndTF.delegate = self;
    
    _positionX.returnKeyType = UIReturnKeyDone;
    _positionX.delegate = self;
    _positionY.returnKeyType = UIReturnKeyDone;
    _positionY.delegate = self;
    _contentSizeX.returnKeyType = UIReturnKeyDone;
    _contentSizeX.delegate = self;
    _contentSizeY.returnKeyType = UIReturnKeyDone;
    _contentSizeY.delegate = self;
    
    _rotationTF.returnKeyType = UIReturnKeyDone;
    _rotationTF.delegate = self;
    _scaleXTF.returnKeyType = UIReturnKeyDone;
    _scaleXTF.delegate = self;
    _scaleYTF.returnKeyType = UIReturnKeyDone;
    _scaleYTF.delegate = self;
    
    _speedSlider.minimumValue = 0.5f;
    _speedSlider.maximumValue = 2.0f;
    /// 事件监听
    [_speedSlider addTarget:self action:@selector(sliderProgressChange:) forControlEvents:UIControlEventValueChanged];
    [_speedSlider addTarget:self action:@selector(sliderProgressUpInside:) forControlEvents:UIControlEventTouchUpInside];
    
    _alphaSlider.minimumValue = 0.0f;
    _alphaSlider.maximumValue = 1.0f;
    _alphaSlider.value = 1.0f;
    /// 事件监听
    [_alphaSlider addTarget:self action:@selector(sliderProgressChange:) forControlEvents:UIControlEventValueChanged];
    [_alphaSlider addTarget:self action:@selector(sliderProgressUpInside:) forControlEvents:UIControlEventTouchUpInside];
    _track = nil;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-  (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self updateSettingViews];
}

//实现UITextField代理方法
- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];//取消第一响应者
    return YES;
}

//限制文本输入格式
- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
    return [self validateNumber:string];
}
-(BOOL)textFieldShouldEndEditing:(UITextField *)textField
{
    
    QGraphicNode* graphicNode;
    if ([_track isKindOfClass:QMediaTrack.class]) {
        graphicNode = ((QMediaTrack*)_track).graphic;
    }else
        graphicNode = _track;
    graphicNode.anchorPoint = CGPointMake(0.5, 0.5);
    if ([textField isEqual: _positionX] || [textField isEqual: _positionY]) {
        graphicNode.position = CGPointMake([_positionX.text floatValue], [_positionY.text floatValue]);
    }else if ([textField isEqual: _contentSizeX] || [textField isEqual: _contentSizeY]) {
        graphicNode.contentSize = CGSizeMake([_contentSizeX.text floatValue], [_contentSizeY.text floatValue]);
    }else if ([textField isEqual: _scaleXTF]) {
        graphicNode.scaleX = [_scaleXTF.text floatValue];
    }else if ([textField isEqual: _scaleYTF]) {
        graphicNode.scaleY = [_scaleYTF.text floatValue];
    }else if ([textField isEqual: _rotationTF]) {
        graphicNode.rotation = [_rotationTF.text floatValue];
    }else if ([textField isEqual: _alphaTF]) {
        graphicNode.alpha = [_alphaTF.text floatValue];
    }
    
    if([textField isEqual: _displayStartTF] || [textField isEqual: _displayEndTF]) {
        int64_t new_displayStart = [_displayStartTF.text intValue];
        int64_t new_displayEnd = [_displayEndTF.text intValue];
        _track.displayRange = QTimeRangeMake(new_displayStart, new_displayEnd);
    }else if([textField isEqual: _sourceStartTF] || [textField isEqual: _sourceEndTF]) {
        int64_t new_sourceStart = [_sourceStartTF.text intValue];
        int64_t new_sourceEnd = [_sourceEndTF.text intValue];
        _track.sourceRange = QTimeRangeMake(new_sourceStart, new_sourceEnd);
    }
    
    [[GlobalXMObject sharedInstance] updateDisplay];
    return YES;
}

- (void)sliderProgressChange:(id)sender {
    if([sender isEqual:_speedSlider]) {
        _speedTF.text = [NSString stringWithFormat:@"%0.1fx", _speedSlider.value];
    }else if([sender isEqual:_alphaSlider]) {
        _alphaTF.text = [NSString stringWithFormat:@"%0.1f", _alphaSlider.value];
    }
}

- (void)sliderProgressUpInside:(id)sender {
    if([sender isEqual:_speedSlider]) {
        _speedTF.text = [NSString stringWithFormat:@"%0.1fx", _speedSlider.value];
        QMediaTrack* mediaTrack = (QMediaTrack*)_track;
        mediaTrack.timeScale = 1.0f/ [_speedTF.text floatValue];
        int64_t start_pos = mediaTrack.displayRange.startPoint;
        int64_t duration = mediaTrack.timeScale * QTimeRangeGetLenght(mediaTrack.sourceRange);
        mediaTrack.displayRange = QTimeRangeMake(start_pos, start_pos+duration);
        
    }else if([sender isEqual:_alphaSlider]) {
        _alphaTF.text = [NSString stringWithFormat:@"%0.1f", _alphaSlider.value];
            QGraphicNode* graphicNode;
        if ([_track isKindOfClass:QMediaTrack.class]) {
            graphicNode = ((QMediaTrack*)_track).graphic;
        }else
            graphicNode = _track;
        
        graphicNode.alpha = [_alphaTF.text floatValue];
    }
    [[GlobalXMObject sharedInstance] updateDisplay];
}

- (void)updateSettingViews {
    NSInteger index = [GlobalXMObject sharedInstance].selectTrackIndex;
    if (index >= 0) {
        _trackInfoCell.hidden = false;
        _deleteBTN.hidden = false;
        
        _track = [[GlobalXMObject sharedInstance].tracks objectAtIndex:index];
        QGraphicNode* graphicNode = nil;
        if ([_track isKindOfClass:QMediaTrack.class]) {
            QMediaTrack* mediaTrack = _track;
            graphicNode = mediaTrack.graphic;
            _speedSlider.value = 1.0f/mediaTrack.timeScale;
            _speedSlider.enabled = true;
            _sourceStartTF.enabled = true;
            _sourceEndTF.enabled = true;
            _mediaDurationLB.text = [NSString stringWithFormat:@"媒体原始时长: %lld 毫秒", mediaTrack.getSourceDuration];
        }else if ([_track isKindOfClass:QGraphicNode.class]) {
            graphicNode = _track;
            _speedSlider.enabled = false;
            _sourceStartTF.enabled = false;
            _sourceEndTF.enabled = false;
            _mediaDurationLB.text = @"";
        }
        
        if (graphicNode != nil) {
            _renderCell.hidden = false;
            _positionX.text = [[NSNumber numberWithFloat: graphicNode.position.x] stringValue];
            _positionY.text = [[NSNumber numberWithFloat: graphicNode.position.y] stringValue];
            _contentSizeX.text = [[NSNumber numberWithFloat: graphicNode.contentSize.width] stringValue];
            _contentSizeY.text = [[NSNumber numberWithFloat: graphicNode.contentSize.height] stringValue];
            
            _scaleXTF.text = [[NSNumber numberWithFloat: graphicNode.scaleX] stringValue];
            _scaleYTF.text = [[NSNumber numberWithFloat: graphicNode.scaleY] stringValue];
            
            _rotationTF.text = [[NSNumber numberWithFloat: graphicNode.rotation] stringValue];
            _alphaTF.text = [[NSNumber numberWithFloat: graphicNode.alpha] stringValue];
        }else {
            _renderCell.hidden = true;
        }
        
        _displayStartTF.text = [[NSNumber numberWithLongLong: _track.displayRange.startPoint] stringValue];
        _displayEndTF.text = [[NSNumber numberWithLongLong: _track.displayRange.endPoint] stringValue];
        _sourceStartTF.text = [[NSNumber numberWithLongLong: _track.sourceRange.startPoint] stringValue];
        _sourceEndTF.text = [[NSNumber numberWithLongLong: _track.sourceRange.endPoint] stringValue];
        
    }else {
        _trackInfoCell.hidden = true;
        _renderCell.hidden = true;
        _deleteBTN.hidden = true;
    }
}

- (BOOL)validateNumber:(NSString*)number {

    BOOL res = YES;

    NSCharacterSet* tmpSet = [NSCharacterSet characterSetWithCharactersInString:@"0123456789."];

    int i = 0;

    while (i < number.length) {

        NSString * string = [number substringWithRange:NSMakeRange(i, 1)];

        NSRange range = [string rangeOfCharacterFromSet:tmpSet];

        if (range.length == 0) {

            res = NO;

            break;

        }

        i++;

    }

    return res;

}
- (IBAction)deleteClick:(id)sender {
    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:_track.displayName
                                                                             message:nil
                                                                      preferredStyle:UIAlertControllerStyleActionSheet];
    UIAlertAction *action = [UIAlertAction actionWithTitle:@"删除"
                                              style:UIAlertActionStyleDestructive
                                            handler:^(UIAlertAction * _Nonnull action) {
//                                                [self.globalXMObject removeSubObjectAtIndex:self.cellIndex];
        if ([_track isKindOfClass:QGraphicNode.class]) {
            QGraphicNode* graphicNode = (QGraphicNode*)_track;
            if (graphicNode.parent) {
                [graphicNode.parent removeChildNode:graphicNode];
            }
        }else if ([_track isKindOfClass:QMediaTrack.class]) {
//            QGraphicNode* graphicNode = ((QMediaTrack*)_track).graphic;
//            if (graphicNode && graphicNode.parent) {
//                [graphicNode.parent removeChildNode:graphicNode];
//            }
            [[GlobalXMObject sharedInstance].player removeMediaTrack:_track];
        }
        [[GlobalXMObject sharedInstance] removeTrack:_track];
        [GlobalXMObject sharedInstance].selectTrackIndex = -1;
        [self updateSettingViews];
        [[GlobalXMObject sharedInstance] updateDisplay];
    }];
    UIAlertAction *action_cancel = [UIAlertAction actionWithTitle:@"取消"
      style:UIAlertActionStyleDestructive
    handler:^(UIAlertAction * _Nonnull action) {
        
    }];
    [actionSheet addAction:action];
    [actionSheet addAction:action_cancel];

    [[UIApplication sharedApplication].delegate.window.rootViewController
         presentViewController:actionSheet
         animated:YES
         completion:nil];
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
