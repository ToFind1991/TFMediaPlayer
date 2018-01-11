//
//  DisplayController.hpp
//  TFMediaPlayer
//
//  Created by shiwei on 17/12/29.
//  Copyright © 2017年 shiwei. All rights reserved.
//

#ifndef DisplayController_hpp
#define DisplayController_hpp

#include <stdlib.h>
#include <pthread.h>
#include "TFMPAVFormat.h"
#include "RecycleBuffer.hpp"
#include "SyncClock.hpp"
#include "AudioResampler.hpp"

extern "C"{
#include <libavformat/avformat.h>
}

#define TFMP_MAX_AUDIO_CHANNEL 8

namespace tfmpcore {
    
    class DisplayController{
        
        pthread_t dispalyThread;
        static void *displayLoop(void *context);
        
        bool shouldDisplay;
        
        
        uint64_t remainingSize[TFMP_MAX_AUDIO_CHANNEL];
        uint8_t *remainingAudioBuffer[TFMP_MAX_AUDIO_CHANNEL];
//        AVFrame *remainFrame;
        
        static int fillAudioBuffer(uint8_t **buffer, int lineCount,int oneLineSize, void *context);
        
        AudioResampler *audioResampler = nullptr;
        
    public:
        
        TFMPMediaType displayMediaType = TFMP_MEDIA_TYPE_ALL_AVIABLE;
        
        //the real display func different with different platform
        void *displayContext;
        
        TFMPVideoFrameDisplayFunc displayVideoFrame;
        TFMPFillAudioBufferStruct getFillAudioBufferStruct();
        
        void setAudioResampler(AudioResampler *audioResampler){
            this->audioResampler = audioResampler;
        }
        
        RecycleBuffer<AVFrame*> *shareVideoBuffer;
        RecycleBuffer<AVFrame*> *shareAudioBuffer;
        
        AVRational videoTimeBase;
        AVRational audioTimeBase;
        
        SyncClock *syncClock;
        

        void startDisplay();
        void stopDisplay();
        
        void freeResource();
    };
}

#endif /* DisplayController_hpp */