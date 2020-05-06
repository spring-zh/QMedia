//
//  QObserverHoster.h
//  QMediaSDK
//
//  Created by LINQIUMING on 05/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol QObserverHosterInterface <NSObject>
@optional

@property (nonatomic, readonly) NSArray *observers;

- (void)addObserver:(id)observer;

- (void)removeObserver:(id)observer;

- (void)removeAllObservers;

- (void)notifyObservers:(SEL)selector;

- (void)notifyObservers:(SEL)selector withObject:(id)param;

- (void)notifyObservers:(SEL)selector withObjects:firstObj,...;

@end

@interface QObserverHoster : NSObject <QObserverHosterInterface>

@end
