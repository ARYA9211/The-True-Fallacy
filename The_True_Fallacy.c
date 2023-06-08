#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define WINHEIGHT 720
#define WINWIDTH 1280
#define TARGET_FPS 80
#define FRAME_DELAY_MS (1000 / TARGET_FPS)  //frame delay in milliseconds

enum GAMESTATE
{
    MENU,
    GAMEPLAY
};

typedef struct sprite
{
    SDL_Rect *IDLEDEST;
    SDL_Rect *MOTIONSOURCE;
    SDL_Rect *FIGHTSOURCE;
    int currframe;
    int currfiframe;
    Uint32 frameTimer;
    float posX;
    float posY;
    float x;
    float y;
    float velocityX;
    float velocityY;
    bool isMoving;
    bool isAttacking;
    int health;
} sprite;

int main(int argc, char **argv)
{

    bool running = true;
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() < 0)
    {
        printf("SDL_ttf initialization failed: %s", TTF_GetError());
    }

    SDL_Window *win = SDL_CreateWindow("The True Fallacy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINWIDTH, WINHEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    enum GAMESTATE game = MENU;

label:
    SDL_Texture *scenes[5];
    SDL_Texture *tex1[5];
    tex1[0] = IMG_LoadTexture(ren, "res/jameen1.jpg"); // ground
    tex1[1] = IMG_LoadTexture(ren, "res/grass1.png");  // ground
    tex1[2] = IMG_LoadTexture(ren, "res/jameen1.jpg"); // ground
    tex1[3] = IMG_LoadTexture(ren, "res/jameen1.jpg"); // ground
    tex1[4] = IMG_LoadTexture(ren, "res/jameen1.jpg"); // ground

    scenes[0] = IMG_LoadTexture(ren, "res/dark.jpg");                 // background 1
    scenes[1] = IMG_LoadTexture(ren, "res/green.jpg");                // background 2
    scenes[2] = IMG_LoadTexture(ren, "res/dark1.jpg");                // background 3
    scenes[3] = IMG_LoadTexture(ren, "res/dark3.jpg");                // background 4
    scenes[4] = IMG_LoadTexture(ren, "res/dark4.jpg");                // background 5
    SDL_Texture *tex4 = IMG_LoadTexture(ren, "res/characters.png");   // spritesheet
    SDL_Texture *tt1 = IMG_LoadTexture(ren, "res/tt1.jpg");           // gameover1
    SDL_Texture *tt2 = IMG_LoadTexture(ren, "res/tt2.jpg");           // gameover2
    SDL_Texture *ls = IMG_LoadTexture(ren, "res/loadingscreen2.jpg"); // loadingscreen

    TTF_Font *font = TTF_OpenFont("fonts/HACKED.ttf", 100); // font

    SDL_Rect src1[5]; // source rectangles of grounds
    src1[0] = (SDL_Rect){0, 0, 871, 871};
    src1[1] = (SDL_Rect){0, 0, 641, 641};
    src1[2] = (SDL_Rect){0, 0, 871, 871};
    src1[3] = (SDL_Rect){0, 0, 871, 871};
    src1[4] = (SDL_Rect){0, 0, 871, 871};
    SDL_Rect src[5]; // source rectangles of backgrounds
    src[0] = (SDL_Rect){0, 0, 1920, 1080};
    src[1] = (SDL_Rect){0, 0, 3840, 2160};
    src[2] = (SDL_Rect){0, 0, 4772, 2664};
    src[3] = (SDL_Rect){0, 0, 5136, 2889};
    src[4] = (SDL_Rect){0, 0, 3895, 2191};
    SDL_Rect scenedst = {0, 0, 1280, 720}; // destination for background
    SDL_Rect dst[9];                       // destination for ground

    sprite player1;                        //object created

    player1.currframe = 0;
    player1.currfiframe = 0;
    player1.MOTIONSOURCE = (SDL_Rect *)malloc(5 * sizeof(SDL_Rect)); // number of running MOTIONSOURCE
    player1.IDLEDEST = (SDL_Rect *)malloc(sizeof(SDL_Rect)); // number of standing MOTIONSOURCE
    player1.FIGHTSOURCE = (SDL_Rect *)malloc(8 * sizeof(SDL_Rect));
    (player1.MOTIONSOURCE)[0] = (SDL_Rect){139, 0, 120, 137}; // running 1
    (player1.MOTIONSOURCE)[1] = (SDL_Rect){260, 0, 140, 126}; // running 2
    (player1.MOTIONSOURCE)[2] = (SDL_Rect){510, 0, 134, 132}; // running 3
    (player1.MOTIONSOURCE)[3] = (SDL_Rect){645, 0, 149, 127}; // running 4
    (player1.MOTIONSOURCE)[4] = (SDL_Rect){0, 0, 138, 131};   // standing

    (player1.FIGHTSOURCE)[0] = (SDL_Rect){0, 138, 189, 122}; // source rectangles for fighting
    (player1.FIGHTSOURCE)[1] = (SDL_Rect){190, 138, 194, 114};
    (player1.FIGHTSOURCE)[2] = (SDL_Rect){385, 138, 199, 104};
    (player1.FIGHTSOURCE)[3] = (SDL_Rect){585, 138, 197, 92};
    (player1.FIGHTSOURCE)[4] = (SDL_Rect){783, 138, 169, 107};
    (player1.FIGHTSOURCE)[5] = (SDL_Rect){953, 138, 148, 109};
    (player1.FIGHTSOURCE)[6] = (SDL_Rect){0, 261, 136, 113};
    (player1.FIGHTSOURCE)[7] = (SDL_Rect){704, 263, 243, 153};

    player1.frameTimer = 0;
    player1.posX = rand() % (WINWIDTH - player1.MOTIONSOURCE[4].w); // for random respawning of character
    player1.posY = WINHEIGHT - (player1.MOTIONSOURCE[0].h) - (1280 / 9);
    player1.velocityX = 0;
    player1.velocityY = 0;
    player1.isMoving = false;
    player1.isAttacking = false;
    player1.x = 0;
    player1.y = 0;
    player1.health = 1000;

    sprite player2;
    player2.currframe = 0;
    player2.currfiframe = 0;
    player2.MOTIONSOURCE = (SDL_Rect *)malloc(7 * sizeof(SDL_Rect));
    player2.IDLEDEST = (SDL_Rect *)malloc(sizeof(SDL_Rect));
    player2.FIGHTSOURCE = (SDL_Rect *)malloc(11 * sizeof(SDL_Rect));

    (player2.MOTIONSOURCE)[0] = (SDL_Rect){955, 261, 140, 151};
    (player2.MOTIONSOURCE)[1] = (SDL_Rect){955, 413, 130, 121};
    (player2.MOTIONSOURCE)[2] = (SDL_Rect){720, 420, 190, 98};
    (player2.MOTIONSOURCE)[3] = (SDL_Rect){720, 519, 190, 97};
    (player2.MOTIONSOURCE)[4] = (SDL_Rect){955, 261, 140, 151};
    (player2.MOTIONSOURCE)[5] = (SDL_Rect){955, 413, 130, 121};
    (player2.MOTIONSOURCE)[6] = (SDL_Rect){0, 413, 105, 167}; // standing

    (player2.FIGHTSOURCE)[0] = (SDL_Rect){221, 420, 98, 109};
    (player2.FIGHTSOURCE)[1] = (SDL_Rect){212, 617, 107, 105};
    (player2.FIGHTSOURCE)[2] = (SDL_Rect){321, 617, 107, 89};
    (player2.FIGHTSOURCE)[3] = (SDL_Rect){432, 617, 107, 89};
    (player2.FIGHTSOURCE)[4] = (SDL_Rect){540, 617, 121, 86};
    (player2.FIGHTSOURCE)[5] = (SDL_Rect){662, 617, 131, 84};
    (player2.FIGHTSOURCE)[6] = (SDL_Rect){794, 617, 125, 95};
    (player2.FIGHTSOURCE)[7] = (SDL_Rect){920, 617, 119, 96};
    (player2.FIGHTSOURCE)[8] = (SDL_Rect){0, 734, 115, 104};
    (player2.FIGHTSOURCE)[9] = (SDL_Rect){116, 734, 195, 126};
    (player2.FIGHTSOURCE)[10] = (SDL_Rect){313, 745, 260, 88};

    player2.frameTimer = 0;
    player2.posX = rand() % (WINWIDTH - (player2.MOTIONSOURCE[0].w));
    player2.posY = WINHEIGHT - (player2.MOTIONSOURCE[6].h) - (1280 / 9);
    player2.velocityX = 0;
    player2.velocityY = 0;
    player2.isMoving = false;
    player2.isAttacking = false;
    player2.x = 0;
    player2.y = 0;
    player2.health = 1000;
    int a = player2.currframe;
    SDL_Event evnt;
    Uint32 frameStartTime, frameEndTime, frameTime;

    SDL_Color textcolor = {255, 255, 255, 255}; // font color
    int scenenumber = rand() % 5;

    while (running)
    {

        frameStartTime = SDL_GetTicks();

        SDL_RenderCopy(ren, scenes[scenenumber], &src[scenenumber], &scenedst);

        for (int j = 0; j < 9; j++)
        {
            dst[j].x = j * (1280 / 9);
            dst[j].y = WINHEIGHT - (1280 / 9);
            dst[j].w = 1280 / 9;
            dst[j].h = 1280 / 9;
            SDL_RenderCopy(ren, tex1[scenenumber], &src1[scenenumber], &dst[j]);
        }
        char arr[102];
        if (player1.posX < player2.posX)
        {
            sprintf(arr, "    %d                                                                                         %d    ", player1.health, player2.health);
        }
        else
        {
            sprintf(arr, "    %d                                                                                         %d    ", player2.health, player1.health);
        }
        SDL_Surface *surface = TTF_RenderText_Blended(font, arr, textcolor);
        SDL_Texture *text = SDL_CreateTextureFromSurface(ren, surface);
        SDL_FreeSurface(surface);
        SDL_Rect txtdst = {0, 0, WINWIDTH, 50};
        SDL_RenderCopy(ren, text, NULL, &txtdst);
        while (SDL_PollEvent(&evnt))
        {

            if (evnt.type == SDL_QUIT)
            {
                running = false;
            }
            else if (evnt.type == SDL_KEYDOWN)
            {

                switch (evnt.key.keysym.sym)
                {
                case SDLK_RETURN:  //if enter key pressed
                    if (game == MENU)
                    {
                        game = GAMEPLAY;
                    }
                    break;

                case SDLK_ESCAPE:
                    if(game == GAMEPLAY)
                    {
                        game = MENU;
                    } 
                    break;   

                case SDLK_SPACE:
                    player1.velocityY = -20;
                    player1.isMoving = true;
                    break;
                case SDLK_x:
                    player1.isAttacking = true;
                    player1.isMoving = true;
                    break;
                case SDLK_a:
                    player1.velocityX = -5;
                    player1.isMoving = true;
                    break;
                case SDLK_d:
                    player1.velocityX = 5;
                    player1.isMoving = true;
                    break;

                case SDLK_KP_0:
                    player2.velocityY = -20;
                    player2.isMoving = true;
                    break;
                case SDLK_KP_ENTER:
                    player2.isAttacking = true;
                    player2.isMoving = true;
                    break;
                case SDLK_KP_4:
                    player2.velocityX = -5;
                    player2.isMoving = true;
                    a = 3;
                    break;
                case SDLK_KP_6:
                    player2.velocityX = 5;
                    player2.isMoving = true;
                    a = 3;
                    break;
                }
            }
            else if (evnt.type == SDL_KEYUP)
            {

                switch (evnt.key.keysym.sym)
                {
                case SDLK_SPACE:
                    player1.velocityY = 7;
                    if (player1.posY <= WINHEIGHT - (player1.MOTIONSOURCE[0].h) - (1280 / 9) || player1.posY <= WINHEIGHT - (player1.MOTIONSOURCE[1].h) - (1280 / 9))
                    {
                        player1.isMoving = false;
                    }
                    break;
                case SDLK_x:
                    player1.isAttacking = false;
                    player1.velocityX = 0;
                    player1.isMoving = false;

                    player1.currfiframe = 0;
                    break;
                case SDLK_a:
                    player1.isMoving = false;
                    player1.isAttacking = false;
                    break;
                case SDLK_d:
                    player1.isMoving = false;
                    player1.isAttacking = false;
                    break;

                case SDLK_KP_0:
                    player2.velocityY = 7;
                    if (player2.posY <= WINHEIGHT - (player2.MOTIONSOURCE[6].h) - (1280 / 9))
                    {
                        player2.isMoving = false;
                    }
                    break;
                case SDLK_KP_ENTER:
                    player2.isAttacking = false;
                    player2.velocityX = 0;
                    player2.isMoving = false;

                    player2.currfiframe = 0;
                    break;
                case SDLK_KP_4:
                    player2.isMoving = false;
                    a = player2.currframe;
                    player2.isAttacking = false;
                    break;
                case SDLK_KP_6:
                    player2.isMoving = false;
                    a = player2.currframe;
                    player2.isAttacking = false;
                    break;
                }
            }
        }
        if (game == MENU)
        {
            SDL_RenderCopy(ren, ls, &scenedst, &scenedst);
        }
        else if (game == GAMEPLAY)
        {
            if (player1.posX <= player2.posX + player2.MOTIONSOURCE[6].w && player1.posX + player1.MOTIONSOURCE[1].w >= player2.posX && player1.posY <= player2.posY + player2.MOTIONSOURCE[player2.currframe].h && player1.posY + player1.MOTIONSOURCE[player1.currframe].h >= player2.posY)
            {
                if (player1.posX < player2.posX)
                {
                    player1.posX = player2.posX - player1.MOTIONSOURCE[1].w;
                }
                else
                {
                    player2.posX = player1.posX - player2.MOTIONSOURCE[6].w;
                }
                if (player1.posY - player2.posY < player1.FIGHTSOURCE[player1.currfiframe].h || player2.posY - player1.posY < player2.FIGHTSOURCE[player2.currfiframe].h)
                {
                    if (player1.isAttacking)
                    {
                        player2.health--;
                        if (player2.health < 100)
                        {
                            textcolor.b = 0;
                            textcolor.g = 0;
                        }
                        if (player2.health == 0)
                        {
                            SDL_RenderCopy(ren, tt2, &scenedst, &scenedst);
                            SDL_RenderPresent(ren);
                            SDL_Delay(5000);
                            goto label;
                        }
                    }
                    if (player2.isAttacking)
                    {
                        player1.health--;
                        if (player1.health < 100)
                        {
                            textcolor.b = 0;
                            textcolor.g = 0;
                        }
                        if (player1.health == 0)
                        {
                            SDL_RenderCopy(ren, tt1, &scenedst, &scenedst);
                            SDL_RenderPresent(ren);
                            SDL_Delay(5000);
                            goto label;
                        }
                    }
                }
            }
            if (player1.posY > WINHEIGHT - (player1.MOTIONSOURCE[0].h) - (1280 / 9))
            {
                player1.posY = WINHEIGHT - (player1.MOTIONSOURCE[0].h) - (1280 / 9);
            }
            else if (player1.posY < 25)
            {
                player1.posY = 25;
            }
            if (player1.posX < 10)
            {
                player1.posX = 10;
            }
            else if (player1.posX > WINWIDTH - (player1.MOTIONSOURCE[1].w))
            {
                player1.posX = WINWIDTH - (player1.MOTIONSOURCE[1].w);
            }
            if (player2.posY > WINHEIGHT - (player2.MOTIONSOURCE[6].h) - (1280 / 9))
            {
                player2.posY = WINHEIGHT - (player2.MOTIONSOURCE[6].h) - (1280 / 9);
            }
            else if (player2.posY < 25)
            {
                player2.posY = 25;
            }
            if (player2.posX < 10)
            {
                player2.posX = 10;
            }
            else if (player2.posX > WINWIDTH - (player2.MOTIONSOURCE[2].w))
            {
                player2.posX = WINWIDTH - (player2.MOTIONSOURCE[2].w);
            }

            if (!player1.isMoving && player1.posY >= WINHEIGHT - (player1.MOTIONSOURCE[0].h) - (1280 / 9)) // player1.MOTIONSOURCE[0].h maximum h isliye liya
            {
                (player1.IDLEDEST)[0] = (SDL_Rect){player1.posX, WINHEIGHT - 132 - (1280 / 9), 134, 132};
                SDL_Rect vibrateFrame = player1.IDLEDEST[0];

                if (player1.x <= player1.posX)
                {
                    SDL_RenderCopyEx(ren, tex4, &(player1.MOTIONSOURCE[4]), &vibrateFrame, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                }
                else
                {
                    SDL_RenderCopyEx(ren, tex4, &(player1.MOTIONSOURCE[4]), &vibrateFrame, 0.0, NULL, SDL_FLIP_NONE);
                }
            }
            else if (player1.isAttacking)
            {
                player1.x = player1.posX;
                player1.y = player1.posY;
                if (player1.velocityX != 0)
                {
                    player1.posX += player1.velocityX;
                }
                player1.posY += player1.velocityY;
                SDL_Rect player1attack = {player1.posX, player1.posY, player1.FIGHTSOURCE[player1.currfiframe].w, player1.FIGHTSOURCE[player1.currfiframe].h};
                if (player1.velocityX >= 0)
                {
                    SDL_RenderCopyEx(ren, tex4, &(player1.FIGHTSOURCE[player1.currfiframe]), &player1attack, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                }
                else
                {
                    SDL_RenderCopyEx(ren, tex4, &(player1.FIGHTSOURCE[player1.currfiframe]), &player1attack, 0.0, NULL, SDL_FLIP_NONE);
                }
            }
            else
            {
                player1.x = player1.posX;
                player1.y = player1.posY;

                player1.posX += player1.velocityX;
                player1.posY += player1.velocityY;

                SDL_Rect player1DstRect = {player1.posX, player1.posY, player1.MOTIONSOURCE[player1.currframe].w, player1.MOTIONSOURCE[player1.currframe].h};

                if (player1.velocityX >= 0)
                {

                    SDL_RenderCopyEx(ren, tex4, &(player1.MOTIONSOURCE[player1.currframe]), &player1DstRect, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                }
                else
                {
                    SDL_RenderCopyEx(ren, tex4, &(player1.MOTIONSOURCE[player1.currframe]), &player1DstRect, 0.0, NULL, SDL_FLIP_NONE);
                }
            }

            if (!player2.isMoving && player2.posY >= WINHEIGHT - (player2.MOTIONSOURCE[6].h) - (1280 / 9)) // player2.MOTIONSOURCE[6].h maximum h isliye liya
            {
                (player2.IDLEDEST)[0] = (SDL_Rect){player2.posX, WINHEIGHT - 167 - (1280 / 9), 105, 167};

                SDL_Rect vibFrame = player2.IDLEDEST[0];

                if (player2.x <= player2.posX)
                {
                    SDL_RenderCopyEx(ren, tex4, &(player2.MOTIONSOURCE[6]), &vibFrame, 0.0, NULL, SDL_FLIP_NONE);
                }
                else
                {
                    SDL_RenderCopyEx(ren, tex4, &(player2.MOTIONSOURCE[6]), &vibFrame, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                }
            }
            else if (player2.isAttacking)
            {
                player2.x = player2.posX;
                player2.y = player2.posY;
                if (player2.velocityX != 0)
                {
                    player2.posX += player2.velocityX;
                }
                player2.posY += player2.velocityY;
                SDL_Rect player2attack = {player2.posX, player2.posY + 50, player2.FIGHTSOURCE[player2.currfiframe].w, player2.FIGHTSOURCE[player2.currfiframe].h};
                if (player2.velocityX >= 0)
                {
                    SDL_RenderCopyEx(ren, tex4, &(player2.FIGHTSOURCE[player2.currfiframe]), &player2attack, 0.0, NULL, SDL_FLIP_NONE);
                }
                else
                {
                    SDL_RenderCopyEx(ren, tex4, &(player2.FIGHTSOURCE[player2.currfiframe]), &player2attack, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                }
            }
            else
            {
                player2.x = player2.posX;
                player2.y = player2.posY;

                player2.posX += player2.velocityX;
                player2.posY += player2.velocityY;

                SDL_Rect player2DstRect = {player2.posX, player2.posY, player2.MOTIONSOURCE[a].w, player2.MOTIONSOURCE[a].h};

                if (player2.velocityX >= 0)
                {

                    SDL_RenderCopyEx(ren, tex4, &(player2.MOTIONSOURCE[a]), &player2DstRect, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                }
                else
                {
                    SDL_RenderCopyEx(ren, tex4, &(player2.MOTIONSOURCE[a]), &player2DstRect, 0.0, NULL, SDL_FLIP_NONE);
                }
            }
        }

        SDL_RenderPresent(ren);

        frameEndTime = SDL_GetTicks();
        frameTime = frameEndTime - frameStartTime;

        if (frameTime < FRAME_DELAY_MS)
        {
            SDL_Delay(FRAME_DELAY_MS - frameTime);
        }

        if (player1.isMoving)
        {
            player1.frameTimer += frameTime;
            if (player1.frameTimer >= 8 * FRAME_DELAY_MS) ///8 for plugged in 3 for on battery
            {
                player1.currframe = (player1.currframe + 1) % 4;

                player1.frameTimer -= 8 * FRAME_DELAY_MS;
            }
        }
        if (player1.isAttacking)
        {
            player1.frameTimer += frameTime;
            if (player1.frameTimer >= FRAME_DELAY_MS / 1.1)
            {
                player1.currfiframe = (player1.currfiframe + 1) % 8;
                player1.frameTimer -= FRAME_DELAY_MS / 1.1;
            }
        }

        if (player2.isMoving)
        {
            player2.frameTimer += frameTime;
            if (player2.frameTimer >= FRAME_DELAY_MS)
            {
                player2.currframe = (player2.currframe + 1) % 6;

                player2.frameTimer -= FRAME_DELAY_MS;
            }
        }
        if (player2.isAttacking)
        {
            player2.frameTimer += frameTime;
            if (player2.frameTimer >= FRAME_DELAY_MS / 1.5)
            {
                player2.currfiframe = (player2.currfiframe + 1) % 11;
                player2.frameTimer -= FRAME_DELAY_MS / 1.5;
            }
        }
        SDL_RenderClear(ren);
    }

    for (int j = 0; j < 5; j++)
    {
        SDL_DestroyTexture(tex1[j]);
        SDL_DestroyTexture(scenes[j]);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    free(player1.MOTIONSOURCE);
    free(player1.IDLEDEST);
    free(player1.FIGHTSOURCE);
    free(player2.MOTIONSOURCE);
    free(player2.IDLEDEST);
    free(player2.FIGHTSOURCE);
    return 0;
}