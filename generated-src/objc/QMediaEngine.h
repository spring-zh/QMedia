// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_engine.djinni

#import "QAudioDescription.h"
#import <Foundation/Foundation.h>
@class QEditorExporterInternal;
@class QEditorPlayerInternal;


@interface QMediaEngine : NSObject

+ (nonnull NSString *)version;

/** set audio output paramenters */
+ (void)configAudioOut:(nonnull QAudioDescription *)desc;

+ (nullable QEditorPlayerInternal *)createEditorPlayerInternal;

+ (nullable QEditorExporterInternal *)createEditorExporterInternal;

@end
