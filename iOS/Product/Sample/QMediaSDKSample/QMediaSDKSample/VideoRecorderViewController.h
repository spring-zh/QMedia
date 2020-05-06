

#import <UIKit/UIKit.h>

@class VideoRecorderViewController;

@protocol VideoRecorderViewControllerDelegate <NSObject>
- (void)onVideoRecorderViewController:(VideoRecorderViewController*)controller
                      savedToFilePath:(NSString*)path;
@end

@interface VideoRecorderViewController : UIViewController
@property (nonatomic, weak) id<VideoRecorderViewControllerDelegate> delegate;
@end
