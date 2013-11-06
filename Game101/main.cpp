//
//  main.cpp
//  Game101
//
//  Created by Aamir Jawaid on 2013-02-17.
//  Copyright (c) 2013 Aamir Jawaid. All rights reserved.
//
//Things yet to do
//Level builder
//Use array files to build level
//Bricks for added difficulty
//Split the Formation box into more partrs, so it's easier for collision detection.
//Enemy Ship
//Collision Detection

#define FRAMES_PER_SECOND 100
#define SCREENWIDTH 640
#define SCREENHEIGHT 480
#define ENEMYWIDTH 40
#define ENEMYHEIGHT 25

#include "SDL/SDL.h"
#include "SDL_image/SDL_image.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "Timer.h"
#include <stdlib.h>

using namespace std;

SDL_Surface *screen = NULL;
SDL_Event event;
SDL_Surface *pSprite = NULL;
int frame = 0;
bool update_fps = false;
Timer fps;

bool init()
{
    //Start SDL
    if (SDL_Init( SDL_INIT_EVERYTHING) == -1)
        return false;
    
    //Set up screen using video
    screen = SDL_SetVideoMode(SCREENWIDTH, SCREENHEIGHT, 32, SDL_SWSURFACE);
    if (screen == NULL)
        return false;
    SDL_WM_SetCaption("Hello World", NULL);
    return true;
}

SDL_Surface *load_image(string filename)
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;
    loadedImage = IMG_Load(filename.c_str());
    if (loadedImage != NULL)
    {
        optimizedImage = SDL_DisplayFormat(loadedImage);
        SDL_FreeSurface(loadedImage);
    }
    return optimizedImage;
}

void apply_surface(SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL, SDL_Rect* offset = NULL)
{
    SDL_BlitSurface(source, clip, destination, offset);
}


struct Sprite
{
    SDL_Surface *sprite;
    SDL_Rect clip;
    SDL_Rect position;
    int xVel, yVel;
    Sprite() { xVel = 0, yVel = 0; sprite = pSprite; }
    void remove() { if (sprite != NULL) SDL_FreeSurface(sprite); }
    void move();
    void show();
    ~Sprite() {}
};

void Sprite::move()
{
    position.x += xVel;
    if (position.x < 0 || position.x > (SCREENWIDTH - clip.w))
        position.x -= xVel;
    position.y -= yVel;
    if (position.y < 0 || position.y > (SCREENHEIGHT - clip.h))
        position.y -= yVel;
}

void Sprite::show()
{
    apply_surface(sprite, screen, &clip, &position);
}

struct Plane : Sprite
{
    Plane() { destroyed = false; clip.x = 192; clip.y = 34; clip.w = 14; clip.h = 16; position.x = 30; position.y = 430; position.w = 14; position.h = 16; speed = 15; }
    bool destroyed;
    int speed;
    void handleInput();
    void show();
};

void Plane::show()
{
        apply_surface(sprite, screen, &clip, &position);
}

void Plane::handleInput()
{
    if (event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.sym)
        {
            case (SDLK_RIGHT) : xVel += speed; break;
            case (SDLK_LEFT) : xVel -= speed; break;
            default: break;
        }
    }
    if (event.type == SDL_KEYUP)
    {
        switch(event.key.keysym.sym)
        {
            case (SDLK_RIGHT) : xVel -= speed; break;
            case (SDLK_LEFT) : xVel += speed; break;
            default: break;
        }
    }
}
struct EnemyFormation;

struct Explosion : Sprite
{
    void show();
    bool exploded;
    Explosion() { exploded = false; clip.x = 350; clip.y = 29; clip.w = 27; clip.h = 16; }
};

void Explosion::show()
{
    apply_surface(sprite, screen, &clip, &position);
    exploded = true;
}


struct Enemy : Sprite
{
    bool destroyed;
    SDL_Rect animatedrect;
    bool toggleanimate;
    void show(int offset_x = 0, int offset_y = 0 );
    int collisionBlock;
    Enemy() { destroyed = false; position.w = clip.w; position.h = clip.h; }
};


void Enemy::show(int offset_x, int offset_y)
{ 
    SDL_Rect temp = position;
    temp.x += offset_x;
    temp.y += offset_y;
    if (!destroyed)
        apply_surface(sprite, screen, update_fps ? &clip : &animatedrect, &temp);
}

struct EnemyA : Enemy
{
    EnemyA() { position.x = 0;
           position.y = 0;
           clip.x = 6;
           clip.y = 9;
           clip.w = 28;
           clip.h = 16;
           animatedrect.x = 46;
           animatedrect.y = 9;
           animatedrect.w = 28,
           animatedrect.h = 16;
    }
};

struct EnemyB : Enemy
{
    EnemyB() {     position.x = 0;
        position.y = 0;
        clip.x = 85;
        clip.y = 12;
        clip.w = 28;
        clip.h = 16;
        animatedrect.x = 125;
        animatedrect.y = 11;
        animatedrect.w = 28,
        animatedrect.h = 16;
    }
};

struct EnemyC : Enemy
{
    EnemyC() { position.x = 0;
        position.y = 0;
        clip.x = 82;
        clip.y = 38;
        clip.w = 28;
        clip.h = 16;
        animatedrect.x = 124;
        animatedrect.y = 38;
        animatedrect.w = 28,
        animatedrect.h = 16;
    }
};

struct EnemyFormation
{
    vector<Enemy> formation;
    SDL_Rect bounds;
    bool reverse;
    void move();
    void show();
    bool loadFormation(string filename);
    int speed;
    EnemyFormation() { reverse = false; bounds.x = 0; bounds.y = 0; speed = 8; numEnemies = 0; }
    int returnBlock(int positionx);
    void effective_position(SDL_Rect* position, Enemy* _enemy);
    int numEnemies;
};

int EnemyFormation::returnBlock(int positionx)
{
    if (positionx < (bounds.x + bounds.w/3))
        return 1;
    else if (positionx >= (bounds.x + bounds.w/3) && positionx < (bounds.x + 2*bounds.w/3))
        return 2;
    else
        return 3;
}
void EnemyFormation::move()
{
        if (!reverse)
        {
            bounds.x += speed;
            if ((bounds.x + bounds.w + speed) >= SCREENWIDTH)
            {
                reverse = true;
                if ((bounds.y + bounds.h + speed) <= SCREENHEIGHT)
                    bounds.y += 30;
                else
                    bounds.y = 20;
            }
        }
        else
        {
            bounds.x -= speed;
            if (bounds.x - speed <= 0 )
            {
                reverse = false;
                if ((bounds.y + bounds.h + speed) <= SCREENHEIGHT)
                    bounds.y += 30;
                else
                    bounds.y = 20;
            }
        }
}

void EnemyFormation::show()
{
    vector<Enemy>::iterator front = formation.begin();
    while(front != formation.end())
    {
        front->show(bounds.x, bounds.y);
        front++;
    }
}
void EnemyFormation::effective_position(SDL_Rect *position, Enemy* _enemy)
{
    *position = _enemy->position;
    position->x += bounds.x;
    position->y += bounds.y;
    position->w = _enemy->clip.w;
    position->h = _enemy->clip.h;
}
bool EnemyFormation::loadFormation(string filename)
{
    bounds.w = 0;
    bounds.h = 0;
    ifstream form_file(filename);
    string line;
    SDL_Rect pos;
    pos.y = 0;
    if (!form_file.is_open())
        return false;
    while (form_file.good())
    {
        getline(form_file, line);
        const char* add = &line.c_str()[0];
        pos.x = 0 - ENEMYWIDTH;
        while (*add != '\0')
        {
            pos.x += ENEMYWIDTH;
            switch(*add)
            {
                case ('a') :
                {
                    EnemyA addAEnemy;
                    addAEnemy.position = pos;
                    formation.push_back(addAEnemy);
                    numEnemies++;
                    break;
                }
                case ('b') :
                {
                    EnemyB addBEnemy;
                    addBEnemy.position = pos;
                    formation.push_back(addBEnemy);
                    numEnemies++;
                    break;
                }
                case ('c') :
                {
                    EnemyC addCEnemy;
                    addCEnemy.position = pos;
                    formation.push_back(addCEnemy);
                    numEnemies++;
                    break;
                }
                default:
                    break;
            }
            add++;
        }
        pos.y += ENEMYHEIGHT;
        bounds.w = bounds.w < pos.x ? pos.x : bounds.w;
        bounds.h = bounds.h < pos.y ? pos.y : bounds.h;
    }
    form_file.close();
    //Divvy up all enemies in blocks
    for (vector<Enemy>::iterator i = formation.begin(); i != formation.end(); ++i)
    {
        i->collisionBlock = returnBlock(i->position.x);
    }
    return true;
}

struct Blast : Sprite
{
    Blast(Sprite* source) { yVel = 3; clip.x = 198; clip.y = 11; clip.w = 4; clip.h = 16; position.w = clip.w; position.h = clip.h; position.x = 0; position.y = source->position.y - source->clip.h; }
    Enemy* collisionDetect(EnemyFormation* Formation);
    bool RectIntersect(SDL_Rect* _enemy);
};
bool Blast::RectIntersect(SDL_Rect* _enemy)
{
    /*
     Rectangle 1′s bottom edge is higher than Rectangle 2′s top edge.
     Rectangle 1′s top edge is lower than Rectangle 2′s bottom edge.
     Rectangle 1′s left edge is to the right of Rectangle 2′s right edge.
     Rectangle 1′s right edge is to the left of Rectangle 2′s left edge.
     */
    //return !((rect1.y + rect1.h) < (rect2.y) || (rect1.y) >  (rect2.y + rect2.h) || (rect1.x) > (rect2.x + rect2.w) || (rect1.x + rect1.w) < rect2.x);
    /*
     Cast a ray - if that falls within the rect, then check the future y position
     */
    if (position.x >= _enemy->x && position.x <= (_enemy->x + _enemy->w))
    {
        if ((position.y - yVel) <= (_enemy->y + _enemy->h))
            return true;
    }
    return false;
}
Enemy* Blast::collisionDetect(EnemyFormation *Formation)
{
    //Do a check if blast is even in radius of the formation
    SDL_Rect _Formation = Formation->bounds;
    _Formation.w += Formation->speed;
    if (!RectIntersect(&_Formation))
        return NULL;
    //Okay so it's in the radius
    //find the collision block for it
    int colBlock = Formation->returnBlock(position.x);
    //Go through all the enemy ships->disregard any that aren't with the same collision block, then check if there's a rect
    vector<Enemy>::iterator i = Formation->formation.end();
    while (i != Formation->formation.begin())
    {
        --i;
        if (!i->destroyed && i->collisionBlock == colBlock)
        {
            SDL_Rect _enemy;
            Formation->effective_position(&_enemy, &(*i));
            if (RectIntersect(&_enemy))
            {
                return &(*i);
            }
        }
    }
    return NULL;
}

struct AntiBlast : Sprite
{
    int _animate;
    bool animate;
    SDL_Rect animatedrect;
    AntiBlast() { animate = false; _animate = 0; yVel = -3; clip.x = 168; clip.y = 35; clip.w = 4; clip.h = 16; animatedrect.x = 168; animatedrect.y = 14; animatedrect.w = 4; animatedrect.h = 16; }
    bool RectIntersect(SDL_Rect* _enemy);
    void show();
};

bool AntiBlast::RectIntersect(SDL_Rect* _enemy)
{
    /*
     Rectangle 1′s bottom edge is higher than Rectangle 2′s top edge.
     Rectangle 1′s top edge is lower than Rectangle 2′s bottom edge.
     Rectangle 1′s left edge is to the right of Rectangle 2′s right edge.
     Rectangle 1′s right edge is to the left of Rectangle 2′s left edge.
     */
    //return !((rect1.y + rect1.h) < (rect2.y) || (rect1.y) >  (rect2.y + rect2.h) || (rect1.x) > (rect2.x + rect2.w) || (rect1.x + rect1.w) < rect2.x);
    /*
     Cast a ray - if that falls within the rect, then check the future y position
     */
    if (position.x >= _enemy->x && position.x <= (_enemy->x + _enemy->w))
    {
        if ((position.y + yVel) >= (_enemy->y))
            return true;
    }
    return false;
}

void AntiBlast::show()
{
    if (_animate >= 1000/100)
    {
        _animate = 0;
        animate = ! animate;
    }
    _animate++;
    apply_surface(sprite, screen, animate ? &clip : &animatedrect, &position);
}
int main( int argc, char* args[] )
{ 
    bool quit = false;
    bool fired = false;
    vector<Blast> shots;
    srand (time(NULL));
    //The images
    //SDL_Surface *background = NULL;

    vector<Explosion> blasts;
    
    vector<AntiBlast> enemyShots;
    
    if (!init())
        return 1;
    pSprite = load_image("//Users//Aamir//Pictures//SpaceInvadersprites.png");
    //Ship
    Plane ship;

    
    //Enemy Killer 1
    Enemy killer1;

    
    //Enemy Killer 2
    Enemy killer2;

    
    //Enemy Killer 3
    Enemy killer3;

    
    EnemyFormation Theformation;
    Theformation.loadFormation("//Users//Aamir//Documents//workspace//Game101//Game101//formation1");
    int changeEnemy = 0;
    while (!quit && !ship.destroyed)
    {
        while( SDL_PollEvent( &event ) )
        {
            ship.handleInput();
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        changeEnemy++;
        //Blasts
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE && !fired)
        {
            Blast shot(&ship);
            shot.position.x = ship.position.x + 10;
            shots.push_back(shot);
            fired = true;
        }
        else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE)
            fired = false;
        //Things to Render
        SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );
        
        if (changeEnemy >= 50)
        {
            int EnemyNumb = rand() % Theformation.numEnemies;
            AntiBlast bl;
            int i = 0;
            vector<Enemy>::iterator attacker = Theformation.formation.begin();
            while(i <= EnemyNumb)
            {
                if (!attacker->destroyed)
                {
                    i++;
                }
                attacker++;
            }
            attacker--; 
            Theformation.effective_position(&bl.position, &(*attacker));
            enemyShots.push_back(bl);
            changeEnemy = 0;
        }
        
        if (!shots.empty())
        {
            vector<Blast>::iterator FirstElement = shots.begin();
            if (FirstElement->position.y <= 0)
                shots.erase(FirstElement);
            vector<Blast>::iterator i = shots.begin();
            while (i != shots.end())
            {
                Enemy* kill = i->collisionDetect(&Theformation);
                if (kill != NULL)
                {
                    kill->destroyed = true;
                    Theformation.numEnemies--;
                    Explosion _exp;
                    Theformation.effective_position(&_exp.position, &(*kill));
                    _exp.show();
                    blasts.push_back(_exp);
                    i = shots.erase(i);
                }
                else
                {
                    i->move();
                    i->show();
                    i++;
                }

            }
        }
        
        //All the antiblasts
        if (!enemyShots.empty())
        {
            vector<AntiBlast>::iterator FirstElement = enemyShots.begin();
            if (FirstElement->position.y >= SCREENHEIGHT)
                enemyShots.erase(FirstElement);
            vector<AntiBlast>::iterator j = enemyShots.begin();
            while (j != enemyShots.end())
            {
                if (j->RectIntersect(&ship.position))
                {
                    ship.destroyed = true;
                }
                else
                { 
                    j->move();
                    j->show();
                }
                j++;
                
            }
            
        }
        
        //Deal with collisions
        
        Theformation.show();
        ship.show();
        //killer1.show();
        //killer2.show();
        //killer3.show();
        if (SDL_Flip( screen ) == -1)
            return 1;
        frame++;
        if (frame >= 1000/FRAMES_PER_SECOND)
        {
            //Clean up blasts
            vector<Explosion>::iterator i = blasts.begin();
            while (i != blasts.end())
            {
                if (i->exploded)
                    i = blasts.erase(i);
                else
                    i++;
            }
            Theformation.move();
            ship.move();
            update_fps = !update_fps;
            frame = 0;
        }
    }
    //Cleanup
    ship.remove();    //SDL_FreeSurface(background);
    //Quit SDL
    SDL_Quit();
    
    return 0;
}