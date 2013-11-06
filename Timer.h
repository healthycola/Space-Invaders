//
//  Timer.h
//  Game101
//
//  Created by Aamir Jawaid on 2013-02-25.
//  Copyright (c) 2013 Aamir Jawaid. All rights reserved.
//

#ifndef __Game101__Timer__
#define __Game101__Timer__

#include <iostream>
#include "SDL/SDL.h"
class Timer
{
    int startTicks;
    int pausedTicks;
    bool paused;
    bool started;
public:
    Timer() {startTicks = 0; pausedTicks = 0; paused = false; started = false; }
    void start() {started = true; paused = false; startTicks = SDL_GetTicks(); }
    void stop() {started = false; paused = false; }
    void pause();
    void unpause();
    int get_Ticks();
    bool is_Started() { return started; }
    bool is_Paused() { return paused; }
};

#endif /* defined(__Game101__Timer__) */
