
#import "TransformTableViewController.h"
#import "CommonKits.h"
#import <QMediaSDK/QMediaSDK.h>
#import "GlobalXMObject.h"

@interface TransformTableViewController () <QEditorPlayerObserver>
@property (nonatomic) IBOutlet UISlider* speedSlider;
@property (nonatomic) IBOutlet UILabel* speedLabel;

@property (nonatomic) IBOutlet UISlider* volumeSlider;
@property (nonatomic) IBOutlet UILabel* volumeLabel;

@property (nonatomic) IBOutlet UISlider* alphaSlider;
@property (nonatomic) IBOutlet UILabel* alphaLabel;

@property (nonatomic, strong) QEditorPlayer* player;

@property (nonatomic) CGFloat minSpeed;
@property (nonatomic) CGFloat maxSpeed;
@end

@implementation TransformTableViewController

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        self.minSpeed = 0.25;
        self.maxSpeed = 4.0;
        
        self.player = [GlobalXMObject sharedInstance].player;
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
    
    self.speedSlider.minimumValue = 0;
    self.speedSlider.maximumValue = 2.0;
    
    [self onPlayerChangedObjectToPlay];
    
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
    
    [CommonKits showAlertWithTitle:@"温馨提示" message:@"变速 和 透明度 仅对编辑中的子视频有用"];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)onPlayerChangedObjectToPlay
{
//    XMObject* xmObj = self.player.objectToPlay;
//
//    // Speed
//    CGFloat speed = 1.0;
//    if (xmObj.timeLayout && xmObj.timeLayout.timeScale > 0) {
//        speed = 1.0/xmObj.timeLayout.timeScale;
//        if (speed <= 1.0) {
//            self.speedSlider.value = (speed-self.minSpeed)/(1.0-self.minSpeed);
//        }
//        else {
//            self.speedSlider.value = (speed-1.0)/(self.maxSpeed-1.0)+1.0;
//        }
//    }
//    else {
//        self.speedSlider.value = 1.0;
//    }
//    self.speedLabel.text = [NSString stringWithFormat:@"%.2fx", speed];
//
//    // Volume
//    self.volumeSlider.value = xmObj.volume;
//    self.volumeLabel.text = [NSString stringWithFormat:@"%d%%", (int)(xmObj.volume*100)];
//
//    // Alpha
//    self.alphaSlider.value = xmObj.layout.alpha;
//    self.alphaLabel.text = [NSString stringWithFormat:@"%d%%", (int)(xmObj.layout.alpha*100)];
}

- (IBAction)onSpeedSliderValueChanged:(UISlider*)sender
{
//    XMObject* xmObj = self.player.objectToPlay;
//    CGFloat speed = 1.0;
//    if (self.speedSlider.value <= 1.0) {
//        speed = self.speedSlider.value*(1.0-self.minSpeed)+self.minSpeed;
//    }
//    else {
//        speed = (self.speedSlider.value-1.0)*(self.maxSpeed-1.0)+1.0;
//    }
//    xmObj.timeLayout.timeScale = 1.0/speed;
//    self.speedLabel.text = [NSString stringWithFormat:@"%.2fx", speed];
}

- (IBAction)onVolumeSliderValueChanged:(UISlider*)sender
{
//    XMObject* xmObj = self.player.objectToPlay;
//    xmObj.volume = self.volumeSlider.value;
//    self.volumeLabel.text = [NSString stringWithFormat:@"%d%%", (int)(xmObj.volume*100)];
}

- (IBAction)onAlphaSliderValueChanged:(UISlider*)sender
{
//    XMObject* xmObj = self.player.objectToPlay;
//    xmObj.layout.alpha = self.alphaSlider.value;
//    self.alphaLabel.text = [NSString stringWithFormat:@"%d%%", (int)(xmObj.layout.alpha*100)];
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
