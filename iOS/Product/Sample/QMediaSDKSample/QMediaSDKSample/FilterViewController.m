//
//  FilterViewController.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "FilterViewController.h"
#import "FilterCollectionViewController.h"
#import "FilterTableViewController.h"
#import "CommonKits.h"

@interface FilterViewController () <FilterCollectionViewControllerDelegate>
@property (nonatomic, weak) FilterCollectionViewController* collection;
@property (nonatomic, weak) FilterTableViewController* table;
@end

@implementation FilterViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    [CommonKits showAlertWithTitle:@"温馨提示" message:@"滤镜仅对子视频有效。\n目前滤镜功能尚有一些小Bug，请暂时不要同时添加多个滤镜！"];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
    
    if ([segue.identifier isEqualToString:@"FilterCollectionViewController"]) {
        self.collection = [segue destinationViewController];
        self.collection.delegate = self;
    }
    else if ([segue.identifier isEqualToString:@"FilterTableViewController"]) {
        self.table = [segue destinationViewController];
    }
}

- (void)onFilterCollectionViewDidSelectedFilterAtIndex:(NSInteger)index
{
    [self.table reloadData];
}

@end
