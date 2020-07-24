//
//  TrackSettingTableViewController.m
//  QMediaSDKSample
//
//  Created by spring on 2020/7/16.
//  Copyright © 2020 QMedia. All rights reserved.
//

#import "TrackSettingTableViewController.h"
#import "GlobalXMObject.h"

@interface TrackSettingTableViewController() <UITextFieldDelegate, UIPickerViewDataSource, UIPickerViewDelegate>
@property (weak, nonatomic) IBOutlet UILabel *titleLB;
@property (weak, nonatomic) IBOutlet UITableViewCell *trackInfoCell;
@property (weak, nonatomic) IBOutlet UITableViewCell *renderCell;
@property (weak, nonatomic) IBOutlet UITableViewCell *audioCell;
@property (weak, nonatomic) IBOutlet UILabel *mediaDurationLB;
@property (weak, nonatomic) IBOutlet UITextField *displayStartTF;
@property (weak, nonatomic) IBOutlet UITextField *displayEndTF;
@property (weak, nonatomic) IBOutlet UITextField *sourceStartTF;
@property (weak, nonatomic) IBOutlet UITextField *sourceEndTF;

@property (weak, nonatomic) IBOutlet UISlider *speedSlider;
@property (weak, nonatomic) IBOutlet UILabel *speedTF;
@property (weak, nonatomic) IBOutlet UILabel *videoInfoLB;
@property (weak, nonatomic) IBOutlet UILabel *audioInfoLB;


@property (weak, nonatomic) IBOutlet UITextField *positionX;
@property (weak, nonatomic) IBOutlet UITextField *positionY;
@property (weak, nonatomic) IBOutlet UITextField *contentSizeX;
@property (weak, nonatomic) IBOutlet UITextField *contentSizeY;
@property (weak, nonatomic) IBOutlet UITextField *rotationTF;
@property (weak, nonatomic) IBOutlet UITextField *scaleXTF;
@property (weak, nonatomic) IBOutlet UITextField *scaleYTF;
@property (weak, nonatomic) IBOutlet UISlider *alphaSlider;
@property (weak, nonatomic) IBOutlet UILabel *alphaTF;
@property (weak, nonatomic) IBOutlet UISwitch *blendSW;
@property (weak, nonatomic) IBOutlet UIPickerView *srcPK;
@property (weak, nonatomic) IBOutlet UIPickerView *dstPK;

@property (weak, nonatomic) IBOutlet UISlider *volumSlider;
@property (weak, nonatomic) IBOutlet UISlider *pitchSlider;
@property (weak, nonatomic) IBOutlet UILabel *volumTF;
@property (weak, nonatomic) IBOutlet UILabel *pitchTF;


@property (weak, nonatomic) IBOutlet UIButton *deleteBTN;
@end

@implementation TrackSettingTableViewController {
    id<QTrack> _track;
    NSArray<NSString*>* _blendNameArray;
    NSArray<NSNumber*>* _blendValueArray;
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
    
    _srcPK.hidden = true;
    _srcPK.dataSource = self;
    _srcPK.delegate = self;
    [_srcPK selectRow:0 inComponent:0 animated:FALSE];
    _dstPK.hidden = true;
    _dstPK.dataSource = self;
    _dstPK.delegate = self;
    [_dstPK selectRow:0 inComponent:0 animated:FALSE];
    [_blendSW setOn:FALSE];
    
    _volumSlider.maximumValue = 2.0f;
    _volumSlider.minimumValue = 0;
    [_volumSlider addTarget:self action:@selector(sliderProgressChange:) forControlEvents:UIControlEventValueChanged];
    [_volumSlider addTarget:self action:@selector(sliderProgressUpInside:) forControlEvents:UIControlEventTouchUpInside];
    
    _pitchSlider.maximumValue = 4.0f;
    _pitchSlider.minimumValue = 0.25f;
    [_pitchSlider addTarget:self action:@selector(sliderProgressChange:) forControlEvents:UIControlEventValueChanged];
    [_pitchSlider addTarget:self action:@selector(sliderProgressUpInside:) forControlEvents:UIControlEventTouchUpInside];
    
    _blendNameArray = [NSArray arrayWithObjects:@"ZERO", @"ONE",@"SRC_COLOR",@"ONE_MINUS_SRC_COLOR",@"SRC_ALPHA",@"ONE_MINUS_SRC_ALPHA",@"DST_ALPHA",@"ONE_MINUS_DST_ALPHA", @"DST_COLOR",@"ONE_MINUS_DST_COLOR",nil];
    
    _blendValueArray = [NSArray arrayWithObjects:[NSNumber numberWithUnsignedInt:Blend_ZERO],
                        [NSNumber numberWithUnsignedInt:Blend_ONE],
                        [NSNumber numberWithUnsignedInt:Blend_SRC_COLOR],
                        [NSNumber numberWithUnsignedInt:Blend_ONE_MINUS_SRC_COLOR],
                        [NSNumber numberWithUnsignedInt:Blend_SRC_ALPHA],
                        [NSNumber numberWithUnsignedInt:Blend_ONE_MINUS_SRC_ALPHA],
                        [NSNumber numberWithUnsignedInt:Blend_DST_ALPHA],
                        [NSNumber numberWithUnsignedInt:Blend_ONE_MINUS_DST_ALPHA],
                        [NSNumber numberWithUnsignedInt:Blend_DST_COLOR],
                        [NSNumber numberWithUnsignedInt:Blend_ONE_MINUS_DST_COLOR],nil];
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
- (IBAction)toTop:(id)sender {
    QGraphicNode* graphicNode;
    if ([_track isKindOfClass:QMediaTrack.class]) {
        graphicNode = ((QMediaTrack*)_track).graphic;
    }else
        graphicNode = _track;
    
    [graphicNode.parent topChildNode:graphicNode];
    [[GlobalXMObject sharedInstance] updateDisplay];
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
    }else if([sender isEqual:_pitchSlider]) {
        _pitchTF.text = [NSString stringWithFormat:@"%0.2f", _pitchSlider.value];
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
    else if([sender isEqual:_pitchSlider]) {
        _pitchTF.text = [NSString stringWithFormat:@"%0.2f", _pitchSlider.value];
        QMediaTrack* mediaTrack = (QMediaTrack*)_track;
        mediaTrack.audio.pitch = [_pitchTF.text floatValue];
        return;
    }
    [[GlobalXMObject sharedInstance] updateDisplay];
}

- (void)updateSettingViews {
    NSInteger index = [GlobalXMObject sharedInstance].selectTrackIndex;
    if (index >= 0) {
        _trackInfoCell.hidden = false;
        _audioCell.hidden = false;
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
            if (mediaTrack.videoDesc) {
                _videoInfoLB.text = [NSString stringWithFormat:@"%d x %d", mediaTrack.videoDesc.width, mediaTrack.videoDesc.height];
            }else {
                _videoInfoLB.text = @"--";
            }
            
            if (mediaTrack.audioDesc) {
                _audioInfoLB.text = [NSString stringWithFormat:@"采样率: %d 通道: %d", mediaTrack.audioDesc.samplerate, mediaTrack.audioDesc.nchannel];
            }else {
                _audioInfoLB.text = @"--";
            }
            
        }else if ([_track isKindOfClass:QGraphicNode.class]) {
            graphicNode = _track;
            _audioCell.hidden = true;
            _speedSlider.enabled = false;
            _sourceStartTF.enabled = false;
            _sourceEndTF.enabled = false;
            _mediaDurationLB.text = @"";
            _videoInfoLB.text = @"--";
            _audioInfoLB.text = @"--";
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
        
        _titleLB.text = [NSString stringWithFormat:@"%@ 设置", _track.displayName];
        
        _displayStartTF.text = [[NSNumber numberWithLongLong: _track.displayRange.startPoint] stringValue];
        _displayEndTF.text = [[NSNumber numberWithLongLong: _track.displayRange.endPoint] stringValue];
        _sourceStartTF.text = [[NSNumber numberWithLongLong: _track.sourceRange.startPoint] stringValue];
        _sourceEndTF.text = [[NSNumber numberWithLongLong: _track.sourceRange.endPoint] stringValue];
        
    }else {
        _trackInfoCell.hidden = true;
        _renderCell.hidden = true;
        _audioCell.hidden = true;
        _deleteBTN.hidden = true;
        _titleLB.text = @"--";
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
            [graphicNode releaseIndex];
        }else if ([_track isKindOfClass:QMediaTrack.class]) {
            [[GlobalXMObject sharedInstance].player removeMediaTrack:_track];
        }
        [[GlobalXMObject sharedInstance] removeTrack:_track];
        [GlobalXMObject sharedInstance].selectTrackIndex = -1;
        [self updateSettingViews];
        [[GlobalXMObject sharedInstance] updateDisplay];
        _track = nil;
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
- (IBAction)blendSwitch:(id)sender {
    if (_blendSW.on) {
        _srcPK.hidden = false;
        _dstPK.hidden = false;
    }else {
        _srcPK.hidden = true;
        _dstPK.hidden = true;
        QGraphicNode* graphicNode;
        if ([_track isKindOfClass:QGraphicNode.class]) {
            graphicNode = (QGraphicNode*)_track;
        }else if ([_track isKindOfClass:QMediaTrack.class]) {
            graphicNode = ((QMediaTrack*)_track).graphic;
        }
        
        graphicNode.blendFunc = QBlendDisable;
        [[GlobalXMObject sharedInstance] updateDisplay];
    }
}

// returns the number of 'columns' to display.
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    return 1;
}

// returns the # of rows in each component..
- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    return _blendNameArray.count;
}

//- (nullable NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
//{
//    return [_blendNameArray objectAtIndex:row];
//}
- (UIView *)pickerView:(UIPickerView *)pickerView viewForRow:(NSInteger)row forComponent:(NSInteger)component reusingView:(UIView *)view{
 
    UILabel* pickerLabel = (UILabel*)view;
    if (!pickerLabel){
        pickerLabel = [[UILabel alloc] init];
        pickerLabel.adjustsFontSizeToFitWidth = YES;
        [pickerLabel setTextAlignment:NSTextAlignmentCenter];
        [pickerLabel setBackgroundColor:[UIColor clearColor]];
        pickerLabel.font = [UIFont systemFontOfSize:14]; ;
    }
    pickerLabel.text=[_blendNameArray objectAtIndex:row];
    return pickerLabel;
 
}



- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    NSInteger src_index = [_srcPK selectedRowInComponent:0];
    NSInteger dst_index = [_dstPK selectedRowInComponent:0];
    unsigned src_blend_type = [[_blendValueArray objectAtIndex:src_index] unsignedIntValue];
    unsigned dst_blend_type = [[_blendValueArray objectAtIndex:dst_index] unsignedIntValue];
    
    QGraphicNode* graphicNode;
    if ([_track isKindOfClass:QGraphicNode.class]) {
        graphicNode = (QGraphicNode*)_track;
    }else if ([_track isKindOfClass:QMediaTrack.class]) {
        graphicNode = ((QMediaTrack*)_track).graphic;
    }
    
    graphicNode.blendFunc = QBlendFuncMake(src_blend_type, dst_blend_type);
    [[GlobalXMObject sharedInstance] updateDisplay];
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
