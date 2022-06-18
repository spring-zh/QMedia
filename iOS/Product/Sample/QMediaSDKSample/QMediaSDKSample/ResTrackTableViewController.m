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

@interface ResTrackTableViewController () <GlobalMeidaManageObserver>
@property (nonatomic, weak) GlobalXMObject* globalXMObject;
@property (nonatomic, weak) QEditorPlayer* player;
@end

NSString* GetFileNameFromPath(NSString* path) {
    return [path lastPathComponent];
}

@implementation ResTrackTableViewController

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        self.globalXMObject = [GlobalXMObject sharedInstance];
        self.player = [GlobalXMObject sharedInstance].player;
//        [self.player addObserver:self];
        [[GlobalXMObject sharedInstance].observers addObject:self];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(onNotification:)
                                                     name:kGlobalXMObjectSubObjectArrayChanged
                                                   object:nil];
    }
    return self;
}

- (void)dealloc
{
//    [self.player removeObserver:self];
    [[GlobalXMObject sharedInstance].observers removeObject:self];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

-  (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self.tableView reloadData];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

//- (void)onPlayerChangedObjectToPlay
//{
//    [self.tableView reloadData];
//}

-(void)onTrackChange {
    [self.tableView reloadData];
}
-(void)onSelectTrackAtIndex:(NSInteger)index {
    _selectRow = index;
    NSInteger sections = self.tableView.numberOfSections;
    for (int section = 0; section < sections; section++) {
        NSInteger rows =  [self.tableView numberOfRowsInSection:section];
        for (int row = 0; row < rows; row++) {
            NSIndexPath *indexPath = [NSIndexPath indexPathForRow:row inSection:section];
            UITableViewCell* cell = [self.tableView cellForRowAtIndexPath:indexPath];
            if (index == row) {
                cell.backgroundColor = [UIColor cyanColor];
                cell.layer.borderColor = [UIColor cyanColor].CGColor;
                cell.layer.borderWidth = 2;
            }else
            {
                cell.backgroundColor = nil;
                cell.layer.borderColor = nil;
                cell.layer.borderWidth = 0;
            }
        }
    }
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
    return self.globalXMObject.tracks.count;
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    ResTrackTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"ResTrackTableViewCell" forIndexPath:indexPath];
    
    cell.cellIndex = indexPath.row;
    QMediaSegment* track = [self.globalXMObject.tracks objectAtIndex:indexPath.row];
    cell.globalTimeLength = QMediaRangeGetLenght([self.player getTotalTimeRange])/1000.0f ;
    cell.sourceTimeLength = QMediaRangeGetLenght([track getSourceRange])/1000.0f;
    cell.resTimeLength = QMediaRangeGetLenght(([track getDisplayRange]))/1000.0f;
    cell.resStartTimePoint = ([track getDisplayRange]).start/1000.0f ;
    cell.resNameLabel.text = GetFileNameFromPath([track getFileName]);
    cell.tableViewController = self;
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    _selectRow = indexPath.row;
}

- (void)selectRowByIndex:(NSUInteger)selectRow {
    self.globalXMObject.selectTrackIndex = selectRow;
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
