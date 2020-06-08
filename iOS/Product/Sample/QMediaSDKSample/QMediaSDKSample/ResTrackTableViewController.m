//
//  ResTrackTableViewController.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "ResTrackTableViewController.h"
#import "ResTrackTableViewCell.h"
#import "GlobalXMObject.h"
#import <QMediaSDK/QMediaSDK.h>

@interface ResTrackTableViewController () <QEditorPlayerObserver>
@property (nonatomic, weak) GlobalXMObject* globalXMObject;
@property (nonatomic, weak) QEditorPlayer* player;
@end

@implementation ResTrackTableViewController

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        self.globalXMObject = [GlobalXMObject sharedInstance];
        [self.player addObserver:self];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(onNotification:)
                                                     name:kGlobalXMObjectSubObjectArrayChanged
                                                   object:nil];
    }
    return self;
}

- (void)dealloc
{
    [self.player removeObserver:self];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self];
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

- (void)onPlayerChangedObjectToPlay
{
    [self.tableView reloadData];
}

-(void)onNotification:(NSNotification *)notification{
    NSString *name = [notification name];
    if ([name isEqualToString:kGlobalXMObjectSubObjectArrayChanged]) {
        [self.tableView reloadData];
    }
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.player.subObjects.count;
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    ResTrackTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"ResTrackTableViewCell" forIndexPath:indexPath];
    
    cell.cellIndex = indexPath.row;
    QMediaTrack *track = [self.player.subObjects objectAtIndex:indexPath];
//    XMObject* subObject = self.globalXMObject.subObjects[indexPath.row];
    cell.globalTimeLength = self.player.mediaTimeRange.length ;
    cell.resTimeLength = track.displayRange.length;
    cell.resStartTimePoint = track.displayRange.location;
    cell.resNameLabel.text = @"subObject.name";
    return cell;
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
