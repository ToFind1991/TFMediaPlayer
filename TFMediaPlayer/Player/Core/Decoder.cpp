//
//  Decoder.cpp
//  TFMediaPlayer
//
//  Created by shiwei on 17/12/28.
//  Copyright © 2017年 shiwei. All rights reserved.
//

#include "Decoder.hpp"
#include "TFMPDebugFuncs.h"
#include "TFMPUtilities.h"

using namespace tfmpcore;

inline void freePacket(AVPacket **pkt){
    av_packet_free(pkt);
}

inline void freeFrame(AVFrame **frame){
    av_frame_free(frame);
}

bool Decoder::prepareDecode(){
    AVCodec *codec = avcodec_find_decoder(fmtCtx->streams[steamIndex]->codecpar->codec_id);
    if (codec == nullptr) {
        printf("find codec type: %d error\n",type);
        return false;
    }
    
    codecCtx = avcodec_alloc_context3(codec);
    if (codecCtx == nullptr) {
        printf("alloc codecContext type: %d error\n",type);
        return false;
    }
    
    avcodec_parameters_to_context(codecCtx, fmtCtx->streams[steamIndex]->codecpar);
    
    int retval = avcodec_open2(codecCtx, codec, NULL);
    if (retval < 0) {
        printf("avcodec_open2 id: %d error\n",codec->id);
        return false;
    }
    
#if DEBUG
    if (type == AVMEDIA_TYPE_AUDIO) {
        strcpy(frameBuffer.name, "audio_frame");
        strcpy(pktBuffer.name, "audio_packet");
    }else if (type == AVMEDIA_TYPE_VIDEO){
        strcpy(frameBuffer.name, "video_frame");
        strcpy(pktBuffer.name, "video_packet");
    }else{
        strcpy(frameBuffer.name, "subtitle_frame");
        strcpy(pktBuffer.name, "subtitle_packet");
    }
#endif
    
    shouldDecode = true;
    
    pktBuffer.valueFreeFunc = freePacket;
    frameBuffer.valueFreeFunc = freeFrame;
    
    RecycleBuffer<TFMPMediaType> intBuffer;
    TFMPMediaType a;
    intBuffer.getOut(&a);
    
    return true;
}

void Decoder::startDecode(){
    pthread_create(&decodeThread, NULL, decodeLoop, this);
    pthread_detach(decodeThread);
}

void Decoder::stopDecode(){
    shouldDecode = false;
}

void Decoder::freeResources(){
    
    if (!shouldDecode) shouldDecode = false;
    
    frameBuffer.signalAllBlock();
    pktBuffer.signalAllBlock();
    while (isDecoding) {
        av_usleep(10000); //0.01s
        TFMPDLOG_C("wait one decode loop end.[%d]\n",steamIndex);
    }
    frameBuffer.clear();
    pktBuffer.clear();
    
    if (codecCtx) avcodec_free_context(&codecCtx);
    fmtCtx = nullptr;
}

void Decoder::decodePacket(AVPacket *packet){
    
    AVPacket *refPacket = av_packet_alloc();
    int retval = av_packet_ref(refPacket, packet);
    TFCheckRetval("av_packet_ref")
    
    pktBuffer.blockInsert(refPacket);
}

void *Decoder::decodeLoop(void *context){
    
    Decoder *decoder = (Decoder *)context;
    
    AVPacket *pkt = nullptr;
    AVFrame *frame = nullptr;
    
    while (decoder->shouldDecode) {
        
        decoder->isDecoding = true;
        
        decoder->pktBuffer.blockGetOut(&pkt);
        
        int retval = avcodec_send_packet(decoder->codecCtx, pkt);
        if (retval < 0) {
            TFCheckRetval("avcodec send packet");
            continue;
        }
        
        if (decoder->type == AVMEDIA_TYPE_AUDIO) { //may many frames in one packet.
            
            while (retval == 0) {
                
                frame = av_frame_alloc();
                retval = avcodec_receive_frame(decoder->codecCtx, frame);
                if (retval == AVERROR(EAGAIN)) {
                    break;
                }
                
                if (retval != 0 && retval != AVERROR_EOF) {
                    TFCheckRetval("avcodec receive frame");
                    continue;
                }
                if (frame->extended_data == nullptr) {
                    printf("audio frame data is null\n");
                    continue;
                }
                
                if (decoder->shouldDecode) decoder->frameBuffer.blockInsert(frame);
            }
        }else{
            
            frame = av_frame_alloc();
            retval = avcodec_receive_frame(decoder->codecCtx, frame);
            if (decoder->shouldDecode) decoder->frameBuffer.blockInsert(frame);
            
            if (retval != 0) {
                TFCheckRetval("avcodec receive frame");
                continue;
            }
            
            if (frame->extended_data == nullptr) {
                printf("audio frame data is null\n");
                continue;
            }
        }
        av_packet_unref(pkt);
    }
    
    decoder->isDecoding = false;
    
    return 0;
}


