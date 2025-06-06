/*
 Copyright (c) 2020, Apple Inc. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 
 1.  Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 2.  Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.
 
 3.  Neither the name of the copyright holder(s) nor the names of any contributors
 may be used to endorse or promote products derived from this software without
 specific prior written permission. No license is granted to the trademarks of
 the copyright holders even if such marks are included in this software.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import <ResearchKit/ORKDefines.h>

NS_ASSUME_NONNULL_BEGIN

@class ORKHealthKitPermissionType;
@class ORKNotificationPermissionType;
@class ORKSensorPermissionType;
@class ORKMotionActivityPermissionType;
@class ORKLocationPermissionType;
@class HKSampleType, HKObjectType;

typedef NS_ENUM(NSInteger, ORKRequestPermissionsState) {
    ORKRequestPermissionsStateDefault = 0,
    ORKRequestPermissionsStateConnected,
    ORKRequestPermissionsStateNotSupported,
    ORKRequestPermissionsStateError,
};

typedef NS_OPTIONS(NSUInteger, UNAuthorizationOptions);
typedef NSString * SRSensor NS_TYPED_ENUM API_AVAILABLE(ios(14.0));

/**
 An abstract class that all permission types subclass from.
 */

ORK_CLASS_AVAILABLE
@interface ORKPermissionType : NSObject

@property (nonatomic, copy) void (^permissionsStatusUpdateCallback)(void);

@property (nonatomic, copy, readonly) NSString *localizedTitle;
@property (nonatomic, copy, readonly) NSString *localizedDetailText;
@property (nonatomic, strong, readonly) UIImage * _Nullable image;
@property (nonatomic, copy, readonly) UIColor *iconTintColor;
@property (nonatomic, assign, readonly) ORKRequestPermissionsState permissionState;
@property (nonatomic, assign, readonly) BOOL canContinue;

- (void)requestPermission;
- (void)cleanUp;

+ (ORKHealthKitPermissionType *)healthKitPermissionTypeWithSampleTypesToWrite:(nullable NSSet<HKSampleType *> *)sampleTypesToWrite
                                                            objectTypesToRead:(nullable NSSet<HKObjectType *> *)objectTypesToRead;

+ (ORKNotificationPermissionType *) notificationPermissionType:(UNAuthorizationOptions)options;

+ (ORKSensorPermissionType *) sensorPermissionType:(NSSet<SRSensor>*)sensors API_AVAILABLE(ios(14.0));

+ (ORKMotionActivityPermissionType *) deviceMotionPermissionType;

#if ORK_FEATURE_CLLOCATIONMANAGER_AUTHORIZATION
+ (ORKLocationPermissionType *) locationPermissionType;
#endif

@end

NS_ASSUME_NONNULL_END
