#include <SDL.h>
#include <iostream>


bool load();

bool init();

void kill();


SDL_Window *window;
SDL_Surface *winSurface;
SDL_Surface *pacman;

int main(int argc, char *args[]) {
    using namespace std;

    if (!init()) return 1;

    if (!load()) return 1;


    SDL_Rect dest;
    dest.x = 160;
    dest.y = 120;

    SDL_Event event;
    int playing = 1;
    while (playing) {
        SDL_FillRect(winSurface, NULL, 0x0000000);
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_w:
                            dest.y -= 1;
                            break;
                        case SDLK_s:
                            dest.y += 1;
                            break;
                        case SDLK_a:
                            dest.x -= 1;
                            break;
                        case SDLK_d:
                            dest.x += 1;
                            break;
                    }
                    break;
                case SDL_QUIT:
                    playing = false;
                    break;
            }
        }
        SDL_Delay(100);
        SDL_BlitSurface(pacman, NULL, winSurface, &dest);
        SDL_UpdateWindowSurface(window);
    }
    system("pause");
    kill();
    return 0;
}

bool load() {
    using namespace std;
    SDL_Surface *temp1;

    temp1 = SDL_LoadBMP("/Users/dawid/ClionProjects/PacMan/Pacman.bmp");

    if (!temp1) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    pacman = SDL_ConvertSurface(temp1, winSurface->format, 0);

    SDL_FreeSurface(temp1);

    if (!pacman) {
        cout << "Error Converting surface" << SDL_GetError() << endl;
        system("pause");
        return false;
    }
    return true;
}

bool init() {
    using namespace std;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cout << "Error initializing SDL: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    window = SDL_CreateWindow("PacMan", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);

    if (!window) {
        cout << "Error creating window: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    winSurface = SDL_GetWindowSurface(window);

    if (!winSurface) {
        cout << "surface: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }


    return true;
}

void kill() {
    SDL_FreeSurface(pacman);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


