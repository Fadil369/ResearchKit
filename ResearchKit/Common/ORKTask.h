/*
 Copyright (c) 2015, Apple Inc. All rights reserved.
 
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

#if ORK_FEATURE_HEALTHKIT_AUTHORIZATION
#import <HealthKit/HealthKit.h>
#endif


#if TARGET_OS_IOS
#import <ResearchKit/ORKTypes.h>
#endif


NS_ASSUME_NONNULL_BEGIN

@class ORKStep;
@class ORKTaskResult;

/**
 The `ORKTaskFinishReason` value indicates how the task has finished.
 */
typedef NS_ENUM(NSInteger, ORKTaskFinishReason) {
    
    /// The task was canceled by the participant or the developer, and the participant asked to save the current result.
    ORKTaskFinishReasonSaved,
    
    /// The task was canceled by the participant or the developer, and the participant asked to discard the current result.
    ORKTaskFinishReasonDiscarded,
    
    /// The task has completed successfully, because all steps have been completed.
    ORKTaskFinishReasonCompleted,
    
    /// An error was detected during the current step.
    ORKTaskFinishReasonFailed,
    
    /// Interntional early termination of a task
    ORKTaskFinishReasonEarlyTermination
};

/**

 A structure that represents how far a task has progressed.
 
 Objects that implement the `ORKTask` protocol return the task progress structure to indicate
 to the task view controller how far the task has progressed.
 
 Note that the values in an `ORKTaskProgress` structure are used only for display; you don't use the values to access the steps in a task.
 */
typedef struct {
    /// The index of the current step, starting from 0.
    NSUInteger current;
    
    /// The total number of steps in the task.
    NSUInteger total;
    
    /// Indicates if the the step should present a progress label
    BOOL shouldBePresented;
} ORKTaskProgress ORK_AVAILABLE_DECL;

/**
 Returns a task progress structure with the specified current and total values.
 
 @param current   The index of the current step, starting from 0.
 @param total     The total number of steps.
 
 @return A task progress structure.
 */
ORK_EXTERN ORKTaskProgress ORKTaskProgressMake(NSUInteger current, NSUInteger total) ORK_AVAILABLE_DECL;

/**

 A structure that represents how far a task has progressed based on the total amount of questions.
  
 Note that the values in an `ORKTotalTaskProgress` structure are used only for display; you don't use the values to access the steps in a task.
 */
typedef struct {
    /// The position of the current question based on the total amount of questions within the task.
    NSUInteger currentStepStartingProgressPosition;
    
    /// The total number of questions in the task.
    NSUInteger total;
    
    /// Determines if the task should display the progress based on the total amount of questions in the task or the total amount in each step
    BOOL stepShouldShowTotalProgress;
} ORKTaskTotalProgress ORK_AVAILABLE_DECL;

/**
 Returns a task progress structure with the specified current and total values.
 
 @param currentStepStartingProgressPosition   The position of the current question based on the total amount of questions within the task.
 @param total     The total number of questions in the task.
 @param stepShouldShowTotalProgress     Determines if the task should display the progress based on the total amount of questions in the task or the total amount in each step
 
 @return A task progress structure.
 */
ORK_EXTERN ORKTaskTotalProgress ORKTaskTotalProgressMake(NSUInteger currentStepStartingProgressPosition, NSUInteger total, BOOL stepShouldShowTotalProgress) ORK_AVAILABLE_DECL;

/**
 A protocol that defines a task that a participant carries out in a research study.
 
 To present the ResearchKit framework UI in your app, instantiate an
 object that implements the `ORKTask` protocol (such as `ORKOrderedTask`) and
 provide it to an `ORKTaskViewController` object.
 
 Implement this protocol to enable dynamic selection of the steps for a given task.
 By default, `ORKOrderedTask` implements this protocol for simple sequential tasks.
 
 Each step (`ORKStep`) in a task roughly corresponds to one screen, and represents the
 primary unit of work in any task presented by a task view controller. For example,
 an `ORKQuestionStep` object corresponds to a single question presented on screen,
 together with controls the participant uses to answer the question. Another example is `ORKFormStep`, which corresponds to a single
 screen that displays multiple questions or items for which participants provide information, such as first name, last name, and birth date.
 
 Each step corresponds to one
 `ORKStepViewController` object, which may manage child view controllers in a particular
 sequence. The correspondence of step to view controller holds even though some steps, such as `ORKVisualConsentStep` and
 `ORKConsentReviewStep`, can produce multiple screens.
 */
ORK_AVAILABLE_DECL
@protocol ORKTask <NSObject>

@required
/**
 The unique identifier for this task.
 
 The identifier should be a short string that identifies the task. The identifier is copied
 into the `ORKTaskResult` objects generated by the task view controller for this
 task. You can use a human-readable string for the task identifier
 or a UUID; the exact string you use depends on your app.
 
 In the case of apps whose tasks come from a server, the unique
 identifier for the task may be in an external database.
 
 The task view controller uses the identifier when constructing the task result.
 The identifier can also be used during UI state restoration to identify the
 task that needs to be restored.
 */
@property (nonatomic, copy, readonly) NSString *identifier;

/**
 Returns the step after the specified step, if there is one.
 
 This method lets you use a result to determine the next step.
 
 The task view controller calls this method to determine the step to display
 after the specified step. The task view controller can also call this method every time the result updates, to determine if the new result changes which steps are available.
 
 If you need to implement this method, take care to avoid creating a confusing sequence of steps. As much as possible, use `ORKOrderedTask` instead.
 
 @param step     The reference step. Pass `nil` to specify the first step.
 @param result   A snapshot of the current set of results.
 
 @return The step that comes after the specified step, or `nil` if there isn't one.
 */
- (nullable ORKStep *)stepAfterStep:(nullable ORKStep *)step withResult:(ORKTaskResult *)result;

/**
 Returns the step that precedes the specified step, if there is one.
 
 The task view controller calls this method to determine the step to display
 before the specified step. The task view controller can also call this method every time the result changes, to determine if the new result changes which steps are available.
 
 If you need to implement this method, take care to avoid creating a confusing sequence of steps. As much as possible, use `ORKOrderedTask` instead. Returning `nil` prevents the user from navigating back to a previous step.
 
 @param step     The reference step. Pass `nil` to specify the last step.
 @param result   A snapshot of the current set of results.
 
 @return The step that precedes the reference step, or `nil` if there isn't one.
 */
- (nullable ORKStep *)stepBeforeStep:(nullable ORKStep *)step withResult:(ORKTaskResult *)result;

@optional
/**
 Returns the step that matches the specified identifier.
 
 Implementing this method allows state restoration of a task
 to a particular step. If you don't implement this method, `ORKTaskViewController` restores the state
 to the first step of the task.
 
 @param identifier  The identifier of the step to restore.
 
 @return The step that matches the specified identifier.
 */
- (nullable ORKStep *)stepWithIdentifier:(NSString *)identifier;

/**
 Returns the progress of the current step.
 
 During a task, the task view controller can display the progress (that is, the current step number
 out of the total number of steps) in the navigation bar. Implement
 this method to control what is displayed; if you don't implement this method, the progress label does not appear.
 If the returned `ORKTaskProgress` object has a count of 0, the progress is not displayed.

 @param step    The current step.
 @param result  A snapshot of the current set of results.
 
 @return The current step's index and the total number of steps in the task, as an `ORKTaskProgress` object.
 */
- (ORKTaskProgress)progressOfCurrentStep:(ORKStep *)step withResult:(ORKTaskResult *)result;

/**
 Returns the progress of the current step based on the total amount of questions in the task.
 
 During a task, each questions can display its progress (that is, the current question number out of the total amount of questions in the task) in its header view.

 @param currentStep    The current step.
 
 @return The current step's index and the total number of steps in the task, as an `ORKTaskProgress` object.
 The index of the current step's first question, the total amount of questions in the task, and a BOOL to determine if the step should display its progress labels in this manner.
 */
- (ORKTaskTotalProgress)totalProgressOfCurrentStep:(ORKStep *)currentStep;

/**
 Returns true if the provided step is the first or last object in the array of steps

 @param step    The current step.
 */

- (BOOL)shouldHideProgressFor:(ORKStep *)step;

/**
 Validates the task parameters.
 
 The implementation of this method should check that all the task parameters are correct. An invalid task
 is considered an unrecoverable error: the implementation should throw an exception on parameter validation failure.
 For example, the `ORKOrderedTask` implementation makes sure that all its step identifiers are unique, throwing an
 exception otherwise.
 
 This method is usually called by a task view controller when its task is set.
 */
- (void)validateParameters;

/**
 The set of HealthKit types that steps in the task need to be able to
 read. (read-only)
 
 The value of this property is a set of `HKObjectType` values to request for reading from HealthKit during this task. After the last of the initial instruction steps, the task view controller
requests access to these HealthKit types.
 
 To set this property, you can scan the steps in the task
 and collate the HealthKit types that are requested by each active step, question, or
 form step that has a Health answer format, and then include any additional types known
 to be required. (Note that `ORKOrderedTask` does something similar for this property.)
 
 See also: `requestedHealthKitTypesForWriting`.
 */
#if ORK_FEATURE_HEALTHKIT_AUTHORIZATION
@property (nonatomic, copy, readonly, nullable) NSSet<HKObjectType *> *requestedHealthKitTypesForReading;

/**
 The set of HealthKit types for which the task needs to request write access.
 
 The requested `HKObjectType` values for writing can be returned by an extended task,
 to request write access to these HealthKit types together with the read access
 requested by the task view controller by calling `requestedHealthKitTypesForReading`.
 
 See also: `requestedHealthKitTypesForReading`.
 */
@property (nonatomic, copy, readonly, nullable) NSSet<HKObjectType *> *requestedHealthKitTypesForWriting;
#endif
/**
 The set of permissions requested by the task.
 
 By default in `ORKOrderedTask` object, these permissions are collected from the
 recorder configurations associated with the active steps in the task.
 */
@property (nonatomic, readonly) ORKPermissionMask requestedPermissions;

/**
 A Boolean value indicating whether this task involves spoken audio prompts. (read-only)
 
 If the value of this property is `YES`, the shared `AVAudioSession` is configured for playback in the background.
 The audio `UIBackgroundMode` value must be set in the application's `Info.plist` file
 for this to be effective.
 
 By default, this property looks for active steps that have
 audio prompts or count down enabled, and returns `YES` if such steps exist in
 the task.
 */
@property (nonatomic, readonly) BOOL providesBackgroundAudioPrompts;

@end

NS_ASSUME_NONNULL_END
