//
//  TFAudioFileReader.h
//  VideoGather
//
//  Created by shiwei on 17/11/2.
//  Copyright © 2017年 shiwei. All rights reserved.
//

//采用pull模式，不主动推送数据，而是外界需要的时候，调用这个类的方法拉取（pull）数据。
//输出数据格式固定

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "TFAudioBufferData.h"

@interface TFAudioFileReader : NSObject

@property (nonatomic, assign, readonly) BOOL readAviable;

@property (nonatomic, copy) NSString *filePath;

@property (nonatomic, assign) AudioStreamBasicDescription outputDesc;

@property (nonatomic, assign) BOOL isRepeat;

-(OSStatus)readFrames:(UInt32 *)framesNum toBufferData:(TFAudioBufferData *)bufferData;

@end
