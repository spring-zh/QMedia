//
//  QFileUtils.m
//  QMediaSDK
//
//  Created by LINQIUMING on 05/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QFileUtils.h"

@implementation QFileUtils : NSObject

@end

@implementation QFileUtils (XMDirectory)

+ (BOOL)createDirectoryAtPath:(NSString *)directoryPath {
    
    if(directoryPath){
        if(![QFileUtils existDirectoryAtPath:directoryPath]){
            NSFileManager *fileManager = [NSFileManager defaultManager];
            return [fileManager createDirectoryAtPath:directoryPath
                          withIntermediateDirectories:YES
                                           attributes:nil
                                                error:nil];
        }else{
            return YES;
        }
    }
    return NO;
    
}

+ (BOOL)deleteDirectoryAtPath:(NSString *)directoryPath {
    
    if(directoryPath){
        if([QFileUtils existDirectoryAtPath:directoryPath]){
            NSFileManager *fileManager = [NSFileManager defaultManager];
            return [fileManager removeItemAtPath:directoryPath
                                           error:nil];
        }else{
            return YES;
        }
    }
    return NO;
    
}

+ (BOOL)existDirectoryAtPath:(NSString *)directoryPath {
    
    if(directoryPath){
        NSFileManager *fileManager = [NSFileManager defaultManager];
        BOOL isExist = NO;
        BOOL isDirectory = NO;
        isExist = [fileManager fileExistsAtPath:directoryPath
                                    isDirectory:&isDirectory];
        if(isExist && !isDirectory){
            isExist = NO;
        }
        return isExist;
    }
    return NO;
    
}

+ (NSInteger)fileSizeOfDirectoryAtPath:(NSString *)directoryPath {
    
    NSInteger directorySize = 0;
    NSDictionary *fileAttributeDict = [QFileUtils attributesOfDirectoryAtPath:directoryPath];
    if(fileAttributeDict){
        NSNumber *sizeNumber = fileAttributeDict[NSFileSize];
        if(sizeNumber != nil){
            directorySize = [sizeNumber integerValue];
        }
    }
    return directorySize;
    
}

+ (NSDate *)creationDateOfDirectoryAtPath:(NSString *)directoryPath {
    
    NSDictionary *fileAttributeDict = [QFileUtils attributesOfDirectoryAtPath:directoryPath];
    if(fileAttributeDict){
        return fileAttributeDict[NSFileCreationDate];
    }
    return nil;
    
}

+ (NSDate *)modificationDateOfDirectoryAtPath:(NSString *)directoryPath {
    
    NSDictionary *fileAttributeDict = [QFileUtils attributesOfDirectoryAtPath:directoryPath];
    if(fileAttributeDict){
        return fileAttributeDict[NSFileModificationDate];
    }
    return nil;
    
}

+ (NSString *)ownerNameOfDirectoryAtPath:(NSString *)directoryPath {
    
    NSDictionary *fileAttributeDict = [QFileUtils attributesOfDirectoryAtPath:directoryPath];
    if(fileAttributeDict){
        return fileAttributeDict[NSFileOwnerAccountName];
    }
    return nil;
    
}

+ (NSString *)groupOwnerNameOfDirectoryAtPath:(NSString *)directoryPath {
    
    NSDictionary *fileAttributeDict = [QFileUtils attributesOfDirectoryAtPath:directoryPath];
    if(fileAttributeDict){
        return fileAttributeDict[NSFileGroupOwnerAccountName];
    }
    return nil;
    
}

+ (NSDictionary *)attributesOfDirectoryAtPath:(NSString *)directoryPath {
    
    if([QFileUtils existDirectoryAtPath:directoryPath]){
        NSFileManager *fileManager = [NSFileManager defaultManager];
        return [fileManager attributesOfItemAtPath:directoryPath
                                             error:nil];
    }
    return nil;
    
}

+ (NSString *)idOfDirectoryAtPath:(NSString *)directoryPath {
    
    return [[QFileUtils attributesOfDirectoryAtPath:directoryPath] descriptionInStringsFileFormat];
    
}

+ (BOOL)isReadableDirectoryAtPath:(NSString *)directoryPath {
    
    if([QFileUtils existDirectoryAtPath:directoryPath]){
        NSFileManager *fileManager = [NSFileManager defaultManager];
        return [fileManager isReadableFileAtPath:directoryPath];
    }
    return NO;
    
}

+ (BOOL)isWritableDirectoryAtPath:(NSString *)directoryPath {
    
    if([QFileUtils existDirectoryAtPath:directoryPath]){
        NSFileManager *fileManager = [NSFileManager defaultManager];
        return [fileManager isWritableFileAtPath:directoryPath];
    }
    return NO;
    
}

+ (NSString *)cachesDirectory {
    
    NSArray *pathArray = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    return [pathArray objectAtIndex:0];
    
}

+ (NSString *)resourceDirectory {
    
    return [[NSBundle mainBundle] resourcePath];
    
}

@end

@implementation QFileUtils (XMFile)

+ (BOOL)deleteFileAtPath:(NSString *)filePath {
    
    if(filePath){
        if([QFileUtils existFileAtPath:filePath]){
            NSFileManager *fileManager = [NSFileManager defaultManager];
            return [fileManager removeItemAtPath:filePath
                                           error:nil];
        }else{
            return YES;
        }
    }
    return NO;
    
}

+ (BOOL)existFileAtPath:(NSString *)filePath {
    
    if(filePath){
        NSFileManager *fileManager = [NSFileManager defaultManager];
        BOOL isExist = NO;
        BOOL isDirectory = NO;
        isExist = [fileManager fileExistsAtPath:filePath
                                    isDirectory:&isDirectory];
        if(isExist && isDirectory){
            isExist = NO;
        }
        return isExist;
    }
    return NO;
    
}

+ (NSInteger)fileSizeOfFileAtPath:(NSString *)filePath {
    
    NSInteger fileSize = 0;
    NSDictionary *fileAttributeDict = [QFileUtils attributesOfFileAtPath:filePath];
    if(fileAttributeDict){
        NSNumber *sizeNumber = fileAttributeDict[NSFileSize];
        if(sizeNumber != nil){
            fileSize = [sizeNumber integerValue];
        }
    }
    return fileSize;
    
}

+ (NSDate *)creationDateOfFileAtPath:(NSString *)filePath {

    NSDictionary *fileAttributeDict = [QFileUtils attributesOfFileAtPath:filePath];
    if(fileAttributeDict){
        return fileAttributeDict[NSFileCreationDate];
    }
    return nil;
    
}

+ (NSDate *)modificationDateOfFileAtPath:(NSString *)filePath {
    
    NSDictionary *fileAttributeDict = [QFileUtils attributesOfFileAtPath:filePath];
    if(fileAttributeDict){
        return fileAttributeDict[NSFileModificationDate];
    }
    return nil;
    
}

+ (NSString *)ownerNameOfFileAtPath:(NSString *)filePath {
    
    NSDictionary *fileAttributeDict = [QFileUtils attributesOfFileAtPath:filePath];
    if(fileAttributeDict){
        return fileAttributeDict[NSFileOwnerAccountName];
    }
    return nil;
    
}

+ (NSString *)groupOwnerNameOfFileAtPath:(NSString *)filePath {
    
    NSDictionary *fileAttributeDict = [QFileUtils attributesOfFileAtPath:filePath];
    if(fileAttributeDict){
        return fileAttributeDict[NSFileGroupOwnerAccountName];
    }
    return nil;
    
}

+ (NSDictionary *)attributesOfFileAtPath:(NSString *)filePath {
    
    if([QFileUtils existFileAtPath:filePath]){
        NSFileManager *fileManager = [NSFileManager defaultManager];
        return [fileManager attributesOfItemAtPath:filePath
                                             error:nil];
    }
    return nil;
    
}

+ (NSString *)idOfFileAtPath:(NSString *)filePath {
    
    return [[QFileUtils attributesOfFileAtPath:filePath] descriptionInStringsFileFormat];
    
}

+ (BOOL)isReadableFileAtPath:(NSString *)filePath {
    
    if([QFileUtils existFileAtPath:filePath]){
        NSFileManager *fileManager = [NSFileManager defaultManager];
        return [fileManager isReadableFileAtPath:filePath];
    }
    return NO;
    
}

+ (BOOL)isWritableFileAtPath:(NSString *)filePath {
    
    if([QFileUtils existFileAtPath:filePath]){
        NSFileManager *fileManager = [NSFileManager defaultManager];
        return [fileManager isWritableFileAtPath:filePath];
    }
    return NO;
    
}

@end

@implementation QFileUtils (XMIO)
+ (NSString*) getFileFromMainbundleAbsolutePath:(NSString*) fileCompent
{
    return [NSString stringWithFormat:@"%@/%@",[[NSBundle mainBundle] resourcePath], fileCompent];
}
@end

@implementation QFileUtils (XMPath)

+ (NSString *)directoryComponentOfPath:(NSString *)path {
    
    return [path stringByDeletingLastPathComponent];
    
}

+ (NSString *)fileComponentOfPath:(NSString *)path {
    
    return [path lastPathComponent];
    
}

@end
