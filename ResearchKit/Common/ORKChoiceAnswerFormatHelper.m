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


#import "ORKChoiceAnswerFormatHelper.h"

#import "ORKAnswerFormat_Internal.h"
#import "ORKQuestionResult_Private.h"
#import "ORKResult_Private.h"

#import "ORKHelpers_Internal.h"


@implementation ORKChoiceAnswerFormatHelper {
    NSArray *_choices;
    BOOL _isValuePicker;
}

- (instancetype)initWithAnswerFormat:(ORKAnswerFormat *)answerFormat {
    self = [super init];
    if (self) {
        _choices = [answerFormat choices];
        _isValuePicker = [answerFormat isValuePicker];
    }
    return self;
}

- (NSUInteger)choiceCount {
    return _choices.count;
}

- (id<ORKAnswerOption>)answerOptionAtIndex:(NSUInteger)index {
    if (index >= _choices.count) {
        return nil;
    }
    
    return _choices[index];
}
#if TARGET_OS_IOS
- (ORKImageChoice *)imageChoiceAtIndex:(NSUInteger)index {
    id<ORKAnswerOption> option = [self answerOptionAtIndex:index];
    return option && [option isKindOfClass:[ORKImageChoice class]] ? (ORKImageChoice *) option : nil;
}

- (ORKColorChoice *)colorChoiceAtIndex:(NSUInteger)index {
    id<ORKAnswerOption> option = [self answerOptionAtIndex:index];
    return option && [option isKindOfClass:[ORKColorChoice class]] ? (ORKColorChoice *) option : nil;
}
#endif

- (ORKTextChoice *)textChoiceAtIndex:(NSUInteger)index {
    id<ORKAnswerOption> option = [self answerOptionAtIndex:index];
    return option && [option isKindOfClass:[ORKTextChoice class]] ? (ORKTextChoice *) option : nil;
}

- (id)answerForSelectedIndex:(NSUInteger)index {
    return [self answerForSelectedIndexes:@[ @(index) ]];
}

- (id)answerForSelectedIndexes:(NSArray *)indexes {
    NSMutableArray *array = [NSMutableArray new];
    
    for (NSNumber *indexNumber in indexes) {
        
        NSUInteger index = indexNumber.unsignedIntegerValue;
        
        if (index >= _choices.count) {
            continue;
        }
        
        id<ORKAnswerOption> choice = _choices[index];
#if TARGET_OS_IOS
        ORKTextChoiceOther *textChoiceOther;
        if ([choice isKindOfClass: [ORKTextChoiceOther class]]) {
            textChoiceOther = (ORKTextChoiceOther *)choice;
        }
        id value = textChoiceOther.textViewText ? : choice.value;
#else
        id value = choice.value;
#endif
        if (value == nil) {
            value = _isValuePicker ? @(index - 1) : @(index);
        }
        
        if (_isValuePicker && index == 0) {
            // Don't add to answer array if this index is the 1st value of a value picker
        } else {
            [array addObject:value];
        }
    }
    return array.count > 0 ? [array copy] : ORKNullAnswerValue();
}

- (NSNumber *)selectedIndexForAnswer:(nullable id)answer {
    NSArray *indexes = [self selectedIndexesForAnswer:answer];
    return indexes.count > 0 ? indexes.firstObject : nil;
}

- (NSArray *)selectedIndexesForAnswer:(nullable id)answer {
    // Works with boolean result
    if ([answer isKindOfClass:[NSNumber class]]) {
        answer = @[answer];
    }
    
    NSMutableArray *indexArray = [NSMutableArray new];
    
    if (answer != nil && answer != ORKNullAnswerValue() ) {
        
        if (![answer isKindOfClass:[ORKChoiceQuestionResult answerClass]]) {
            @throw [NSException exceptionWithName:@"Wrong answer type"
                                           reason:[NSString stringWithFormat:@"Expected answer type %@, but was given %@", [ORKChoiceQuestionResult answerClass], [answer class]]
                                         userInfo:nil];
        }
        
        for (id answerValue in (NSArray *)answer) {
            id<ORKAnswerOption> matchedChoice = nil;
            BOOL isTextChoiceOtherResult = [self _isTextChoiceOtherResult:answerValue choices:_choices];
            
            for ( id<ORKAnswerOption> choice in _choices) {
#if TARGET_OS_IOS
                if ([choice isKindOfClass:[ORKTextChoiceOther class]]) {
                    ORKTextChoiceOther *textChoiceOther = (ORKTextChoiceOther *)choice;
                    if ([textChoiceOther.textViewText isEqual:answerValue]) {
                        matchedChoice = choice;
                        break;
                    } else if (textChoiceOther.textViewInputOptional && textChoiceOther.textViewText.length <= 0 && [textChoiceOther.value isEqual:answerValue]) {
                        matchedChoice = choice;
                        break;
                    } else if (isTextChoiceOtherResult) {
                        textChoiceOther.textViewText = answerValue;
                        matchedChoice = choice;
                        break;
                    }
                    
                } else if ([choice.value isEqual:answerValue]) {
                    matchedChoice = choice;
                    break;
                }
#else
                if ([choice.value isEqual:answerValue]) {
                    matchedChoice = choice;
                    break;
                }
#endif
            }
            
            if (nil == matchedChoice) {
                
                if (![answerValue isKindOfClass:[NSNumber class]]) {
                    @throw [NSException exceptionWithName:@"No matching choice found"
                                                   reason:[NSString stringWithFormat:@"Provided choice of type %@ not found in available choices. Answer is %@ and choices are %@", [answerValue class], answer, _choices]
                                                 userInfo:nil];
                }
                
                if (_isValuePicker) {
                    matchedChoice = _choices[((NSNumber *)answerValue).unsignedIntegerValue + 1];
                } else {
                    matchedChoice = _choices[((NSNumber *)answerValue).unsignedIntegerValue];
                }
            }
            
            if (matchedChoice) {
                [indexArray addObject:@([_choices indexOfObject:matchedChoice])];
            }
        }
    }
    
    if (_isValuePicker && indexArray.count == 0) {
        // value picker should at least select the placeholder index
        [indexArray addObject:@(0)];
    }
    
    return [indexArray copy];
    
}

- (BOOL)_isTextChoiceOtherResult:(id)answerValue choices:(NSArray *)choices {
    if (answerValue == nil) {
        return NO;
    }
    
    for (id<ORKAnswerOption> choice in _choices) {
        if ([choice.value isEqual:answerValue]){
            return NO;
        }
    }
    
    return YES;
}

- (NSString *)stringForChoiceAnswer:(id)answer {
    NSMutableArray<NSString *> *answerStrings = [[NSMutableArray alloc] init];
    NSArray *indexes = [self selectedIndexesForAnswer:answer];
    for (NSNumber *index in indexes) {
        NSString *text = [[self answerOptionAtIndex:[index integerValue]] text];
        if (text != nil) {
            [answerStrings addObject:text];
        }
    }
    return [answerStrings componentsJoinedByString:@"\n"];
}

- (NSString *)labelForChoiceAnswer:(id)answer {
    NSMutableArray<NSString *> *answerStrings = [[NSMutableArray alloc] init];
    NSArray *indexes = [self selectedIndexesForAnswer:answer];
    for (NSNumber *index in indexes) {
        NSString *text = [[self answerOptionAtIndex:[index integerValue]] text];
        if (text != nil) {
            [answerStrings addObject:text];
        }
    }
    return [answerStrings componentsJoinedByString:@", "];
}

@end
