//
//  FilterTableViewController.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "FilterTableViewController.h"
#import "FilterTableViewCell.h"
#import "PreviewSliderBarController.h"
#import <QMediaSDK/QMediaSDK.h>

@interface FilterTableViewController () <QEditorPlayerObserver>
@property (nonatomic, weak) QEditorPlayer* player;
@end

@implementation FilterTableViewController

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
//        self.player = [XMPlayer sharedInstance];
        [self.player addObserver:self];
    }
    return self;
}

- (void)dealloc
{
    [self.player removeObserver:self];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)reloadData
{
    [self.tableView reloadData];
}

- (void)onPlayerChangedObjectToPlay
{
    [self.tableView reloadData];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
//    return self.player.objectToPlay.dresser.filters.count;
    return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    FilterTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"FilterTableViewCell" forIndexPath:indexPath];
//    XMFilter* filter = self.player.objectToPlay.dresser.filters[indexPath.row];
//    cell.filter = filter;
//    cell.filterNameLabel.text = filter.name;
//    cell.sliderBarCtrl.totalTimeLength = self.player.objectToPlay.duration;
//    if (filter.timeRange.duration <= 0) {
//        cell.sliderBarCtrl.trimLeftProgress = 0;
//        cell.sliderBarCtrl.trimRightProgress = 0;
//    }
//    else {
//        cell.sliderBarCtrl.trimLeftProgress = filter.timeRange.startPoint/self.player.objectToPlay.duration;
//        cell.sliderBarCtrl.trimRightProgress = (self.player.objectToPlay.duration-filter.timeRange.duration-filter.timeRange.startPoint)/self.player.objectToPlay.duration;
//    }
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:@"请选择操作"
                                                                         message:nil
                                                                  preferredStyle:UIAlertControllerStyleActionSheet];
//    UIAlertAction *action = [UIAlertAction actionWithTitle:@"删除"
//                                                     style:UIAlertActionStyleDestructive
//                                                   handler:^(UIAlertAction * _Nonnull action) {
//                                                       [self.player.objectToPlay.dresser removeFilter:self.player.objectToPlay.dresser.filters[indexPath.row]];
//                                                       [self.tableView reloadData];
//                                                   }];
//    [actionSheet addAction:action];
//    
//    action = [UIAlertAction actionWithTitle:@"取消"
//                                      style:UIAlertActionStyleCancel
//                                    handler:^(UIAlertAction * _Nonnull action) {
//                                        
//                                    }];
//    [actionSheet addAction:action];
    
    [[UIApplication sharedApplication].delegate.window.rootViewController
     presentViewController:actionSheet
     animated:YES
     completion:nil];
}


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
