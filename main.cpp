#include <SDL.h>
#include <iostream>
#include <vector>


SDL_Window *window;
SDL_Surface *winSurface;
SDL_Surface *pacman_bitmap;

Uint32 band_color;

Uint32 road_color;


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

struct CellRoad {
    int y;
    int x;
    int w;
    int h;
};

std::vector<CellRoad> joinElements(std::vector<CellRoad> cellRoad);

std::vector<CellRoad> x_axis_Road(std::vector<CellRoad> cellRoad);

bool compareByHeight(const CellRoad &a, const CellRoad &b);

bool compareByWidth(const CellRoad &a, const CellRoad &b);

bool printBoard(SDL_Rect *rects, int count);

bool printRoad(SDL_Rect *rectsRoad, int count);

void generateBoard(SDL_Rect *rectsBoard);

void generateRoad(SDL_Rect *rectsBoard, SDL_Rect *rectsRoad, int length, std::vector<CellRoad> &cellsRoad);

void x_axis_Board(SDL_Rect *rectsBoardRight, SDL_Rect *rectsBoardLeft, int length);

bool load();

bool init();

void kill();

int main(int argc, char *args[]) {
    using namespace std;

    if (!init()) return 1;

    if (!load()) return 1;

    SDL_Rect pacman;
    pacman.x = 319/*WINDOW_WIDTH / 2*/;
    pacman.y = 380/*WINDOW_HEIGHT / 2 - BOARD_SCORE_HEIGHT + BAND_HEIGHT + BAND_HEIGHT / 2*/;
    pacman.w = 30;
    pacman.h = 30;

    SDL_Rect bounds_coordinatesRight[33];
    SDL_Rect bounds_coordinatesLeft[33];

    SDL_Rect road_coordinates[66];

    std::vector<CellRoad> road;

    band_color = SDL_MapRGB(winSurface->format, 0, 0, 255);
    road_color = SDL_MapRGB(winSurface->format, 210, 4, 45);

    generateBoard(bounds_coordinatesRight);
    x_axis_Board(bounds_coordinatesRight, bounds_coordinatesLeft, 33);
    generateRoad(bounds_coordinatesRight, road_coordinates, 33, road);

    road = joinElements(road);
    road = x_axis_Road(road);

    int count = 0;

    for (auto cell: road) {
        road_coordinates[count].x = cell.x;
        road_coordinates[count].y = cell.y;
        if (cell.w > 0) {
            road_coordinates[count].w = cell.w;
            road_coordinates[count].h = BAND_WIDTH;
        } else {
            road_coordinates[count].h = cell.h;
            road_coordinates[count].w = BAND_WIDTH;
        }
        count++;
    }

    int direction = 0, next_direction = 0;
    int state = 0;
    int next_way = 0;
    int key = 0, temp = 0, smooth = 0;

    SDL_Event event;
    int playing = 1;
    while (playing) {

        smooth = 0;
        temp = 0;
        key = 0;

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
                for (auto cellRoad: road) {
                    if (cellRoad.h > 0) {

                        if (direction != state) {
                            if (state == 3) {
                                smooth = pacman.x >= cellRoad.x ? pacman.x - cellRoad.x : 50;
                            } else if (state == 4) {
                                smooth = cellRoad.x >= pacman.x ? cellRoad.x - pacman.x : 50;
                            }

                            if (pacman.y <= cellRoad.y + cellRoad.h && pacman.y >= cellRoad.y && smooth < 20) {
                                next_way = cellRoad.x >= pacman.x ? pacman.x + smooth : pacman.x - smooth;
                                next_direction = direction;
                            }
                        }


                        for (int i = 0; i <= cellRoad.h; ++i) {
                            if (pacman.y == cellRoad.y + i && pacman.x == cellRoad.x) {
                                key = 1;
                                temp = cellRoad.y;
                            }
                        }
                    }
                }

                if (key && pacman.y > temp) {
                    pacman.y--;
                    state = direction;
                } else {
                    direction = state;
                }
                break;
            case 2:
                for (auto cellRoad: road) {
                    if (cellRoad.h > 0) {

                        if (direction != state) {
                            if (state == 3) {
                                smooth = pacman.x >= cellRoad.x ? pacman.x - cellRoad.x : 50;
                            } else if (state == 4) {
                                smooth = cellRoad.x >= pacman.x ? cellRoad.x - pacman.x : 50;
                            }

                            if (smooth < 20 && pacman.y >= cellRoad.y && pacman.y <= cellRoad.y + cellRoad.h) {
                                next_way = cellRoad.x >= pacman.x ? pacman.x + smooth : pacman.x - smooth;
                                next_direction = direction;
                            }
                        }

                        for (int i = 0; i <= cellRoad.h; ++i) {
                            if (pacman.y == cellRoad.y + i && pacman.x == cellRoad.x) {
                                key = 1;
                                temp = cellRoad.y + cellRoad.h;
                            }
                        }
                    }
                }

                if (key && pacman.y < temp) {
                    pacman.y++;
                    state = direction;
                } else {
                    direction = state;
                }
                break;
            case 3:
                for (auto cellRoad: road) {
                    if (cellRoad.w > 0) {

                        if (direction != state) {
                            if (state == 1) {
                                smooth = pacman.y >= cellRoad.y ? pacman.y - cellRoad.y : 50;
                            } else if (state == 2) {
                                smooth = pacman.y <= cellRoad.y ? cellRoad.y - pacman.y : 50;
                            }

                            if (smooth < 20 && pacman.x >= cellRoad.x && pacman.x <= cellRoad.x + cellRoad.w) {
                                next_way = cellRoad.y >= pacman.y ? pacman.y + smooth : pacman.y - smooth;
                                next_direction = direction;
                            }
                        }

                        for (int i = 0; i <= cellRoad.w; ++i) {
                            if (pacman.x == cellRoad.x + i && pacman.y == cellRoad.y) {
                                key = 1;
                                temp = cellRoad.x;
                            }
                        }
                    }
                }

                if (key && pacman.x > temp) {
                    pacman.x--;
                    state = direction;
                } else {
                    direction = state;
                }

                break;
            case 4:

                for (auto cellRoad: road) {
                    if (cellRoad.w > 0) {

                        if (direction != state) {
                            if (state == 1) {
                                smooth = pacman.y >= cellRoad.y ? pacman.y - cellRoad.y : 50;
                            } else if (state == 2) {
                                smooth = pacman.y <= cellRoad.y ? cellRoad.y - pacman.y : 50;
                            }

                            if (smooth < 20 && pacman.x >= cellRoad.x && pacman.x <= cellRoad.x + cellRoad.w) {
                                next_way = cellRoad.y >= pacman.y ? pacman.y + smooth : pacman.y - smooth;
                                next_direction = direction;
                            }
                        }

                        for (int i = 0; i <= cellRoad.w + 1; ++i) {
                            if (pacman.x == cellRoad.x + i && pacman.y == cellRoad.y) {
                                key = 1;
                                temp = cellRoad.x + cellRoad.w;
                            }
                        }
                    }
                }

                if (key && pacman.x < temp) {
                    pacman.x++;
                    state = direction;
                } else {
                    direction = state;
                }
                break;
        }

        if (pacman.x == next_way || pacman.y == next_way) {
            direction = next_direction;
        }


        SDL_Delay(10);
        SDL_BlitSurface(pacman_bitmap, NULL, winSurface, &pacman);
        if (!printBoard(bounds_coordinatesRight, 33)) return -1;
        if (!printBoard(bounds_coordinatesLeft, 33)) return -1;
        if (!printRoad(road_coordinates, 66)) return -1;
        SDL_UpdateWindowSurface(window);
    }
    system("pause");
    kill();
    return 0;
}

std::vector<CellRoad> joinElements(std::vector<CellRoad> cellRoad) {
    using namespace std;
    vector<CellRoad> sortedWidthRoad;
    vector<CellRoad> sortedHeightRoad;

    for (auto cell: cellRoad) {
        if (cell.w > 0) {
            sortedWidthRoad.push_back(cell);
        } else {
            sortedHeightRoad.push_back(cell);
        }
    }

    sort(sortedWidthRoad.begin(), sortedWidthRoad.end(), compareByHeight);
    sort(sortedHeightRoad.begin(), sortedHeightRoad.end(), compareByWidth);

    int foo = 0;

    for (auto &cellFirst: sortedWidthRoad) {
        for (auto &cellSecond: sortedWidthRoad) {
            if (cellFirst.y == cellSecond.y && cellFirst.x != cellSecond.x) {
                for (int i = 0; i < cellFirst.w + BAND_WIDTH; ++i) {
                    if (cellFirst.x + i == cellSecond.x) {
                        cellFirst.w += cellSecond.w;
                        sortedWidthRoad.erase(sortedWidthRoad.begin() + foo);
                    }
                }
            }
            foo++;
        }
        foo = 0;
    }


    foo = 0;

    for (auto &cellFirst: sortedHeightRoad) {
        for (auto &cellSecond: sortedHeightRoad) {
            if (cellFirst.x == cellSecond.x && cellFirst.y != cellSecond.y) {
                for (int i = 0; i < cellFirst.h + BAND_WIDTH; ++i) {
                    if (cellFirst.y + i == cellSecond.y) {
                        cellFirst.h += cellSecond.h;
                        sortedHeightRoad.erase(sortedHeightRoad.begin() + foo);
                    }
                }
            }
            foo++;
        }
        foo = 0;
    }

    int count = 0;

    foo = 0;

    for (auto &cellFirst: sortedWidthRoad) {
        for (auto &cellSecond: sortedHeightRoad) {
            int endWidth = cellFirst.x + cellFirst.w;
            int endHeight = cellSecond.y + cellSecond.h;
            int halfOfHeight = endHeight / 2;
            int difference = endWidth > cellSecond.x ? endWidth - cellSecond.x : cellSecond.x - endWidth;
            int difference1 = cellFirst.y > endHeight ? cellFirst.y - endHeight : endHeight - cellFirst.y;
            int difference2 = cellFirst.x > cellSecond.x ? cellFirst.x - cellSecond.x : cellSecond.x - cellFirst.x;
            int difference3 = cellFirst.y > cellSecond.y ? cellFirst.y - cellSecond.y : cellSecond.y - cellFirst.y;
            int difference4 = halfOfHeight > cellFirst.y ? halfOfHeight - cellFirst.y : cellFirst.y - halfOfHeight;

            if (difference < 10 && difference3 < 10) {
                cellSecond.x = cellFirst.x + cellFirst.w;
                cellSecond.y = cellFirst.y;
            } else if (difference1 < 10 && difference < 10) {
                cellSecond.x += difference;
                cellSecond.h -= difference1;
            } else if (difference2 < 10 && difference3 < 10) {
                cellFirst.y -= difference3;
                cellFirst.x -= difference2;
            } else if (difference1 < 10 && difference2 < 10) {
                cellSecond.h -= difference1;
            } else if (cellFirst.y >= cellSecond.y && cellFirst.y <= endHeight && difference2 < 20) {
                if (cellFirst.x < cellSecond.x) {
                    cellFirst.x = cellSecond.x;
                } else {
                    cellSecond.x = cellFirst.x;
                }
            }
        }
    }

    for (auto &cellFirst: sortedWidthRoad) {
        for (auto &cellSecond: sortedHeightRoad) {
            if (count == 4 && foo == 10) {
                int difference = cellFirst.x + cellFirst.w;
                cellFirst.w += cellSecond.x - difference;
            } else if (count == 7 && foo == 4) {
                cellFirst.x = cellSecond.x;
            } else if (count == 7 && foo == 10) {
                int difference = cellFirst.x + cellFirst.w;
                cellFirst.w -= difference - cellSecond.x;
            } else if (count == 6 && foo == 10) {
                cellFirst.x = cellSecond.x;
            } else if (count == 6 && foo == 12) {
                int difference = cellFirst.x + cellFirst.w;
                cellFirst.w += cellSecond.x - difference;
            } else if (count == 15 && foo == 7) {
                int difference = cellSecond.y + cellSecond.h;
                cellSecond.h -= difference - cellFirst.y;
            } else if (count == 12 && foo == 3) {
                int difference = cellFirst.x + cellFirst.w;
                cellFirst.w += cellSecond.x - difference;
            }
            ++foo;
        }
        foo = 0;
        ++count;
    }

    /*count = 0;

    for (auto cell: sortedWidthRoad) {
        cout << "----" << count << "----\n";
        cout << "X: " << cell.x << "\n";
        cout << "Y: " << cell.y << "\n";
        cout << "W: " << cell.w << "\n";
        count++;
    }

    count = 0;

    for (auto cell: sortedHeightRoad) {
        cout << "----" << count << "----\n";
        cout << "X: " << cell.x << "\n";
        cout << "Y: " << cell.y << "\n";
        cout << "H: " << cell.h << "\n";
        count++;
    }*/

    for (auto cell: sortedHeightRoad) {
        sortedWidthRoad.push_back(cell);
    }

    return sortedWidthRoad;
}

std::vector<CellRoad> x_axis_Road(std::vector<CellRoad> cellRoad) {
    using namespace std;

    vector<CellRoad> axisCellRoad;


    for (auto cell: cellRoad) {
        axisCellRoad.push_back(cell);
    }

    for (auto &cell: axisCellRoad) {
        if (cell.w != 0) {
            int endWidth = cell.x + cell.w;
            cell.x = WINDOW_WIDTH - endWidth;
        } else {
            cell.x = WINDOW_WIDTH - cell.x;
        }
    }

    int foo = 0;

    int count = 0;

    for (auto &cell1: axisCellRoad) {
        for (auto &cell: cellRoad) {
            if (cell1.w > 0 && cell.w > 0) {
                int width = cell1.x + cell1.w;
                int difference = width >= cell.x ? width - cell.x : cell.x - width;
                if (difference < 20 && cell1.y == cell.y) {
                    cell1.w = cell1.w + cell.w - difference;
                    cellRoad.erase(cellRoad.begin() + foo);
                }
            }
            foo++;
        }
        foo = 0;
    }

    for (auto cell: cellRoad) {
        axisCellRoad.push_back(cell);
    }


    return axisCellRoad;
}

bool compareByHeight(const CellRoad &a, const CellRoad &b) {
    return a.y < b.y || (a.y == b.y && a.x < b.x);
}

bool compareByWidth(const CellRoad &a, const CellRoad &b) {
    return a.x < b.x || (a.x == b.x && a.y < b.y);
}

bool printBoard(SDL_Rect *rects, int count) {
    using namespace std;
    if (SDL_FillRects(winSurface, rects, count, band_color) != 0) {
        cout << "surface board: " << SDL_GetError() << endl;
        return false;
    }
    return true;
}

bool printRoad(SDL_Rect *rectsRoad, int count) {
    using namespace std;
    if (SDL_FillRects(winSurface, rectsRoad, count, road_color) != 0) {
        cout << "surface road: " << SDL_GetError() << endl;
        return false;
    }
    return true;
}

void generateBoard(SDL_Rect *rects) {

    using namespace std;

    int count = 0;
    int previousY = 0;
    int previousX = 0;

    struct Cell endBoard[4][1] = {{BOARD_START + BAND_HEIGHT,    WINDOW_WIDTH - BAND_WIDTH},
                                  {8 * BAND_HEIGHT - BAND_WIDTH, WINDOW_WIDTH - BAND_WIDTH},
                                  {WINDOW_HEIGHT,                WINDOW_WIDTH / 2},
                                  {BOARD_START,                  WINDOW_WIDTH / 2}};

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

void generateRoad(SDL_Rect *rectsBoard, SDL_Rect *rectsRoad, int length, std::vector<CellRoad> &cellsRoad) {
    using namespace std;

    int count = 0;

    for (int i = 0; i < length; ++i) {
        *(rectsRoad + i) = *(rectsBoard + i);
    }

    for (int i = 0; i < length + 1; ++i) {
        if (rectsRoad[i].y == BOARD_SCORE_HEIGHT - BAND_WIDTH) {
            rectsRoad[i].y += BAND_HEIGHT / 2 + BAND_WIDTH;
            rectsRoad[i].w = BAND_HEIGHT + BAND_HEIGHT / 2 + 2 * BAND_WIDTH;
        } else if (rectsRoad[i].y == WINDOW_HEIGHT - BAND_WIDTH) {
            rectsRoad[i].y -= BAND_HEIGHT / 2;
            rectsRoad[i].w = BAND_HEIGHT / 2;
        } else if (rectsRoad[i].x == WINDOW_WIDTH - BAND_WIDTH) {
            if (i % 2 == 0) {
                rectsRoad[i].x -= 2 * BAND_HEIGHT + BAND_HEIGHT / 2 + BAND_WIDTH;
                rectsRoad[i].y -= BAND_HEIGHT / 2;
                rectsRoad[i].w = 5;
                rectsRoad[i].h = BAND_HEIGHT;
            } else {
                rectsRoad[i].y += rectsRoad[i].h - BAND_HEIGHT / 2 - 3 * BAND_WIDTH;
                rectsRoad[i].x = WINDOW_WIDTH / 2 + BAND_HEIGHT + BAND_HEIGHT / 2 + BAND_WIDTH;
                rectsRoad[i].w = 3 * BAND_HEIGHT + BAND_HEIGHT / 2 - 3 * BAND_WIDTH;
                rectsRoad[i].h = 5;
            }
        } else if (i >= 4 && i <= 8) {
            if (rectsRoad[i].w > BAND_WIDTH) {
                rectsRoad[i].y -= BAND_HEIGHT / 2;
                rectsRoad[i].x -= BAND_HEIGHT / 2;
                if (rectsRoad[i].w > BAND_HEIGHT) {
                    rectsRoad[i].w += BAND_HEIGHT / 2 + BAND_WIDTH;
                } else {
                    rectsRoad[i].w = 3 * BAND_HEIGHT + 2 * BAND_WIDTH;
                }
            } else {
                if (i % 2 == 0) {
                    rectsRoad[i].y -= BAND_HEIGHT / 2;
                    rectsRoad[i].x -= BAND_HEIGHT / 2;
                    rectsRoad[i].w = BAND_HEIGHT;
                    rectsRoad[i].h = 5;
                } else {
                    if (i == 5) {
                        rectsRoad[i].y -= BAND_HEIGHT / 2;
                        rectsRoad[i].x += BAND_HEIGHT / 2;
                        rectsRoad[i].h = 2 * BAND_HEIGHT;
                    } else {
                        rectsRoad[i].y += BAND_HEIGHT / 2;
                        rectsRoad[i].x -= BAND_HEIGHT / 2;
                        rectsRoad[i].h += BAND_HEIGHT;
                    }
                }
            }
        } else if (i >= 9 && i <= 22) {
            if (i <= 20) {
                if (i >= 9 && i <= 12) {
                    if (i < 11) {
                        if (i % 2 != 0) {
                            rectsRoad[i].x += BAND_HEIGHT / 2;
                            rectsRoad[i].y += BAND_HEIGHT / 2;
                            rectsRoad[i].h = BAND_HEIGHT;
                        } else {
                            //change size here
                            rectsRoad[i].y -= BAND_HEIGHT / 2;
                            rectsRoad[i].w = 2 * BAND_HEIGHT + 2 * BAND_WIDTH;
                        }
                    } else {
                        if (i % 2 != 0) {
                            rectsRoad[i].x += BAND_HEIGHT / 2;
                            rectsRoad[i].y += BAND_HEIGHT / 2;
                            rectsRoad[i].h = BAND_HEIGHT;
                        } else {
                            rectsRoad[i].y -= BAND_HEIGHT / 2;
                            rectsRoad[i].w = 2 * BAND_HEIGHT + 2 * BAND_WIDTH + BAND_HEIGHT / 2;
                        }
                    }
                } else if (i >= 15 && i <= 16) {
                    if (i % 2 != 0) {
                        rectsRoad[i].x += BAND_HEIGHT / 2;
                        rectsRoad[i].y -= BAND_HEIGHT / 2;
                    } else {
                        rectsRoad[i].y -= BAND_HEIGHT / 2;
                        rectsRoad[i].x += BAND_HEIGHT / 2;
                        rectsRoad[i].w += BAND_WIDTH;
                    }
                } else {
                    if (i % 2 != 0) {
                        rectsRoad[i].x += BAND_HEIGHT / 2;
                        rectsRoad[i].y -= BAND_HEIGHT / 2;
                        rectsRoad[i].h += BAND_HEIGHT - BAND_WIDTH;
                    } else {
                        rectsRoad[i].x -= BAND_HEIGHT / 2;
                        rectsRoad[i].y -= BAND_HEIGHT / 2;
                        rectsRoad[i].w = BAND_HEIGHT + BAND_WIDTH;
                    }
                }
            } else {
                if (i % 2 != 0) {
                    rectsRoad[i].y += BAND_HEIGHT + BAND_HEIGHT / 2;
                    rectsRoad[i].x += BAND_HEIGHT + BAND_HEIGHT / 2 + BAND_WIDTH;
                    rectsRoad[i].h = BAND_HEIGHT + BAND_WIDTH;
                } else {
                    rectsRoad[i].y += BAND_HEIGHT / 2;
                    rectsRoad[i].w = BAND_HEIGHT / 2 + BAND_WIDTH;
                }
            }

        } else if (i >= 23 && i <= 26) {
            if (i < 25) {
                if (i % 2 != 0) {
                    rectsRoad[i].y -= BAND_HEIGHT / 2;
                    rectsRoad[i].x -= BAND_HEIGHT / 2;
                    rectsRoad[i].w = 3 * BAND_HEIGHT + 2 * BAND_WIDTH;
                } else {
                    rectsRoad[i].y -= BAND_HEIGHT / 2;
                    rectsRoad[i].x += BAND_HEIGHT / 2;
                    rectsRoad[i].h = 2 * BAND_HEIGHT;

                    *(rectsRoad + length) = rectsRoad[i];
                    //add new track here
                }
            } else {
                if (i % 2 != 0) {
                    rectsRoad[i].x += BAND_HEIGHT / 2;
                    rectsRoad[i].y -= BAND_HEIGHT / 2;
                    rectsRoad[i].h = 3 * BAND_HEIGHT;
                } else {
                    rectsRoad[i].y -= BAND_HEIGHT / 2;
                    rectsRoad[i].x -= BAND_HEIGHT / 2 + BAND_HEIGHT / 2 + BAND_WIDTH;
                    rectsRoad[i].w += BAND_HEIGHT / 2 + BAND_WIDTH;
                }
            }
        } else if (i >= 27 && i <= 32) {
            if (i < 30) {
                if (rectsRoad[i].w > BAND_WIDTH) {
                    if (i % 2 == 0) {
                        rectsRoad[i].x += BAND_HEIGHT + BAND_HEIGHT / 2 + BAND_WIDTH;
                        rectsRoad[i].y -= BAND_HEIGHT / 2;
                    } else {
                        rectsRoad[i].y -= BAND_HEIGHT / 2;
                        rectsRoad[i].x -= BAND_HEIGHT / 2;
                        rectsRoad[i].w += BAND_HEIGHT;
                    }
                } else {
                    rectsRoad[i].y -= BAND_HEIGHT / 2;
                    rectsRoad[i].x += BAND_HEIGHT / 2;
                    rectsRoad[i].h = 2 * BAND_HEIGHT;
                }

            } else {
                if (rectsRoad[i].w > BAND_WIDTH) {
                    if (i % 2 == 0) {
                        rectsRoad[i].y -= BAND_HEIGHT / 2;
                        rectsRoad[i].x -= BAND_HEIGHT / 2;
                        rectsRoad[i].w += BAND_HEIGHT / 2;
                    } else {
                        rectsRoad[i].y += BAND_HEIGHT / 2;
                        rectsRoad[i].x += BAND_HEIGHT / 2;
                        rectsRoad[i].h = BAND_HEIGHT;
                        rectsRoad[i].w = BAND_WIDTH;
                    }
                } else {
                    rectsRoad[i].x -= 2 * BAND_HEIGHT - BAND_HEIGHT / 2 + 2 * BAND_WIDTH;
                    rectsRoad[i].y += BAND_HEIGHT / 2;
                    rectsRoad[i].h = 3 * BAND_HEIGHT;
                }
            }
        } else {
            rectsRoad[i].y += BAND_HEIGHT + 2 * (BAND_HEIGHT / 2);
            rectsRoad[i].x -= BAND_HEIGHT;
            rectsRoad[i].w = BAND_HEIGHT;
            rectsRoad[i].h = 5;
        }
    }


    for (int i = 0; i < length + 1; ++i) {
        CellRoad cellRoad;
        cellRoad.x = rectsRoad[i].x;
        cellRoad.y = rectsRoad[i].y;
        if (rectsRoad[i].w > BAND_WIDTH) {
            cellRoad.w = rectsRoad[i].w;
            cellRoad.h = 0;
        } else {
            cellRoad.h = rectsRoad[i].h;
            cellRoad.w = 0;
        }
        cellsRoad.push_back(cellRoad);
    }


}

void x_axis_Board(SDL_Rect *rectsBoardRight, SDL_Rect *rectsBoardLeft, int length) {

    for (int i = 0; i < length; ++i) {
        *(rectsBoardLeft + i) = *(rectsBoardRight + i);
    }

    for (int i = 0; i < length; ++i) {
        if (rectsBoardLeft[i].w > BAND_WIDTH) {
            int endWidth = rectsBoardLeft[i].x + rectsBoardLeft[i].w;
            rectsBoardLeft[i].x = WINDOW_WIDTH - endWidth;
        } else {
            rectsBoardLeft[i].x = WINDOW_WIDTH - (rectsBoardLeft[i].x + BAND_WIDTH);
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


