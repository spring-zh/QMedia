//
//  SettingsTableViewController.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 17/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "SettingsTableViewController.h"
#import "MSColorSelectionViewController.h"
#import "GlobalXMObject.h"
#import "PixelSizeManager.h"
#import <QMediaSDK/QMediaSDK.h>

@interface SettingsTableViewController () <UIPickerViewDataSource, UIPickerViewDelegate, MSColorSelectionViewControllerDelegate>
@property (nonatomic, weak) IBOutlet UIPickerView* sizePicker;
@property (nonatomic, weak) IBOutlet UIView* colorView;
@property (nonatomic, weak) IBOutlet UIButton* applyButton;

@property (nonatomic, weak) GlobalXMObject* globalXMObject;

@property (nonatomic, weak) PixelSizeManager* pixelMngr;
@property (nonatomic) NSInteger selectedRow;
@end

@implementation SettingsTableViewController

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        self.globalXMObject = [GlobalXMObject sharedInstance];
        self.pixelMngr = [PixelSizeManager sharedInstance];
    }
    return self;
}

- (void)dealloc
{
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
    
//    self.colorView.backgroundColor = self.globalXMObject.layout.backgroundColor;
    
    [self.sizePicker selectRow:self.globalXMObject.selectedPixelSizeIndex
                   inComponent:0
                      animated:NO];
    
    self.applyButton.enabled = NO;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)onPixelApplyButtonPressed:(id)sender
{
    self.globalXMObject.selectedPixelSizeIndex = self.selectedRow;
    
    self.applyButton.enabled = NO;
}

- (IBAction)onColorViewTapped:(id)sender
{
    MSColorSelectionViewController *colorSelectionController = [[MSColorSelectionViewController alloc] init];
    UINavigationController *navCtrl = [[UINavigationController alloc] initWithRootViewController:colorSelectionController];
    
    navCtrl.modalPresentationStyle = UIModalPresentationFullScreen;
    navCtrl.preferredContentSize = [colorSelectionController.view systemLayoutSizeFittingSize:UILayoutFittingCompressedSize];
    
    colorSelectionController.delegate = self;
//    colorSelectionController.color = self.globalXMObject.layout.backgroundColor;
    
    if (self.traitCollection.horizontalSizeClass == UIUserInterfaceSizeClassCompact) {
        UIBarButtonItem *doneBtn = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"Done", ) style:UIBarButtonItemStyleDone target:self action:@selector(ms_dismissViewController:)];
        colorSelectionController.navigationItem.rightBarButtonItem = doneBtn;
    }
    
    [self presentViewController:navCtrl animated:YES completion:nil];
}

- (void)ms_dismissViewController:(id)sender
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

#pragma mark - UIPickerViewDataSource && UIPickerViewDelegate
// returns the number of 'columns' to display.
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    return 1;
}

// returns the # of rows in each component..
- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    return self.pixelMngr.allPixelSizes.count;
}

- (nullable NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return self.pixelMngr.allPixelSizes[row].name;
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    self.selectedRow = row;
    
    if (self.globalXMObject.selectedPixelSizeIndex != self.selectedRow) {
        self.applyButton.enabled = YES;
    }
}

#pragma mark - MSColorViewDelegate

- (void)colorViewController:(MSColorSelectionViewController *)colorViewCntroller didChangeColor:(UIColor *)color
{
    CGFloat red = 0.0;
    CGFloat green = 0.0;
    CGFloat blue = 0.0;
    CGFloat alpha = 0.0;
    [color getRed:&red green:&green blue:&blue alpha:&alpha];
    self.globalXMObject.player.rootNode.bkColor = QColorMake(red, green, blue, alpha);
    [self.globalXMObject updateDisplay];
}

#pragma mark - Table view data source

//- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
//#warning Incomplete implementation, return the number of sections
//    return 0;
//}
//
//- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
//#warning Incomplete implementation, return the number of rows
//    return 0;
//}

/*
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:<#@"reuseIdentifier"#> forIndexPath:indexPath];
    
    // Configure the cell...
    
    return cell;
}
*/

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    } else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
