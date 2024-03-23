#include <SDL.h>
#include <iostream>
#include <vector>


const int WINDOW_WIDTH = 640; // *64
const int WINDOW_HEIGHT = 650; // *60

const int BOARD_SCORE_HEIGHT = 50;

const int BOARD_START = BOARD_SCORE_HEIGHT;

const int BAND_WIDTH = 5;

const int BAND_HEIGHT = 60;

struct Cell {
    int y;
    int x;
};

struct MainCharacter {
    int x;
    int y;
};

bool load();

bool init();

void kill();

void generateBoard(SDL_Rect *rects);

bool printBoard(SDL_Rect *rects, int count);


SDL_Window *window;
SDL_Surface *winSurface;
SDL_Surface *pacman_bitmap;

Uint32 band_color;

int main(int argc, char *args[]) {
    using namespace std;

    MainCharacter pacman;
    pacman.x = 320;
    pacman.y = 240;

    if (!init()) return 1;

    if (!load()) return 1;

    band_color = SDL_MapRGB(winSurface->format, 255, 255, 255);

    SDL_Rect dest;
    dest.x = pacman.x;
    dest.y = pacman.y;
    dest.w = 30;
    dest.h = 30;

    SDL_Rect bounds_coordinates[38];

    generateBoard(bounds_coordinates);

    int direction = 0;

    SDL_Event event;
    int playing = 1;
    while (playing) {
        SDL_FillRect(winSurface, NULL, 0x0000000);
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_w:
                            direction = 1;
                            break;
                        case SDLK_s:
                            direction = 2;
                            break;
                        case SDLK_a:
                            direction = 3;
                            break;
                        case SDLK_d:
                            direction = 4;
                            break;
                    }
                    break;
                case SDL_QUIT:
                    playing = false;
                    break;
            }
        }
        switch (direction) {
            case 1:
                if (dest.y > 0)
                    dest.y--;
                break;
            case 2:
                if (dest.y <= WINDOW_HEIGHT - 30)
                    dest.y++;
                break;
            case 3:
                if (dest.x > 0)
                    dest.x--;
                break;
            case 4:
                if (dest.x <= WINDOW_WIDTH - 30)
                    dest.x++;
                break;
        }
        SDL_Delay(10);
        SDL_BlitSurface(pacman_bitmap, NULL, winSurface, &dest);
        if (!printBoard(bounds_coordinates, 38)) return -1;
        SDL_UpdateWindowSurface(window);

    }
    system("pause");
    kill();
    return 0;
}

bool printBoard(SDL_Rect *rects, int count) {
    using namespace std;
    if (SDL_FillRects(winSurface, rects, count, band_color) != 0) {
        cout << "surface: " << SDL_GetError() << endl;
        return false;
    }
    return true;
}

void generateBoard(SDL_Rect *rects) {

    using namespace std;

    int count = 0;
    int previousY = 0;
    int previousX = 0;

    struct Cell endBoard[4][1] = {{BOARD_START + BAND_HEIGHT, WINDOW_WIDTH - BAND_WIDTH},
                                  {8 * BAND_HEIGHT - BAND_WIDTH,           WINDOW_WIDTH - BAND_WIDTH},
                                  {WINDOW_HEIGHT,             WINDOW_WIDTH / 2},
                                  {BOARD_START,               WINDOW_WIDTH / 2}};

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 1; ++j) {
            SDL_Rect band;
            switch (i) {
                case 0:
                    band.x = endBoard[i][j].x;
                    band.y = endBoard[i][j].y;
                    band.w = BAND_WIDTH;
                    band.h = 4 * BAND_HEIGHT + BAND_WIDTH;
                    break;
                case 1:
                    band.x = endBoard[i][j].x;
                    band.y = endBoard[i][j].y;
                    band.w = BAND_WIDTH;
                    band.h = 3 * BAND_HEIGHT + BAND_WIDTH;
                    break;
                default:
                    band.x = endBoard[i][j].x;
                    band.y = endBoard[i][j].y - BAND_WIDTH;
                    band.w = WINDOW_WIDTH / 2;
                    band.h = BAND_WIDTH;
                    break;
            }
            rects[count] = band;
            ++count;
        }
    }

    struct Cell cellsTwo[5][2] = {
            {0, 3, 0,  4},
            {1, 0, 2,  0},
            {2, 1, 2,  2},
            {0, 2, 1,  2},
            {9, 1, 10, 1}
    };


    for (int j = 0; j < 5; ++j) {
        for (int k = 0; k < 2; ++k) {

            if (k != 0) {
                SDL_Rect band;
                if (previousX < cellsTwo[j][k].x) {
                    band.x = previousX * 64 + WINDOW_WIDTH / 2;
                    band.y = previousY * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                    band.w = (j < 1) ? BAND_HEIGHT * 2 + BAND_WIDTH : BAND_HEIGHT;
                    band.h = BAND_WIDTH;
                } else if (previousY < cellsTwo[j][k].y) {
                    band.x = previousX * 64 + WINDOW_WIDTH / 2;
                    band.y = (j > 2) ? previousY * 60 + BOARD_SCORE_HEIGHT : previousY * 60 +
                                                                             BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                    band.w = BAND_WIDTH;
                    band.h = BAND_HEIGHT;
                }

                rects[count] = band;
                ++count;

            }
            previousY = cellsTwo[j][k].y;
            previousX = cellsTwo[j][k].x;
        }
    }

    struct Cell cellsThree[7][2] = {
            {7, 0, 7, 0},
            {7, 2, 7, 2},
            {7, 4, 8, 3},
            {5, 0, 6, 0},
            {3, 4, 4, 3},
            {2, 3, 3, 2},
            {0, 1, 0, 0},
    };


    for (int j = 0; j < 7; ++j) {
        for (int k = 0; k < 2; ++k) {
            SDL_Rect band;
            switch (k) {
                case 0:
                    band.x = cellsThree[j][k].x * 64 + WINDOW_WIDTH / 2;
                    band.y = cellsThree[j][k].y * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                    band.w = BAND_WIDTH;
                    band.h = BAND_HEIGHT + BAND_WIDTH;
                    break;
                case 1:
                    if (previousY < cellsThree[j][k].y && previousX == cellsThree[j][k].x) {
                        band.x = cellsThree[j][k].x * 64 + WINDOW_WIDTH / 2;
                        band.y = cellsThree[j][k].y * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                        band.w = BAND_HEIGHT;
                        band.h = BAND_WIDTH;
                    } else if (previousY < cellsThree[j][k].y && previousX != cellsThree[j][k].x) {
                        band.x = cellsThree[j][k].x * 64 + WINDOW_WIDTH / 2;
                        band.y = cellsThree[j][k].y * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                        band.w = BAND_HEIGHT + BAND_WIDTH;
                        band.h = BAND_WIDTH;
                    } else if (previousX > cellsThree[j][k].x) {
                        band.x = cellsThree[j][k].x * 64 + WINDOW_WIDTH / 2;
                        band.y = cellsThree[j][k].y * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                        band.w = BAND_HEIGHT + BAND_WIDTH;
                        band.h = BAND_WIDTH;
                    } else {
                        band.x = cellsThree[j][k].x * 64 + WINDOW_WIDTH / 2;
                        band.y = cellsThree[j][k].y * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                        band.w = BAND_HEIGHT;
                        band.h = BAND_WIDTH;
                    }
                    break;
            }

            rects[count] = band;
            ++count;

            previousY = cellsThree[j][k].y;
            previousX = cellsThree[j][k].x;
        }
    }


    struct Cell cellsFour[2][2] = {
            {1, 2, 1, 4},
            {4, 2, 5, 1},
    };

    for (int j = 0; j < 2; ++j) {
        for (int k = 0; k < 2; ++k) {
            SDL_Rect band;
            if (j < 1) {
                switch (k) {
                    case 0:
                        band.x = cellsFour[j][k].x * 64 + WINDOW_WIDTH / 2;
                        band.y = cellsFour[j][k].y * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                        band.w = BAND_HEIGHT * 2 + BAND_WIDTH * 2;
                        band.h = BAND_WIDTH;
                        break;
                    case 1:
                        band.x = cellsFour[j][k].x * 64 + WINDOW_WIDTH / 2;
                        band.y = cellsFour[j][k].y * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                        band.w = BAND_WIDTH;
                        band.h = BAND_HEIGHT;
                        break;
                }
            } else {
                switch (k) {
                    case 0:
                        band.x = cellsFour[j][k].x * 64 + WINDOW_WIDTH / 2;
                        band.y = cellsFour[j][k].y * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                        band.w = BAND_WIDTH;
                        band.h = BAND_HEIGHT * 2;
                        break;
                    case 1:
                        band.x = cellsFour[j][k].x * 64 + WINDOW_WIDTH / 2;
                        band.y = cellsFour[j][k].y * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                        band.w = BAND_HEIGHT + BAND_WIDTH;
                        band.h = BAND_WIDTH;
                        break;
                }
            }

            rects[count] = band;
            ++count;
        }
    }

    struct Cell trap[2][3] = {{3, 0, 4, 0, 3, 1},
                              {5, 3, 6, 3, 5, 3}};

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            SDL_Rect band;
            if (j > 1) {
                band.x = trap[i][j].x * 64 + WINDOW_WIDTH / 2;
                band.y = trap[i][j].y * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                band.w = BAND_WIDTH;
                band.h = BAND_HEIGHT + BAND_WIDTH;
            } else {
                switch (i) {
                    case 0:
                        band.x = (j == 1) ? trap[i][j].x * 64 + WINDOW_WIDTH / 2 : trap[i][j].x * 64 +
                                                                                   WINDOW_WIDTH / 2 + BAND_HEIGHT / 2;
                        band.y = trap[i][j].y * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                        band.w = (j == 1) ? BAND_HEIGHT + BAND_WIDTH : BAND_HEIGHT / 2 + BAND_WIDTH;
                        band.h = BAND_WIDTH;
                        break;
                    case 1:
                        band.x = trap[i][j].x * 64 + WINDOW_WIDTH / 2;
                        band.y = trap[i][j].y * 60 + BOARD_SCORE_HEIGHT + BAND_HEIGHT;
                        band.w = BAND_HEIGHT * 2 + BAND_WIDTH * 2;
                        band.h = BAND_WIDTH;
                        break;
                }
            }

            rects[count] = band;
            ++count;
        }
    }

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

    pacman_bitmap = SDL_ConvertSurface(temp1, winSurface->format, 0);

    SDL_FreeSurface(temp1);

    if (!pacman_bitmap) {
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

    window = SDL_CreateWindow("PacMan", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);

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
    SDL_FreeSurface(pacman_bitmap);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


