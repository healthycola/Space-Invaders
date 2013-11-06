//
//  Timer.cpp
//  Game101
//
//  Created by Aamir Jawaid on 2013-02-25.
//  Copyright (c) 2013 Aamir Jawaid. All rights reserved.
//

#include "Timer.h"

int Timer::get_Ticks()
{
    if (started)
    {
        if (paused)
            return pausedTicks;
        else
            return SDL_GetTicks() - startTicks;
    }
    return 0;
}

void Timer::pause()
{
    if (started && !paused)
    {
        paused = true;
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
    if (paused)
    {
        paused = false;
        startTicks = get_Ticks() - pausedTicks;
        pausedTicks = 0;
    }
}