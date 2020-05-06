//
//  QFileUtils.h
//  QMediaSDK
//
//  Created by LINQIUMING on 05/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 文件操作工具类
 */
@interface QFileUtils : NSObject

@end

@interface QFileUtils (XMDirectory)

/**
 创建目录
 
 @param directoryPath 目录路径

 @return 创建成功或目录已存在时返回YES，失败返回NO
 */
+ (BOOL)createDirectoryAtPath:(NSString *)directoryPath;

/**
 删除目录
 
 @param directoryPath 目录路径

 @return 成功返回YES，失败返回NO
 */
+ (BOOL)deleteDirectoryAtPath:(NSString *)directoryPath;

/**
 检测目录存在性
 
 @param directoryPath 目录路径
 
 @return 路径存在且为目录返回YES，路径存在且为文件或路径不存在返回NO
 */
+(BOOL)existDirectoryAtPath:(NSString *)directoryPath;

/**
 获取目录大小，单位:b
 
 @param directoryPath 目录路径
 
 @return 目录大小，若获取失败返回0
 */
+ (NSInteger)fileSizeOfDirectoryAtPath:(NSString *)directoryPath;

/**
 获取目录的创建日期
 
 @param directoryPath 目录路径
 
 @return 目录的创建日期，若获取失败返回nil
 */
+ (NSDate *)creationDateOfDirectoryAtPath:(NSString *)directoryPath;

/**
 获取目录的修改日期
 
 @param directoryPath 目录路径
 
 @return 目录的修改日期，若获取失败返回nil
 */
+ (NSDate *)modificationDateOfDirectoryAtPath:(NSString *)directoryPath;

/**
 获取目录的所有者名称
 
 @param directoryPath 目录路径
 
 @return 目录的所有者名称，若获取失败返回nil
 */
+ (NSString *)ownerNameOfDirectoryAtPath:(NSString *)directoryPath;

/**
 获取目录的组所有者名称
 
 @param directoryPath 目录路径
 
 @return 目录的组所有者名称，若获取失败返回nil
 */
+ (NSString *)groupOwnerNameOfDirectoryAtPath:(NSString *)directoryPath;

/**
 获取目录的属性字典
 
 属性列表见:https://developer.apple.com/reference/foundation/nsfilemanager/file_attribute_keys
 
 @param directoryPath 目录路径
 
 @return 目录的属性字典，若获取失败返回nil
 */
+ (NSDictionary *)attributesOfDirectoryAtPath:(NSString *)directoryPath;

/**
 获取目录的唯一标识符
 
 @param directoryPath 目录路径
 
 @return 目录的唯一标识符，若获取失败返回nil
 */
+ (NSString *)idOfDirectoryAtPath:(NSString *)directoryPath;

/**
 获取目录的可读属性
 
 @param directoryPath 目录路径
 
 @return 目录的可读属性，若可读返回YES
 */
+ (BOOL)isReadableDirectoryAtPath:(NSString *)directoryPath;

/**
 获取目录的可写属性
 
 @param directoryPath 目录路径
 
 @return 目录的可写属性，若可写返回YES
 */
+ (BOOL)isWritableDirectoryAtPath:(NSString *)directoryPath;

/**
 获取caches目录
 
 @return caches目录路径
 */
+ (NSString *)cachesDirectory;

/**
 获取resource目录
 
 @return resource目录路径
 */
+ (NSString *)resourceDirectory;

@end

@interface QFileUtils (XMFile)

/**
 删除文件
 
 该方法首先判断路径是否存在且为文件，若符合则删除文件。
 
 @param filePath  文件路径
 
 @return 删除成功返回YES，失败返回NO
 */
+ (BOOL)deleteFileAtPath:(NSString *)filePath;

/**
 检测文件存在性
 
 该方法用于判断路径是否存在且为文件，若符合返回YES。
 
 @param filePath 文件路径
 
 @return 路径存在且为文件返回YES，路径存在且为目录或路径不存在返回NO
 */
+ (BOOL)existFileAtPath:(NSString *)filePath;

/**
 获取文件大小，单位:b
 
 @param filePath 文件路径
 
 @return 文件大小，若获取失败返回0
 */
+ (NSInteger)fileSizeOfFileAtPath:(NSString *)filePath;

/**
 获取文件的创建日期
 
 @param filePath 文件路径
 
 @return 文件的创建日期，若获取失败返回nil
 */
+ (NSDate *)creationDateOfFileAtPath:(NSString *)filePath;

/**
 获取文件的修改日期
 
 @param filePath 文件路径
 
 @return 文件的修改日期，若获取失败返回nil
 */
+ (NSDate *)modificationDateOfFileAtPath:(NSString *)filePath;

/**
 获取文件的所有者名称
 
 @param filePath 文件路径
 
 @return 文件的所有者名称，若获取失败返回nil
 */
+ (NSString *)ownerNameOfFileAtPath:(NSString *)filePath;

/**
 获取文件的组所有者名称
 
 @param filePath 文件路径
 
 @return 文件的组所有者名称，若获取失败返回nil
 */
+ (NSString *)groupOwnerNameOfFileAtPath:(NSString *)filePath;

/**
 获取文件的属性字典
 
 属性列表见:https://developer.apple.com/reference/foundation/nsfilemanager/file_attribute_keys
 
 @param filePath 文件路径
 
 @return 文件的属性字典，若获取失败返回nil
 */
+ (NSDictionary *)attributesOfFileAtPath:(NSString *)filePath;

/**
 获取文件的唯一标识符
 
 @param filePath 文件路径
 
 @return 文件的唯一标识符，若获取失败返回nil
 */
+ (NSString *)idOfFileAtPath:(NSString *)filePath;

/**
 获取文件的可读属性
 
 @param filePath 文件路径
 
 @return 文件的可读属性，若可读返回YES
 */
+ (BOOL)isReadableFileAtPath:(NSString *)filePath;

/**
 获取文件的可写属性
 
 @param filePath 文件路径
 
 @return 文件的可写属性，若可写返回YES
 */
+ (BOOL)isWritableFileAtPath:(NSString *)filePath;

@end

@interface QFileUtils (XMIO)

@end

@interface QFileUtils (XMPath)

/**
 获取路径中的目录部分
 
 @param path 路径
 
 @return 路径中的目录部分
 */
+ (NSString *)directoryComponentOfPath:(NSString *)path;

/**
 获取路径中的文件部分
 
 @param path 路径
 
 @return 路径中的文件部分
 */
+ (NSString *)fileComponentOfPath:(NSString *)path;

@end
