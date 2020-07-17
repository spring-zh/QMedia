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
@property (weak, nonatomic) IBOutlet UITextField *alphaTF;
@property (weak, nonatomic) IBOutlet UITextField *scaleXTF;
@property (weak, nonatomic) IBOutlet UITextField *scaleYTF;
@end

@implementation TrackSettingTableViewController

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
    _alphaTF.returnKeyType = UIReturnKeyDone;
    _alphaTF.delegate = self;
    _scaleXTF.returnKeyType = UIReturnKeyDone;
    _scaleXTF.delegate = self;
    _scaleYTF.returnKeyType = UIReturnKeyDone;
    _scaleYTF.delegate = self;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewWillDisappear {
    [self updateSettingViews];
}

//实现UITextField代理方法
- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];//取消第一响应者
    return YES;
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
    return [self validateNumber:string];
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

- (void)updateSettingViews {
    NSInteger index = [GlobalXMObject sharedInstance].selectTrackIndex;
    if (index >= 0) {
        id<QTrack> track = [[GlobalXMObject sharedInstance].tracks objectAtIndex:index];
        QGraphicNode* graphicNode = nil;
        if ([track isKindOfClass:QMediaTrack.class]) {
            graphicNode = ((QMediaTrack*)track).graphic;
        }else if ([track isKindOfClass:QGraphicNode.class]) {
            graphicNode = track;
        }
        
        _displayStartTF.text = [[NSNumber numberWithLongLong: track.displayRange.startPoint] stringValue];
        
        _displayEndTF.text = [[NSNumber numberWithLongLong: track.displayRange.endPoint] stringValue];
        
        _sourceStartTF.text = [[NSNumber numberWithLongLong: track.sourceRange.startPoint] stringValue];
        
        _sourceEndTF.text = [[NSNumber numberWithLongLong: track.sourceRange.endPoint] stringValue];
    }
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
