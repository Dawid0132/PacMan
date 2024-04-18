#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>


SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *pacman_bitmap;
SDL_Texture *yellow_ghost;
SDL_Texture *red_ghost;
SDL_Texture *pink_ghost;
SDL_Texture *blue_ghost;
SDL_Texture *point_bitmap;
SDL_Texture *score_message;
SDL_Texture *scoreCount;
TTF_Font *Sans;

const int WINDOW_WIDTH = 640; // *64

const int WINDOW_HEIGHT = 650; // *60

const int BOARD_SCORE_HEIGHT = 50;

const int BOARD_START = BOARD_SCORE_HEIGHT;

const int BAND_WIDTH = 5;

const int BAND_HEIGHT = 60;

enum coordinates {
    WIDTH, HEIGHT,
};

enum directions {
    UP, DOWN, LEFT, RIGHT, STOP
};

struct pacman {
    int y;
    int x;
    int currentNodeDestination;
};

struct ghost {
    int y;
    int x;
    directions d;
    int currentNodeSource;
};

enum ghost_colors {
    RED, YELLOW, PINK, BLUE
};

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

std::vector<CellRoad> generateRoad(SDL_Rect *rectsBoard, int length);

std::vector<Cell> createPoints(std::vector<CellRoad> cellRoad);

std::vector<CellRoad> createTrapRoad(SDL_Rect *rectsL, SDL_Rect *rectR, int length);

std::vector<CellRoad> joinGhostRoadWithPacManRoad(std::vector<CellRoad> rp, std::vector<CellRoad> rg);

/*CellRoad movingGhost(ghost ghost, std::vector<CellRoad> &ghostRoad);*/

bool addNewRoad(CellRoad &cellRoad, ghost &ghost, SDL_Rect &renderGhost);

bool compareByHeight(const CellRoad &a, const CellRoad &b);

bool compareByWidth(const CellRoad &a, const CellRoad &b);

bool compareByLength(const CellRoad &a, const CellRoad &b);

bool printBoard(SDL_Rect *rects, int count, Uint8 r, Uint8 g, Uint8 b);

void printPoint(const std::vector<Cell> &pointsCoordinates);

bool printScore(int TotalScore);

bool printGates(SDL_Rect gates, Uint8 r, Uint8 g, Uint8 b);

void generateBoard(SDL_Rect *rectsBoard);

void x_axis_Board(SDL_Rect *rectsBoardRight, SDL_Rect *rectsBoardLeft, int length);

int changeRender(int coordinate, coordinates c);

SDL_Rect setGhostsPosition(std::vector<CellRoad> &ghostRoad, ghost_colors c, ghost &ghostCoordinate);

pacman tunnelSwap(pacman &pacman);

void bfs(std::vector<std::vector<CellRoad>> &adjList, int startNode, std::vector<int> &par, std::vector<int> &dist,
         std::vector<CellRoad> &road);

std::vector<CellRoad> getShortestPath(std::vector<std::vector<CellRoad>> &adjList, int source, int destination,
                                      std::vector<CellRoad> &road);

std::vector<std::vector<CellRoad>> createGraphWithAllNeighbours(std::vector<CellRoad> &road);

void stayInTrap(ghost &ghost, ghost_colors ghostColors, std::vector<CellRoad> &ghostRoad, SDL_Rect &ghostRender);

void setSourceNodeGhost(ghost &ghostCoordinate, std::vector<CellRoad> &road);

void ghostMoving(std::vector<CellRoad> &ghostWay, ghost &ghostCoordinate, SDL_Rect &ghostRendering,
                 pacman &pacmanCoordinate, bool &addNewPath);

bool escapeFromTrap(ghost &ghostCoordinate, std::vector<CellRoad> &road, bool &escape, SDL_Rect &ghostRender);

bool load();

bool init();

void kill();

int main(int argc, char *args[]) {
    using namespace std;

    if (!init()) return 1;

    if (!load()) return 1;

    int TotalScore = 0;
    bool startGame = false;
    bool escape = false;
    bool addNewPath = false;

    pacman pacman = {};
    pacman.x = 319;
    pacman.y = 380;


    ghost red_ghost_coordinate = {};
    ghost yellow_ghost_coordinate = {};
    ghost pink_ghost_coordinate = {};
    ghost blue_ghost_coordinate = {};

    CellRoad red_ghost_currentRoad;
    red_ghost_currentRoad.w = 0;
    red_ghost_currentRoad.h = 0;


    SDL_Rect pacmanRender;
    pacmanRender.w = 30;
    pacmanRender.h = 30;


    SDL_Rect yellow_ghost_render;
    SDL_Rect red_ghost_render;
    SDL_Rect pink_ghost_render;
    SDL_Rect blue_ghost_render;


    SDL_Rect messageRender;
    messageRender.x = 0;
    messageRender.y = 0;
    messageRender.h = BOARD_SCORE_HEIGHT - BAND_WIDTH;
    messageRender.w = BAND_HEIGHT;


    SDL_Rect score_position;
    score_position.x = BAND_HEIGHT;
    score_position.y = 0;
    score_position.h = BOARD_SCORE_HEIGHT - BAND_WIDTH;
    score_position.w = BAND_HEIGHT / 2;


    SDL_Rect bounds_coordinatesRight[33];
    SDL_Rect bounds_coordinatesLeft[33];

    SDL_Rect ghost_coordinate[30];

    SDL_Rect trap_gates;
    trap_gates.x = 290;
    trap_gates.y = 290;
    trap_gates.w = 60;
    trap_gates.h = BAND_WIDTH / 2;

    std::vector<CellRoad> road;
    std::vector<CellRoad> ghostRoad;
    std::vector<Cell> pointsCoordinates;

    std::vector<std::vector<CellRoad>> neighboursGraph;


    std::vector<CellRoad> ghostWay;


    generateBoard(bounds_coordinatesRight);
    x_axis_Board(bounds_coordinatesRight, bounds_coordinatesLeft, 33);


    road = generateRoad(bounds_coordinatesRight, 33);
    road = joinElements(road);
    road = x_axis_Road(road);


    ghostRoad = createTrapRoad(bounds_coordinatesLeft, bounds_coordinatesRight, 33);
    ghostRoad = joinGhostRoadWithPacManRoad(road, ghostRoad);

    neighboursGraph = createGraphWithAllNeighbours(road);


    int count = 0;

    pointsCoordinates = createPoints(road);

    yellow_ghost_render = setGhostsPosition(ghostRoad, YELLOW, yellow_ghost_coordinate);
    pink_ghost_render = setGhostsPosition(ghostRoad, PINK, pink_ghost_coordinate);
    blue_ghost_render = setGhostsPosition(ghostRoad, BLUE, blue_ghost_coordinate);
    red_ghost_render = setGhostsPosition(ghostRoad, RED, red_ghost_coordinate);


    int direction = 0, next_direction = 0;
    int state = 0;
    int next_way = 0;
    int key = 0, temp = 0, smooth = 0;


    int previousNodePacMan = 0;

    int test = 0;


    SDL_Event event;
    int playing = 1;

    while (playing) {

        int currentNodePacMan = 0;
        previousNodePacMan = pacman.currentNodeDestination;
        smooth = 0;
        temp = 0;
        key = 0;


        if (direction != 0 && !startGame) {
            if (escapeFromTrap(red_ghost_coordinate, road, escape, red_ghost_render)) {
                ghostWay = getShortestPath(neighboursGraph, pacman.currentNodeDestination,
                                           red_ghost_coordinate.currentNodeSource,
                                           road);
                /*cout << "nodepacman : " << currentNodePacMan << "\n";
                for (auto gw: ghostWay) {
                    SDL_Rect rect;
                    rect.x = gw.x;
                    rect.y = gw.y;
                    if (gw.w > 0) {
                        rect.w = gw.w;
                        rect.h = BAND_WIDTH;
                    } else {
                        rect.w = BAND_WIDTH;
                        rect.h = gw.h;
                    }
                    ghost_coordinate[count] = rect;
                    cout << "X:" << gw.x << " , ";
                    cout << "Y:" << gw.y << " , ";
                    cout << "W:" << gw.w << " , ";
                    cout << "H:" << gw.h << "\n";
                    count++;
                }*/
                escape = !escape;
                startGame = !startGame;
            }
        }


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
                                pacman.currentNodeDestination = currentNodePacMan;
                                temp = cellRoad.y;
                            }
                        }
                    }
                    currentNodePacMan++;
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
                                pacman.currentNodeDestination = currentNodePacMan;
                                key = 1;
                                temp = cellRoad.y + cellRoad.h;
                            }
                        }
                    }
                    currentNodePacMan++;
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
                                pacman.currentNodeDestination = currentNodePacMan;
                                key = 1;
                                temp = cellRoad.x;
                            }
                        }
                    }
                    currentNodePacMan++;
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
                                pacman.currentNodeDestination = currentNodePacMan;
                                temp = cellRoad.x + cellRoad.w;
                            }
                        }
                    }
                    currentNodePacMan++;
                }

                if (key && pacman.x < temp) {
                    pacman.x++;
                    state = direction;
                } else {
                    direction = state;
                }
                break;
        }

        setSourceNodeGhost(red_ghost_coordinate, road);

        if (pacman.x == next_way || pacman.y == next_way) {
            direction = next_direction;
        }

        if (previousNodePacMan != pacman.currentNodeDestination && startGame && addNewPath) {
            /*if (test == 0) {*/
            ghostWay = getShortestPath(neighboursGraph, pacman.currentNodeDestination,
                                       red_ghost_coordinate.currentNodeSource,
                                       road);
            /*cout << "Pacman :" << "X :" << pacman.x << "Y :" << pacman.y << "\n";
            cout << "Road :" << "X :" << road.at(pacman.currentNodeDestination).x << "Y :"
                 << road.at(pacman.currentNodeDestination).y << "W :" << road.at(pacman.currentNodeDestination).w
                 << "H :" << road.at(pacman.currentNodeDestination).h << "\n";

            cout << "Ghost :" << "X :" << red_ghost_coordinate.x << "Y :" << red_ghost_coordinate.y << "\n";
            cout << "Road :" << "X :" << road.at(red_ghost_coordinate.currentNodeSource).x << "Y :"
                 << road.at(red_ghost_coordinate.currentNodeSource).y << "W :"
                 << road.at(red_ghost_coordinate.currentNodeSource).w
                 << "H :" << road.at(red_ghost_coordinate.currentNodeSource).h << "\n";

            for (auto gw: ghostWay) {
                *//*SDL_Rect rect;
                    rect.x = gw.x;
                    rect.y = gw.y;
                    if (gw.w > 0) {
                        rect.w = gw.w;
                        rect.h = BAND_WIDTH;
                    } else {
                        rect.w = BAND_WIDTH;
                        rect.h = gw.h;
                    }
                    ghost_coordinates[count] = rect;*//*
                    cout << "X:" << gw.x << " , ";
                    cout << "Y:" << gw.y << " , ";
                    cout << "W:" << gw.w << " , ";
                    cout << "H:" << gw.h << "\n";
                    count++;

                }
                test++;
            }*/
        }

        if (!ghostWay.empty()) {
            ghostMoving(ghostWay, red_ghost_coordinate, red_ghost_render, pacman, addNewPath);
        }

        pacman = tunnelSwap(pacman);

        pacmanRender.x = changeRender(pacman.x, WIDTH);
        pacmanRender.y = changeRender(pacman.y, HEIGHT);

        int count = 0;

        for (auto &point: pointsCoordinates) {
            if (pacman.x == point.x && pacman.y == point.y) {
                TotalScore += 10;
                if (TotalScore % 1000 == 0) {
                    score_position.w = BAND_HEIGHT;
                }
                pointsCoordinates.erase(pointsCoordinates.begin() + count);
            }
            count++;
        }


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);


        if (!printBoard(bounds_coordinatesRight, 33, 0, 0, 255)) return -1;
        if (!printBoard(bounds_coordinatesLeft, 33, 0, 0, 255)) return -1;
        if (!printBoard(ghost_coordinate, 8, 100, 0, 255)) return -1;
        if (!printGates(trap_gates, 255, 240, 0)) return -1;


        /*if (addNewRoad(red_ghost_currentRoad, red_ghost_coordinate, red_ghost_render)) {
            if (!ghostWay.empty()) {
                red_ghost_currentRoad = movingGhost(red_ghost_coordinate, ghostWay);
            }
        }*/


        stayInTrap(yellow_ghost_coordinate, YELLOW, ghostRoad, yellow_ghost_render);
        stayInTrap(blue_ghost_coordinate, BLUE, ghostRoad, blue_ghost_render);
        stayInTrap(pink_ghost_coordinate, PINK, ghostRoad, pink_ghost_render);


        SDL_RenderCopy(renderer, pacman_bitmap, NULL, &pacmanRender);
        SDL_RenderCopy(renderer, score_message, NULL, &messageRender);
        SDL_RenderCopy(renderer, yellow_ghost, NULL, &yellow_ghost_render);
        SDL_RenderCopy(renderer, blue_ghost, NULL, &blue_ghost_render);
        SDL_RenderCopy(renderer, pink_ghost, NULL, &pink_ghost_render);
        SDL_RenderCopy(renderer, red_ghost, NULL, &red_ghost_render);
        printPoint(pointsCoordinates);
        if (printScore(TotalScore)) {
            SDL_RenderCopy(renderer, scoreCount, NULL, &score_position);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
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

    for (auto &elementRoad: axisCellRoad) {
        int diff = 0;
        if (elementRoad.x < 0) {
            diff = abs(elementRoad.x);
            elementRoad.x = 0;
            elementRoad.w -= diff;
        } else if (elementRoad.x + elementRoad.w > WINDOW_WIDTH) {
            diff = elementRoad.x + elementRoad.w - WINDOW_WIDTH;
            elementRoad.w -= diff;
        }
    }

    sort(axisCellRoad.begin(), axisCellRoad.end(), compareByWidth);

    for (auto cell: axisCellRoad) {
        cout << "X:" << cell.x << " , ";
        cout << "Y:" << cell.y << " , ";
        cout << "W:" << cell.w << " , ";
        cout << "H:" << cell.h << "\n";
    }

    return axisCellRoad;
}

std::vector<CellRoad> generateRoad(SDL_Rect *rectsBoard, int length) {
    using namespace std;

    vector<CellRoad> road;

    for (int i = 0; i < length; ++i) {
        CellRoad roadElement;
        roadElement.x = rectsBoard[i].x;
        roadElement.y = rectsBoard[i].y;
        roadElement.w = rectsBoard[i].w;
        roadElement.h = rectsBoard[i].h;
        road.push_back(roadElement);
    }

    int count = 0;

    for (auto &roadElement: road) {
        if (roadElement.y == BOARD_SCORE_HEIGHT - BAND_WIDTH) {
            roadElement.y += BAND_HEIGHT / 2 + BAND_WIDTH;
            roadElement.w = BAND_HEIGHT + BAND_HEIGHT / 2 + 2 * BAND_WIDTH;
        } else if (roadElement.y == WINDOW_HEIGHT - BAND_WIDTH) {
            roadElement.y -= BAND_HEIGHT / 2;
            roadElement.w = BAND_HEIGHT / 2;
        } else if (roadElement.x == WINDOW_WIDTH - BAND_WIDTH) {
            if (count % 2 == 0) {
                roadElement.x -= 2 * BAND_HEIGHT + BAND_HEIGHT / 2 + BAND_WIDTH;
                roadElement.y -= BAND_HEIGHT / 2;
                roadElement.w = 5;
                roadElement.h = BAND_HEIGHT;
            } else {
                roadElement.y += roadElement.h - BAND_HEIGHT / 2 - 3 * BAND_WIDTH;
                roadElement.x = WINDOW_WIDTH / 2 + BAND_HEIGHT + BAND_HEIGHT / 2 + BAND_WIDTH;
                roadElement.w = 3 * BAND_HEIGHT + BAND_HEIGHT / 2 - 3 * BAND_WIDTH;
                roadElement.h = 5;
            }
        } else if (count >= 4 && count <= 8) {
            if (roadElement.w > BAND_WIDTH) {
                roadElement.y -= BAND_HEIGHT / 2;
                roadElement.x -= BAND_HEIGHT / 2;
                if (roadElement.w > BAND_HEIGHT) {
                    roadElement.w += BAND_HEIGHT / 2 + BAND_WIDTH;
                } else {
                    roadElement.w = 3 * BAND_HEIGHT + 2 * BAND_WIDTH;
                }
            } else {
                if (count % 2 == 0) {
                    roadElement.y -= BAND_HEIGHT / 2;
                    roadElement.x -= BAND_HEIGHT / 2;
                    roadElement.w = BAND_HEIGHT;
                    roadElement.h = 5;
                } else {
                    if (count == 5) {
                        roadElement.y -= BAND_HEIGHT / 2;
                        roadElement.x += BAND_HEIGHT / 2;
                        roadElement.h = 2 * BAND_HEIGHT;
                    } else {
                        roadElement.y += BAND_HEIGHT / 2;
                        roadElement.x -= BAND_HEIGHT / 2;
                        roadElement.h += BAND_HEIGHT;
                    }
                }
            }
        } else if (count >= 9 && count <= 22) {
            if (count <= 20) {
                if (count >= 9 && count <= 12) {
                    if (count< 11) {
                        if (count % 2 != 0) {
                            roadElement.x += BAND_HEIGHT / 2;
                            roadElement.y += BAND_HEIGHT / 2;
                            roadElement.h = BAND_HEIGHT;
                        } else {
                            roadElement.y -= BAND_HEIGHT / 2;
                            roadElement.w = 2 * BAND_HEIGHT + 2 * BAND_WIDTH;
                        }
                    } else {
                        if (count % 2 != 0) {
                            roadElement.x += BAND_HEIGHT / 2;
                            roadElement.y += BAND_HEIGHT / 2;
                            roadElement.h = BAND_HEIGHT;
                        } else {
                            roadElement.y -= BAND_HEIGHT / 2;
                            roadElement.w = 2 * BAND_HEIGHT + 2 * BAND_WIDTH + BAND_HEIGHT / 2;
                        }
                    }
                } else if (count >= 15 && count <= 16) {
                    if (count % 2 != 0) {
                        roadElement.x += BAND_HEIGHT / 2;
                        roadElement.y -= BAND_HEIGHT / 2;
                    } else {
                        roadElement.y -= BAND_HEIGHT / 2;
                        roadElement.x += BAND_HEIGHT / 2;
                        roadElement.w += BAND_WIDTH;
                    }
                } else {
                    if (count % 2 != 0) {
                        roadElement.x += BAND_HEIGHT / 2;
                        roadElement.y -= BAND_HEIGHT / 2;
                        roadElement.h += BAND_HEIGHT - BAND_WIDTH;
                    } else {
                        roadElement.x -= BAND_HEIGHT / 2;
                        roadElement.y -= BAND_HEIGHT / 2;
                        roadElement.w = BAND_HEIGHT + BAND_WIDTH;
                    }
                }
            } else {
                if (count % 2 != 0) {
                    roadElement.y += BAND_HEIGHT + BAND_HEIGHT / 2;
                    roadElement.x += BAND_HEIGHT + BAND_HEIGHT / 2 + BAND_WIDTH;
                    roadElement.h = BAND_HEIGHT + BAND_WIDTH;
                } else {
                    roadElement.y += BAND_HEIGHT / 2;
                    roadElement.w = BAND_HEIGHT / 2 + BAND_WIDTH;
                }
            }

        } else if (count >= 23 && count <= 26) {
            if (count< 25) {
                if (count % 2 != 0) {
                    roadElement.y -= BAND_HEIGHT / 2;
                    roadElement.x -= BAND_HEIGHT / 2;
                    roadElement.w = 3 * BAND_HEIGHT + 2 * BAND_WIDTH;
                } else {
                    roadElement.y -= BAND_HEIGHT / 2;
                    roadElement.x += BAND_HEIGHT / 2;
                    roadElement.h = 2 * BAND_HEIGHT;

                    road.push_back(roadElement);
                }
            } else {
                if (count % 2 != 0) {
                    roadElement.x += BAND_HEIGHT / 2;
                    roadElement.y -= BAND_HEIGHT / 2;
                    roadElement.h = 3 * BAND_HEIGHT;
                } else {
                    roadElement.y -= BAND_HEIGHT / 2;
                    roadElement.x -= BAND_HEIGHT / 2 + BAND_HEIGHT / 2 + BAND_WIDTH;
                    roadElement.w += BAND_HEIGHT / 2 + BAND_WIDTH;
                }
            }
        } else if (count >= 27 && count <= 32) {
            if (count< 30) {
                if (roadElement.w > BAND_WIDTH) {
                    if (count % 2 == 0) {
                        roadElement.x += BAND_HEIGHT + BAND_HEIGHT / 2 + BAND_WIDTH;
                        roadElement.y -= BAND_HEIGHT / 2;
                    } else {
                        roadElement.y -= BAND_HEIGHT / 2;
                        roadElement.x -= BAND_HEIGHT / 2;
                        roadElement.w += BAND_HEIGHT;
                    }
                } else {
                    roadElement.y -= BAND_HEIGHT / 2;
                    roadElement.x += BAND_HEIGHT / 2;
                    roadElement.h = 2 * BAND_HEIGHT;
                }

            } else {
                if (roadElement.w > BAND_WIDTH) {
                    if (count % 2 == 0) {
                        roadElement.y -= BAND_HEIGHT / 2;
                        roadElement.x -= BAND_HEIGHT / 2;
                        roadElement.w += BAND_HEIGHT / 2;
                    } else {
                        roadElement.y += BAND_HEIGHT / 2;
                        roadElement.x += BAND_HEIGHT / 2;
                        roadElement.h = BAND_HEIGHT;
                        roadElement.w = BAND_WIDTH;
                    }
                } else {
                    roadElement.x -= 2 * BAND_HEIGHT - BAND_HEIGHT / 2 + 2 * BAND_WIDTH;
                    roadElement.y += BAND_HEIGHT / 2;
                    roadElement.h = 3 * BAND_HEIGHT;
                }
            }
        }
        count++;
    }

    road.at(length).y += BAND_HEIGHT + 2 * (BAND_HEIGHT / 2);
    road.at(length).x -= BAND_HEIGHT;
    road.at(length).w = BAND_HEIGHT;
    road.at(length).h = BAND_WIDTH;

    for (auto &roadElement: road) {
        if (roadElement.w > BAND_WIDTH) {
            roadElement.h = 0;
        } else {
            roadElement.w = 0;
        }
    }

    return road;
}

std::vector<Cell> createPoints(std::vector<CellRoad> cellRoad) {
    std::vector<Cell> pointsCoordinate;

    int count = 0;
    int sum = 0;

    for (auto roadElement: cellRoad) {
        if (roadElement.w > 0) {
            count = 0;
            sum = 0;

            count = (int) roadElement.w / 15;

            for (int i = 0; i < count; i++) {
                Cell cell;
                cell.x = roadElement.x + sum;
                cell.y = roadElement.y;
                pointsCoordinate.push_back(cell);
                sum += 15;
            }
        }
    }

    for (auto roadElement: cellRoad) {
        if (roadElement.h > 0) {
            count = 0;
            sum = 0;

            count = (int) roadElement.h / 15;

            for (auto roadElement1: cellRoad) {
                if (roadElement.x > roadElement1.x && roadElement.x < roadElement1.x + roadElement1.w &&
                    roadElement.y == roadElement1.y) {
                    count--;
                    sum = 15;
                }
            }

            for (int i = 0; i < count; ++i) {
                Cell cell;
                cell.y = roadElement.y + sum;
                cell.x = roadElement.x;
                pointsCoordinate.push_back(cell);
                sum += 15;
            }
        }
    }

    return pointsCoordinate;
}

std::vector<CellRoad> createTrapRoad(SDL_Rect *rectL, SDL_Rect *rectR, int length) {
    using namespace std;
    vector<CellRoad> trapRoad;

    for (int i = 0; i < length; ++i) {
        for (int j = 0; j < length; ++j) {
            if (i == 27) {
                if (rectR[i].w > BAND_WIDTH) {
                    int endWidth = rectR[i].x + rectR[i].w;
                    if (rectL[j].x == WINDOW_WIDTH - endWidth && rectL[j].y == rectR[i].y) {
                        int temp = BAND_HEIGHT / 4;
                        int gap = rectR[i].x - (rectL[j].x + rectL[j].w);
                        int totalWidth = rectL[j].w + rectR[i].w + gap;
                        int halfOfRects = totalWidth / 2;

                        CellRoad cellRoad;
                        cellRoad.x = rectL[j].x + temp;
                        cellRoad.y = rectL[j].y + temp;
                        cellRoad.w = BAND_HEIGHT + (BAND_HEIGHT / 2);
                        cellRoad.h = 0;
                        trapRoad.push_back(cellRoad);
                        cout << "[" << i << "]" << " Trap\n";
                        cout << "X:" << rectR[i].x << " , ";
                        cout << "Y:" << rectR[i].y << " , ";
                        cout << "W:" << rectR[i].w << " , ";
                        cout << "H:" << rectR[i].h << "\n";
                        cout << "[" << i << "]" << " Trap\n";
                        cout << "X:" << rectL[j].x << " , ";
                        cout << "Y:" << rectL[j].y << " , ";
                        cout << "W:" << rectL[j].w << " , ";
                        cout << "H:" << rectL[j].h << "\n";
                    }
                }
            }
        }
    }

    int width = trapRoad.at(0).w;
    int halfOfWidth = width / 2;
    int bar = trapRoad.at(0).x;


    for (int i = 0; i < 3; ++i) {
        CellRoad road;
        road.x = bar;
        road.y = trapRoad.at(0).y;
        road.h = BAND_HEIGHT / 2;
        road.w = 0;
        trapRoad.push_back(road);
        bar += halfOfWidth;
    }


    return trapRoad;
}

std::vector<CellRoad> joinGhostRoadWithPacManRoad(std::vector<CellRoad> rp, std::vector<CellRoad> rg) {

    using namespace std;

    std::vector<CellRoad> completeRoad = rg;

    for (auto roadP: rp) {
        for (auto roadG: rg) {
            if (roadP.w > 0) {
                if (roadG.w > 0) {
                    int diff = roadG.y > roadP.y ? roadG.y - roadP.y : 100;
                    if (diff < BAND_HEIGHT && roadG.x >= roadP.x && roadG.x <= roadP.x + roadP.w) {
                        CellRoad road;
                        road.y = roadP.y;
                        road.x = roadG.x + (roadG.w / 2);
                        road.h = roadG.y - roadP.y;
                        road.w = 0;
                        completeRoad.push_back(road);
                    }
                }
            }
        }
    }

    return completeRoad;
}

std::vector<std::vector<CellRoad>> createGraphWithAllNeighbours(std::vector<CellRoad> &road) {

    using namespace std;

    vector<vector<CellRoad>> adjList(road.size());

    int count = 0;

    for (auto cell: road) {
        for (auto cell1: road) {
            if (cell.w > 0) {
                if (cell1.h > 0) {
                    if (cell.x + cell.w == cell1.x && cell.y == cell1.y) {
                        adjList[count].push_back(cell1);
                    } else if (cell1.x >= cell.x && cell1.x <= cell.x + cell.w && cell1.y == cell.y) {
                        adjList[count].push_back(cell1);
                    } else if (cell1.x >= cell.x && cell1.x <= cell.x + cell.w && cell1.y + cell1.h == cell.y) {
                        adjList[count].push_back(cell1);
                    } else if (cell.x == cell1.x && cell.y >= cell1.y && cell.y <= cell1.y + cell1.h) {
                        adjList[count].push_back(cell1);
                    } else if (cell.x + cell.w == cell1.x && cell.y >= cell1.y && cell.y <= cell1.y + cell1.h) {
                        adjList[count].push_back(cell1);
                    } else if (cell.x <= cell1.x && cell.x + cell.w >= cell1.x && cell.y > cell1.y &&
                               cell.y < cell1.y + cell1.h) {
                        adjList[count].push_back(cell1);
                    }
                }
            } else {
                if (cell1.w > 0) {
                    if (cell.y + cell.h == cell1.y && cell.x == cell1.x) {
                        adjList[count].push_back(cell1);
                    } else if (cell.y + cell.h == cell1.y && cell.x >= cell1.x && cell.x <= cell1.x + cell1.w) {
                        adjList[count].push_back(cell1);
                    } else if (cell1.y >= cell.y && cell1.y <= cell.y + cell.h && cell.x >= cell1.x &&
                               cell.x <= cell1.x + cell1.w) {
                        adjList[count].push_back(cell1);
                    }
                }
            }
        }
        count++;
    }

    return adjList;
}

/*CellRoad movingGhost(ghost ghost, std::vector<CellRoad> &ghostRoad) {
    CellRoad currentRoad;
    currentRoad = ghostRoad.at(0);
    ghostRoad.erase(ghostRoad.begin());
    return currentRoad;
}*/

SDL_Rect setGhostsPosition(std::vector<CellRoad> &ghostRoad, ghost_colors c, ghost &ghostCoordinate) {
    using namespace std;

    SDL_Rect ghost_position;

    sort(ghostRoad.begin(), ghostRoad.end(), compareByLength);

    int count = 0;

    for (auto &road: ghostRoad) {
        if (road.h > 0) {
            switch (c) {
                case RED:
                    if (count == 3) {
                        ghost_position.x = road.x;
                        ghost_position.y = road.y;
                        ghost_position.w = 30;
                        ghost_position.h = 30;
                    }
                    break;
                case YELLOW:
                    if (count == 2) {
                        ghost_position.x = road.x;
                        ghost_position.y = road.y;
                        ghost_position.w = 30;
                        ghost_position.h = 30;
                    }
                    break;
                case BLUE:
                    if (count == 0) {
                        ghost_position.x = road.x;
                        ghost_position.y = road.y;
                        ghost_position.w = 30;
                        ghost_position.h = 30;
                    }
                    break;
                case PINK:
                    if (count == 1) {
                        ghost_position.x = road.x;
                        ghost_position.y = road.y;
                        ghost_position.w = 30;
                        ghost_position.h = 30;
                    }
                    break;
            }
            count++;
        }
    }

    ghostCoordinate.x = ghost_position.x;
    ghostCoordinate.y = ghost_position.y;
    ghostCoordinate.d = DOWN;

    return ghost_position;
}

void bfs(std::vector<std::vector<CellRoad>> &adjList, int startNode, std::vector<int> &par, std::vector<int> &dist,
         std::vector<CellRoad> &road) {


    std::vector<int> queue;
    dist[startNode] = 0;
    queue.push_back(startNode);


    while (!queue.empty()) {

        int currentNode = queue.front();
        queue.erase(queue.begin());


        for (CellRoad neighbour: adjList[currentNode]) {

            int count = 0;

            for (auto cell: road) {
                if (cell.x == neighbour.x && cell.y == neighbour.y && cell.w == neighbour.w) {
                    if (dist[count] == 1e9) {
                        par[count] = currentNode;
                        dist[count] = dist[currentNode] + 1;
                        queue.push_back(count);
                    }
                }
                count++;
            }
        }
    }
}

std::vector<CellRoad> getShortestPath(std::vector<std::vector<CellRoad>> &adjList, int source, int destination,
                                      std::vector<CellRoad> &road) {

    using namespace std;

    vector<int> par(adjList.size(), -1);
    vector<int> dist(adjList.size(), 1e9);

    bfs(adjList, source, par, dist, road);

    if (dist[destination] == 1e9) {
        cout << "Source and destination are not connected";
    }

    vector<CellRoad> path;

    int currentNode = destination;

    path.push_back(road.at(destination));

    while (par[currentNode] != -1) {
        path.push_back(road.at(par[currentNode]));
        currentNode = par[currentNode];
    }

    return path;
}

bool addNewRoad(CellRoad &cellRoad, ghost &ghost, SDL_Rect &renderGhost) {

    renderGhost.x = ghost.x;
    renderGhost.y = ghost.y;


    if (cellRoad.w > 0) {
        if (cellRoad.x >= ghost.x) {
            ghost.x = cellRoad.x++;
            cellRoad.w--;
        } else {
            ghost.x = (cellRoad.x + cellRoad.w) - 1;
            cellRoad.w--;
        }
        return false;
    } else if (cellRoad.h > 0) {
        if (cellRoad.y >= ghost.y) {
            ghost.y = cellRoad.y++;
            cellRoad.h--;
        } else {
            ghost.y = (cellRoad.y + cellRoad.h) - 1;
            cellRoad.h--;
        }
        return false;
    }
    return true;
}

bool compareByHeight(const CellRoad &a, const CellRoad &b) {
    return a.y < b.y || (a.y == b.y && a.x < b.x);
}

bool compareByWidth(const CellRoad &a, const CellRoad &b) {
    return a.x < b.x || (a.x == b.x && a.y < b.y);
}

bool compareByLength(const CellRoad &a, const CellRoad &b) {
    return a.h < b.h;
}

void printPoint(const std::vector<Cell> &pointsCoordinates) {
    using namespace std;

    int length = (int) pointsCoordinates.size();
    SDL_Rect points[length];

    int count = 0;

    for (auto &point: pointsCoordinates) {
        points[count].x = point.x;
        points[count].y = point.y;
        points[count].w = 10;
        points[count].h = 10;
        SDL_RenderCopy(renderer, point_bitmap, NULL, &points[count]);
    }

}

bool printBoard(SDL_Rect *rects, int count, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 0);
    using namespace std;
    if (SDL_RenderFillRects(renderer, rects, count) != 0) {
        cout << "surface board: " << SDL_GetError() << endl;
        return false;
    }
    return true;
}

bool printScore(int TotalScore) {

    using namespace std;

    SDL_Surface *score;

    SDL_Color White = {255, 255, 255};
    SDL_Color Black = {0, 0, 0};

    auto s = to_string(TotalScore);
    char const *ch = s.c_str();

    score = TTF_RenderText(Sans, ch, White, Black);

    scoreCount = SDL_CreateTextureFromSurface(renderer, score);

    SDL_FreeSurface(score);

    if (!scoreCount) {
        cout << "Error Converting surface ScoreMesssage" << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    return true;
}

bool printGates(SDL_Rect gates, Uint8 r, Uint8 g, Uint8 b) {
    using namespace std;

    SDL_SetRenderDrawColor(renderer, r, g, b, 0);
    if (SDL_RenderFillRect(renderer, &gates) != 0) {
        cout << "surface board: " << SDL_GetError() << endl;
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
                                                                                   WINDOW_WIDTH / 2 +
                                                                                   BAND_HEIGHT / 2;
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
            /*cout << "[" << count << "]" << " Trap\n";
            cout << "X:" << band.x << " , ";
            cout << "Y:" << band.y << " , ";
            cout << "W:" << band.w << " , ";
            cout << "H:" << band.h << "\n";*/

            ++count;
        }
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

void stayInTrap(ghost &ghost, ghost_colors ghostColors, std::vector<CellRoad> &ghostRoad, SDL_Rect &ghostRender) {


    int count = 0;

    for (auto ghostPosition: ghostRoad) {
        switch (ghostColors) {
            case RED:
                break;
            case YELLOW:
                if (count == 2) {
                    if (ghost.y == ghostPosition.y) {
                        ghost.d = DOWN;
                    } else if (ghost.y == ghostPosition.y + ghostPosition.h) {
                        ghost.d = UP;
                    }
                }
                break;
            case BLUE:
                if (count == 3) {
                    if (ghost.y == ghostPosition.y) {
                        ghost.d = DOWN;
                    } else if (ghost.y == ghostPosition.y + ghostPosition.h) {
                        ghost.d = UP;
                    }
                }
                break;
            case PINK:
                if (count == 1) {
                    if (ghost.y == ghostPosition.y) {
                        ghost.d = DOWN;
                    } else if (ghost.y == ghostPosition.y + ghostPosition.h) {
                        ghost.d = UP;
                    }
                }
                break;
        }
        count++;
    }

    switch (ghost.d) {
        case UP:
            ghost.y--;
            break;
        case DOWN:
            ghost.y++;
            break;
    }

    ghostRender.x = ghost.x;
    ghostRender.y = ghost.y;

}

void setSourceNodeGhost(ghost &ghostCoordinate, std::vector<CellRoad> &road) {

    int count = 0;

    for (auto currentNode: road) {

        if (ghostCoordinate.x >= currentNode.x && ghostCoordinate.x <= currentNode.x + currentNode.w &&
            ghostCoordinate.y == currentNode.y) {
            ghostCoordinate.currentNodeSource = count;
        } else if (ghostCoordinate.y >= currentNode.y && ghostCoordinate.y <= currentNode.y + currentNode.h &&
                   ghostCoordinate.x == currentNode.x) {
            ghostCoordinate.currentNodeSource = count;
        }

        count++;
    }
}

void ghostMoving(std::vector<CellRoad> &ghostWay, ghost &ghostCoordinate, SDL_Rect &ghostRendering,
                 pacman &pacmanCoordinate, bool &addNewPath) {


    CellRoad lastElement = ghostWay.front();
    CellRoad nextElement = {};


    if (ghostWay.size() > 1) {
        nextElement = ghostWay[1];

        if (lastElement.w > 0) {
            if (lastElement.x <= ghostCoordinate.x && lastElement.x + lastElement.w >= ghostCoordinate.x) {

                if (nextElement.x > ghostCoordinate.x) {
                    ghostCoordinate.d = RIGHT;
                } else if (nextElement.x < ghostCoordinate.x) {
                    ghostCoordinate.d = LEFT;
                }

                if (ghostCoordinate.x == nextElement.x) {
                    ghostWay.erase(ghostWay.begin());
                    ghostCoordinate.d = STOP;
                    addNewPath = true;
                }

            }

        } else {
            if (lastElement.y <= ghostCoordinate.y && lastElement.y + lastElement.h >= ghostCoordinate.y) {

                if (nextElement.y < ghostCoordinate.y) {
                    ghostCoordinate.d = UP;
                } else if (nextElement.y > ghostCoordinate.y) {
                    ghostCoordinate.d = DOWN;
                }

                if (ghostCoordinate.y == nextElement.y) {
                    ghostWay.erase(ghostWay.begin());
                    ghostCoordinate.d = STOP;
                    addNewPath = true;
                }

            }
        }
    } else {
        if (lastElement.w > 0) {
            if (pacmanCoordinate.x > ghostCoordinate.x) {
                ghostCoordinate.d = RIGHT;
            } else if (pacmanCoordinate.x < ghostCoordinate.x) {
                ghostCoordinate.d = LEFT;
            }
        } else {
            if (pacmanCoordinate.y > ghostCoordinate.y) {
                ghostCoordinate.d = DOWN;
            } else if (pacmanCoordinate.y < ghostCoordinate.y) {
                ghostCoordinate.d = UP;
            }
        }
    }


    switch (ghostCoordinate.d) {
        case UP:
            ghostCoordinate.y--;
            break;
        case DOWN:
            ghostCoordinate.y++;
            break;
        case LEFT:
            ghostCoordinate.x--;
            break;
        case RIGHT:
            ghostCoordinate.x++;
            break;
        case STOP:
            break;
    }

    ghostRendering.x = ghostCoordinate.x;
    ghostRendering.y = ghostCoordinate.y;

}

bool escapeFromTrap(ghost &ghostCoordinate, std::vector<CellRoad> &road, bool &escape, SDL_Rect &ghostRender) {

    for (auto cell: road) {
        if (ghostCoordinate.x >= cell.x && ghostCoordinate.x <= cell.x + cell.w && ghostCoordinate.y == cell.y) {
            if (cell.w > 0) {
                if (escape) {
                    if (ghostCoordinate.x < cell.x + cell.w) {
                        ghostCoordinate.d = RIGHT;
                    } else {
                        return true;
                    }
                } else {
                    if (ghostCoordinate.x > cell.x) {
                        ghostCoordinate.d = LEFT;
                    } else {
                        return true;
                    }
                }
            }
        }
    }

    switch (ghostCoordinate.d) {
        case RIGHT:
            ghostCoordinate.x++;
            break;
        case LEFT:
            ghostCoordinate.x--;
            break;
    }

    ghostRender.x = ghostCoordinate.x;
    ghostRender.y = ghostCoordinate.y;

    return false;
}

int changeRender(int coordinate, coordinates c) {

    int position = 0;

    switch (c) {
        case WIDTH:
            if (coordinate == WINDOW_WIDTH) {
                position = coordinate;
            } else if (coordinate == 0) {
                position = coordinate - 30;
            } else {
                position = coordinate - (30 / 2);
            }
            break;
        case HEIGHT:
            position = coordinate - (30 / 2);
            break;
    }

    return position;
}

pacman tunnelSwap(pacman &PacMan) {

    pacman pacman1 = PacMan;

    int tunnelDirect = 0;

    if (PacMan.x == 0) {
        tunnelDirect = 1;
    } else if (PacMan.x == WINDOW_WIDTH) {
        tunnelDirect = 2;
    }

    switch (tunnelDirect) {
        case 1:
            pacman1.x = WINDOW_WIDTH;
            break;
        case 2:
            pacman1.x = 0;
            break;
    }

    return pacman1;
}

bool load() {
    using namespace std;
    SDL_Surface *temp1;

    temp1 = SDL_LoadBMP("/Users/dawid/ClionProjects/PacMan/assets/Pacman.bmp");

    if (!temp1) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    pacman_bitmap = SDL_CreateTextureFromSurface(renderer, temp1);
    /*pacman_bitmap = SDL_ConvertSurface(temp1, winSurface->format, 0);*/

    SDL_FreeSurface(temp1);

    if (!pacman_bitmap) {
        cout << "Error Converting surface PacManBitmap" << SDL_GetError() << endl;
        system("pause");
        return false;
    }


    SDL_Surface *temp2;

    temp2 = SDL_LoadBMP("/Users/dawid/ClionProjects/PacMan/assets/Point.bmp");

    if (!temp2) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    point_bitmap = SDL_CreateTextureFromSurface(renderer, temp2);
    /*point_bitmap = SDL_ConvertSurface(temp2, winSurface->format, 0);*/

    SDL_FreeSurface(temp2);

    if (!point_bitmap) {
        cout << "Error Converting surface PointBitmap" << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    Sans = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 24);

    if (!Sans) {
        cout << "Error initialize font : " << TTF_GetError() << endl;
        system("pause");
        return false;
    }

    SDL_Surface *temp3;

    SDL_Color White = {255, 255, 255};
    SDL_Color Black = {0, 0, 0};

    temp3 = TTF_RenderText(Sans, "SCORE : ", White, Black);

    score_message = SDL_CreateTextureFromSurface(renderer, temp3);

    SDL_FreeSurface(temp3);

    if (!score_message) {
        cout << "Error Converting surface ScoreMesssage" << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    SDL_Surface *temp4;

    temp4 = SDL_LoadBMP("/Users/dawid/ClionProjects/PacMan/assets/Yellow_Ghost.bmp");

    if (!temp4) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    yellow_ghost = SDL_CreateTextureFromSurface(renderer, temp4);

    SDL_FreeSurface(temp4);

    SDL_Surface *temp5;

    temp5 = SDL_LoadBMP("/Users/dawid/ClionProjects/PacMan/assets/Red_Ghost.bmp");

    if (!temp5) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    red_ghost = SDL_CreateTextureFromSurface(renderer, temp5);

    SDL_FreeSurface(temp5);

    SDL_Surface *temp6;

    temp6 = SDL_LoadBMP("/Users/dawid/ClionProjects/PacMan/assets/Pink_Ghost.bmp");

    if (!temp6) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    pink_ghost = SDL_CreateTextureFromSurface(renderer, temp6);

    SDL_FreeSurface(temp6);

    SDL_Surface *temp7;

    temp7 = SDL_LoadBMP("/Users/dawid/ClionProjects/PacMan/assets/Blue_Ghost.bmp");

    if (!temp7) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    blue_ghost = SDL_CreateTextureFromSurface(renderer, temp7);

    SDL_FreeSurface(temp7);


    return true;
}

bool init() {
    using namespace std;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cout << "Error initializing SDL: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    if (TTF_Init() < 0) {
        cout << "Error initializing TTF: " << TTF_GetError() << endl;
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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        cout << "renderer: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    return true;
}

void kill() {
    SDL_DestroyTexture(pacman_bitmap);
    SDL_DestroyTexture(yellow_ghost);
    SDL_DestroyTexture(point_bitmap);
    SDL_DestroyTexture(score_message);
    SDL_DestroyTexture(scoreCount);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}


