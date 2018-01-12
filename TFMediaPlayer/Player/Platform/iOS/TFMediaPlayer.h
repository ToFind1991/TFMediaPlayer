//
//  TFMediaPlayer.h
//  TFMediaPlayer
//
//  Created by shiwei on 17/12/25.
//  Copyright © 2017年 shiwei. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "TFAudioUnitPlayer.h"

typedef NS_ENUM(NSInteger, TFMediaPlayerState) {
    TFMediaPlayerStateNone,
    TFMediaPlayerStateConnecting,
    TFMediaPlayerStateReady,
    TFMediaPlayerStatePlaying,
    TFMediaPlayerStatePause,
};

@interface TFMediaPlayer : NSObject

@property (nonatomic, strong) NSURL *mediaURL;

@property (nonatomic, strong, readonly) UIView *displayView;

-(void)preparePlay;

-(void)play;
-(void)pause;
-(void)stop;

@property (nonatomic, assign) TFMPShareAudioBufferStruct shareAudioStruct;

@property (nonatomic, assign) TFMediaPlayerState state;

@property (nonatomic, assign) BOOL autoPlayWhenReady;

@end
