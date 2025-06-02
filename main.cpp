#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include "chrono"
#include <random>


SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *pacman_bitmap;
SDL_Texture *yellow_ghost;
SDL_Texture *red_ghost;
SDL_Texture *pink_ghost;
SDL_Texture *blue_ghost;
SDL_Texture *dead_ghost;
SDL_Texture *dead_ghostV2;
SDL_Texture *ghost_eyes;
SDL_Texture *point_bitmap;
SDL_Texture *bonus_point;
SDL_Texture *score_message;
SDL_Texture *scoreCount;
TTF_Font *Sans;

const int WINDOW_WIDTH = 640; // *64

const int WINDOW_HEIGHT = 650; // *60

const int BOARD_SCORE_HEIGHT = 50;

const int BOARD_START = BOARD_SCORE_HEIGHT;

const int BAND_WIDTH = 5;

const int BAND_HEIGHT = 60;

const int PACMAN_RENDER_X = 319;
const int PACMAN_RENDER_Y = 380;


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
    int lives = 2;
};

enum ghost_colors {
    RED, YELLOW, PINK, BLUE, OTHER
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

struct ghost {
    int y;
    int x;
    directions d;
    int currentNodeSource;
    bool trapped = true;
    bool dead = false;
    std::vector<CellRoad> ghostWay;
    std::vector<CellRoad> ghostRunAway;
    std::vector<CellRoad> roadToTrap;
    bool addNewPath;
    float slowingGhost = 0.0f;
    bool readyToEscape = false;
    bool readyToLeave = false;
    bool escaped = false;
    bool caught = false;
    float printNextTexture = 0.5f;
    bool deleted = false;
};

std::vector<CellRoad> joinElements(std::vector<CellRoad> cellRoad);

std::vector<CellRoad> x_axis_Road(std::vector<CellRoad> cellRoad);

std::vector<CellRoad> generateRoad(SDL_Rect *rectsBoard, int length);

std::vector<Cell> createPoints(std::vector<CellRoad> cellRoad);

std::vector<Cell> createBonusPoints(std::vector<Cell> &pointsCoordinate, std::vector<CellRoad> road);

std::vector<CellRoad> createTrapRoad(SDL_Rect *rectsL, SDL_Rect *rectR, int length);

std::vector<CellRoad> joinGhostRoadWithPacManRoad(std::vector<CellRoad> rp, std::vector<CellRoad> rg);

bool compareByHeight(const CellRoad &a, const CellRoad &b);

bool compareByWidth(const CellRoad &a, const CellRoad &b);

bool compareByLength(const CellRoad &a, const CellRoad &b);

bool printBoard(SDL_Rect *rects, int count, Uint8 r, Uint8 g, Uint8 b);

void printPoint(const std::vector<Cell> &pointsCoordinates);

void printBonusPoint(const std::vector<Cell> &bonusPointsCoordinates);

bool printScore(int TotalScore);

bool printGates(SDL_Rect gates, Uint8 r, Uint8 g, Uint8 b);

void generateBoard(SDL_Rect *rectsBoard);

void x_axis_Board(SDL_Rect *rectsBoardRight, SDL_Rect *rectsBoardLeft, int length);

int changeRender(int coordinate, coordinates c);

void setGhostsPosition(std::vector<CellRoad> &ghostRoad, ghost_colors c, ghost &ghostCoordinate);

pacman tunnelSwap(pacman &pacman);

void bfs(std::vector<std::vector<CellRoad> > &adjList, int startNode, std::vector<int> &par, std::vector<int> &dist,
         std::vector<CellRoad> &road, ghost_colors ghostColors);

std::vector<CellRoad> getShortestPath(std::vector<std::vector<CellRoad> > &adjList, int source, int destination,
                                      std::vector<CellRoad> &road, ghost_colors ghostColors);

std::vector<std::vector<CellRoad> > createGraphWithAllNeighbours(std::vector<CellRoad> &road);

void stayInTrap(ghost &ghost, ghost_colors ghostColors, std::vector<CellRoad> &ghostRoad);

bool
leaveTheTrap(std::vector<CellRoad> &ghostRoad, ghost &ghostCoordinate, float &slowMoving,
             bool &readyToLeave);

void setSourceNodeGhost(ghost &ghostCoordinate, std::vector<CellRoad> &road);

void ghostMoving(std::vector<CellRoad> &ghostWay, ghost &ghostCoordinate,
                 pacman &pacmanCoordinate, bool &addNewPath);

bool escapeFromTrap(ghost &ghostCoordinate, std::vector<CellRoad> &road, bool &escape,
                    bool &escaped);

void movingInTrap(ghost &blinky, ghost &pinky, ghost &inky, ghost &clyde, std::vector<CellRoad> &ghostRoad);

bool setEscapePosition(ghost &any, std::vector<CellRoad> &ghostRoad, bool &readyToEscape);

void increaseCountGhosts(int totalScore, ghost &blinky, ghost &pinky, ghost &inky, ghost &clyde);

void setDeadAllGhosts(ghost &red, ghost &yellow, ghost &blue, ghost &pink);

void renderGhosts(ghost &redCoordinate, ghost &yelloCoordinate,
                  ghost &pinkCoordinate, ghost &blueCoordinate);

void
runAwayGhostMode(ghost &ghostCoordinate, std::vector<std::vector<CellRoad> > &neighboursRoad, pacman &pacmanCoordinate,
                 std::vector<CellRoad> &road);

void runningAway(ghost &ghostCoordinate);

bool
meetingPacmanWithGhosts(ghost &redCoordinate, ghost &yellowCoordinate, ghost &blueCoordinate, ghost &pinkCoordinate,
                        pacman &pacmanCoordinate, std::vector<CellRoad> &ghostRoad, bool &startGame, int &direction,
                        int &nextDirection, bool &respawn, int &spawnPlace, int &maxPoint);

bool
findFreePlaceInTrap(ghost &ghostCoordinate, std::vector<CellRoad> &ghostRoad, int &spawnPlace, CellRoad &freePlace,
                    int &total_score);

size_t findSpawnPlace(CellRoad &destination, std::vector<CellRoad> &joinedRoad);

void runningToTrap(ghost &ghostCoordinate);

void clearAllRoads(ghost &ghostCoordinate);

void clearAttributesBackToTrap(ghost &ghostCoordinate);

void clearAttributesRespawnCharacters(ghost &ghostCoordinate);

void respawnCharacters(ghost &redCoordinate, ghost &yellowCoordinate,
                       ghost &blueCoordinate,
                       ghost &pinkCoordinate,
                       pacman &pacmanCoordinate, std::vector<CellRoad> &ghostRoad, bool &startGame, int &direction,
                       int &nextDirection, bool &respawn, int &spawnPlace);

void printLives(pacman &pacmanCoordinate);

bool load();

bool init();

void kill();

int main(int argc, char *args[]) {
    using namespace std;

    if (!init()) return 1;

    if (!load()) return 1;

    int TotalScore = 0;
    int maxPoint = 0;
    int spawnPlace = 3;
    bool startGame = false;
    bool escape = false;
    bool respawn = false;

    pacman pacman = {};
    pacman.x = PACMAN_RENDER_X;
    pacman.y = PACMAN_RENDER_Y;


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


    SDL_Rect trap_gates;
    trap_gates.x = 290;
    trap_gates.y = 290;
    trap_gates.w = 60;
    trap_gates.h = BAND_WIDTH / 2;

    std::vector<CellRoad> road;
    std::vector<CellRoad> ghostRoad;
    std::vector<CellRoad> joinedRoad;
    std::vector<Cell> pointsCoordinates;
    std::vector<Cell> bonusPointsCoordinates;

    std::vector<std::vector<CellRoad> > neighboursGraph;
    std::vector<std::vector<CellRoad> > joinedNeighboursGraph;


    generateBoard(bounds_coordinatesRight);
    x_axis_Board(bounds_coordinatesRight, bounds_coordinatesLeft, 33);


    road = generateRoad(bounds_coordinatesRight, 33);
    road = joinElements(road);
    road = x_axis_Road(road);


    ghostRoad = createTrapRoad(bounds_coordinatesLeft, bounds_coordinatesRight, 33);
    ghostRoad = joinGhostRoadWithPacManRoad(road, ghostRoad);

    neighboursGraph = createGraphWithAllNeighbours(road);

    joinedRoad.insert(joinedRoad.end(), road.begin(), road.end());
    joinedRoad.insert(joinedRoad.end(), ghostRoad.begin(), ghostRoad.end());
    joinedNeighboursGraph = createGraphWithAllNeighbours(joinedRoad);

    int count = 0;

    pointsCoordinates = createPoints(road);
    bonusPointsCoordinates = createBonusPoints(pointsCoordinates, road);

    setGhostsPosition(ghostRoad, YELLOW, yellow_ghost_coordinate);
    setGhostsPosition(ghostRoad, PINK, pink_ghost_coordinate);
    setGhostsPosition(ghostRoad, BLUE, blue_ghost_coordinate);
    setGhostsPosition(ghostRoad, RED, red_ghost_coordinate);


    int direction = 0, next_direction = 0;
    int state = 0;
    int next_way = 0;
    int key = 0, temp = 0, smooth = 0;


    int previousNodePacMan = 0;

    chrono::high_resolution_clock::time_point t1;
    chrono::high_resolution_clock::time_point t2;

    SDL_Event event;
    int playing = 1;

    while (playing) {
        int currentNodePacMan = 0;
        previousNodePacMan = pacman.currentNodeDestination;
        smooth = 0;
        temp = 0;
        key = 0;

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
        setSourceNodeGhost(pink_ghost_coordinate, road);
        setSourceNodeGhost(blue_ghost_coordinate, road);
        setSourceNodeGhost(yellow_ghost_coordinate, road);


        increaseCountGhosts(TotalScore, red_ghost_coordinate, pink_ghost_coordinate, blue_ghost_coordinate,
                            yellow_ghost_coordinate);

        if (direction != 0 && !startGame) {
            if (escapeFromTrap(red_ghost_coordinate, road, escape, red_ghost_coordinate.escaped)) {
                red_ghost_coordinate.ghostWay = getShortestPath(neighboursGraph, pacman.currentNodeDestination,
                                                                red_ghost_coordinate.currentNodeSource,
                                                                road, RED);
                red_ghost_coordinate.trapped = false;
                escape = !escape;
                startGame = !startGame;
            }
        }

        if (respawn && startGame) {
            if (pink_ghost_coordinate.trapped) {
                pink_ghost_coordinate.trapped = false;
            }

            if (pink_ghost_coordinate.readyToLeave && blue_ghost_coordinate.trapped) {
                blue_ghost_coordinate.trapped = false;
            }

            if (blue_ghost_coordinate.readyToLeave && yellow_ghost_coordinate.trapped) {
                yellow_ghost_coordinate.trapped = false;
            }
        }


        if (red_ghost_coordinate.dead || pink_ghost_coordinate.dead || blue_ghost_coordinate.dead ||
            yellow_ghost_coordinate.dead) {
            t2 = chrono::high_resolution_clock::now();
            auto time_span = chrono::duration_cast<chrono::duration<int> >(t2 - t1);

            if (time_span.count() >= 7) {
                if (red_ghost_coordinate.dead) {
                    red_ghost_coordinate.printNextTexture += 0.5f;
                }

                if (pink_ghost_coordinate.dead) {
                    pink_ghost_coordinate.printNextTexture += 0.5f;
                }

                if (blue_ghost_coordinate.dead) {
                    blue_ghost_coordinate.printNextTexture += 0.5f;
                }
                if (yellow_ghost_coordinate.dead) {
                    yellow_ghost_coordinate.printNextTexture += 0.5f;
                }
            }

            if (time_span.count() >= 10) {
                if (red_ghost_coordinate.dead) {
                    red_ghost_coordinate.dead = false;
                    red_ghost_coordinate.printNextTexture = 0.5f;
                    red_ghost_coordinate.slowingGhost = 0.0f;
                    red_ghost_coordinate.ghostRunAway.clear();
                    red_ghost_coordinate.ghostWay.clear();
                }
                if (pink_ghost_coordinate.dead) {
                    pink_ghost_coordinate.dead = false;
                    pink_ghost_coordinate.printNextTexture = 0.5f;
                    pink_ghost_coordinate.slowingGhost = 0.0f;
                    pink_ghost_coordinate.ghostRunAway.clear();
                    pink_ghost_coordinate.ghostWay.clear();
                }
                if (blue_ghost_coordinate.dead) {
                    blue_ghost_coordinate.dead = false;
                    blue_ghost_coordinate.printNextTexture = 0.5f;
                    blue_ghost_coordinate.slowingGhost = 0.0f;
                    blue_ghost_coordinate.ghostRunAway.clear();
                    blue_ghost_coordinate.ghostWay.clear();
                }
                if (yellow_ghost_coordinate.dead) {
                    yellow_ghost_coordinate.dead = false;
                    yellow_ghost_coordinate.printNextTexture = 0.5f;
                    yellow_ghost_coordinate.slowingGhost = 0.0f;
                    yellow_ghost_coordinate.ghostRunAway.clear();
                    yellow_ghost_coordinate.ghostWay.clear();
                }
            }
        }

        if (pacman.x == next_way || pacman.y == next_way) {
            direction = next_direction;
        }

        if (previousNodePacMan != pacman.currentNodeDestination && startGame) {
            if (red_ghost_coordinate.addNewPath) {
                red_ghost_coordinate.ghostWay = getShortestPath(neighboursGraph, pacman.currentNodeDestination,
                                                                red_ghost_coordinate.currentNodeSource,
                                                                road, RED);
            }

            if (pink_ghost_coordinate.addNewPath) {
                pink_ghost_coordinate.ghostWay = getShortestPath(neighboursGraph, pacman.currentNodeDestination,
                                                                 pink_ghost_coordinate.currentNodeSource,
                                                                 road, PINK);
            }

            if (blue_ghost_coordinate.addNewPath) {
                blue_ghost_coordinate.ghostWay = getShortestPath(neighboursGraph, pacman.currentNodeDestination,
                                                                 blue_ghost_coordinate.currentNodeSource,
                                                                 road, BLUE);
            }

            if (yellow_ghost_coordinate.addNewPath) {
                yellow_ghost_coordinate.ghostWay = getShortestPath(neighboursGraph, pacman.currentNodeDestination,
                                                                   yellow_ghost_coordinate.currentNodeSource,
                                                                   road, YELLOW);
            }
        }

        movingInTrap(red_ghost_coordinate, pink_ghost_coordinate,
                     blue_ghost_coordinate, yellow_ghost_coordinate,
                     ghostRoad);


        if (!red_ghost_coordinate.trapped && !red_ghost_coordinate.escaped) {
            if (!red_ghost_coordinate.readyToEscape) {
                setEscapePosition(red_ghost_coordinate, ghostRoad,
                                  red_ghost_coordinate.readyToEscape);
            } else {
                if (!red_ghost_coordinate.readyToLeave) {
                    if (leaveTheTrap(ghostRoad, red_ghost_coordinate,
                                     red_ghost_coordinate.slowingGhost, red_ghost_coordinate.readyToLeave)) {
                        escape = !escape;
                        spawnPlace--;
                    }
                } else {
                    if (escapeFromTrap(red_ghost_coordinate, road, escape,
                                       red_ghost_coordinate.escaped)) {
                        red_ghost_coordinate.ghostWay = getShortestPath(neighboursGraph, pacman.currentNodeDestination,
                                                                        red_ghost_coordinate.currentNodeSource,
                                                                        road, PINK);
                    }
                }
            }
        }

        if (!pink_ghost_coordinate.trapped && !pink_ghost_coordinate.escaped) {
            if (!pink_ghost_coordinate.readyToEscape) {
                setEscapePosition(pink_ghost_coordinate, ghostRoad,
                                  pink_ghost_coordinate.readyToEscape);
            } else {
                if (!pink_ghost_coordinate.readyToLeave) {
                    if (leaveTheTrap(ghostRoad, pink_ghost_coordinate,
                                     pink_ghost_coordinate.slowingGhost, pink_ghost_coordinate.readyToLeave)) {
                        escape = !escape;
                        spawnPlace--;
                    }
                } else {
                    if (escapeFromTrap(pink_ghost_coordinate, road, escape,
                                       pink_ghost_coordinate.escaped)) {
                        pink_ghost_coordinate.ghostWay = getShortestPath(neighboursGraph, pacman.currentNodeDestination,
                                                                         pink_ghost_coordinate.currentNodeSource,
                                                                         road, PINK);
                    }
                }
            }
        }

        if (!blue_ghost_coordinate.trapped && !blue_ghost_coordinate.escaped) {
            if (!blue_ghost_coordinate.readyToEscape) {
                setEscapePosition(blue_ghost_coordinate, ghostRoad,
                                  blue_ghost_coordinate.readyToEscape);
            } else {
                if (!blue_ghost_coordinate.readyToLeave) {
                    if (leaveTheTrap(ghostRoad, blue_ghost_coordinate,
                                     blue_ghost_coordinate.slowingGhost, blue_ghost_coordinate.readyToLeave)) {
                        escape = !escape;
                        spawnPlace--;
                    }
                } else {
                    if (escapeFromTrap(blue_ghost_coordinate, road, escape,
                                       blue_ghost_coordinate.escaped)) {
                        blue_ghost_coordinate.ghostWay = getShortestPath(neighboursGraph, pacman.currentNodeDestination,
                                                                         blue_ghost_coordinate.currentNodeSource,
                                                                         road, BLUE);
                    }
                }
            }
        }

        if (!yellow_ghost_coordinate.trapped && !yellow_ghost_coordinate.escaped) {
            if (!yellow_ghost_coordinate.readyToEscape) {
                setEscapePosition(yellow_ghost_coordinate, ghostRoad,
                                  yellow_ghost_coordinate.readyToEscape);
            } else {
                if (!yellow_ghost_coordinate.readyToLeave) {
                    if (leaveTheTrap(ghostRoad, yellow_ghost_coordinate,
                                     yellow_ghost_coordinate.slowingGhost, yellow_ghost_coordinate.readyToLeave)) {
                        escape = !escape;
                        spawnPlace--;
                    }
                } else {
                    if (escapeFromTrap(yellow_ghost_coordinate, road, escape,
                                       yellow_ghost_coordinate.escaped)) {
                        yellow_ghost_coordinate.ghostWay = getShortestPath(neighboursGraph,
                                                                           pacman.currentNodeDestination,
                                                                           yellow_ghost_coordinate.currentNodeSource,
                                                                           road, YELLOW);
                    }
                }
            }
        }


        if (!red_ghost_coordinate.ghostWay.empty() && !red_ghost_coordinate.dead && !red_ghost_coordinate.caught &&
            red_ghost_coordinate.escaped) {
            ghostMoving(red_ghost_coordinate.ghostWay, red_ghost_coordinate, pacman,
                        red_ghost_coordinate.addNewPath);
        } else if (red_ghost_coordinate.dead) {
            if (!red_ghost_coordinate.ghostRunAway.empty()) {
                runningAway(red_ghost_coordinate);
            } else {
                runAwayGhostMode(red_ghost_coordinate, neighboursGraph, pacman, road);
            }
        } else if (red_ghost_coordinate.caught) {
            if (!red_ghost_coordinate.roadToTrap.empty()) {
                runningToTrap(red_ghost_coordinate);
            } else {
                CellRoad destination = {};
                if (findFreePlaceInTrap(red_ghost_coordinate, ghostRoad, spawnPlace, destination, TotalScore)) {
                    int index = findSpawnPlace(destination, joinedRoad);
                    red_ghost_coordinate.roadToTrap = getShortestPath(joinedNeighboursGraph,
                                                                      index, red_ghost_coordinate.currentNodeSource,
                                                                      joinedRoad, OTHER);
                } else {
                    red_ghost_coordinate.deleted = true;
                }
            }
        }

        if (!pink_ghost_coordinate.ghostWay.empty() && !pink_ghost_coordinate.dead && !pink_ghost_coordinate.caught &&
            pink_ghost_coordinate.escaped) {
            ghostMoving(pink_ghost_coordinate.ghostWay, pink_ghost_coordinate, pacman,
                        pink_ghost_coordinate.addNewPath);
        } else if (pink_ghost_coordinate.dead) {
            if (!pink_ghost_coordinate.ghostRunAway.empty()) {
                runningAway(pink_ghost_coordinate);
            } else {
                runAwayGhostMode(pink_ghost_coordinate, neighboursGraph, pacman, road);
            }
        } else if (pink_ghost_coordinate.caught) {
            if (!pink_ghost_coordinate.roadToTrap.empty()) {
                runningToTrap(pink_ghost_coordinate);
            } else {
                CellRoad destination = {};
                if (findFreePlaceInTrap(pink_ghost_coordinate, ghostRoad, spawnPlace, destination, TotalScore)) {
                    int index = findSpawnPlace(destination, joinedRoad);
                    pink_ghost_coordinate.roadToTrap = getShortestPath(joinedNeighboursGraph,
                                                                       index, pink_ghost_coordinate.currentNodeSource,
                                                                       joinedRoad, OTHER);
                } else {
                    pink_ghost_coordinate.deleted = true;
                }
            }
        }

        if (!yellow_ghost_coordinate.ghostWay.empty() && !yellow_ghost_coordinate.dead &&
            !yellow_ghost_coordinate.caught && yellow_ghost_coordinate.escaped) {
            ghostMoving(yellow_ghost_coordinate.ghostWay, yellow_ghost_coordinate, pacman,
                        yellow_ghost_coordinate.addNewPath);
        } else if (yellow_ghost_coordinate.dead) {
            if (!yellow_ghost_coordinate.ghostRunAway.empty()) {
                runningAway(yellow_ghost_coordinate);
            } else {
                runAwayGhostMode(yellow_ghost_coordinate, neighboursGraph, pacman, road);
            }
        } else if (yellow_ghost_coordinate.caught) {
            if (!yellow_ghost_coordinate.roadToTrap.empty()) {
                runningToTrap(yellow_ghost_coordinate);
            } else {
                CellRoad destination = {};
                if (findFreePlaceInTrap(yellow_ghost_coordinate, ghostRoad, spawnPlace, destination, TotalScore)) {
                    int index = findSpawnPlace(destination, joinedRoad);
                    yellow_ghost_coordinate.roadToTrap = getShortestPath(joinedNeighboursGraph,
                                                                         index,
                                                                         yellow_ghost_coordinate.currentNodeSource,
                                                                         joinedRoad, OTHER);
                } else {
                    yellow_ghost_coordinate.deleted = true;
                }
            }
        }

        if (!blue_ghost_coordinate.ghostWay.empty() && !blue_ghost_coordinate.dead && !blue_ghost_coordinate.caught &&
            blue_ghost_coordinate.escaped) {
            ghostMoving(blue_ghost_coordinate.ghostWay, blue_ghost_coordinate, pacman,
                        blue_ghost_coordinate.addNewPath);
        } else if (blue_ghost_coordinate.dead) {
            if (!blue_ghost_coordinate.ghostRunAway.empty()) {
                runningAway(blue_ghost_coordinate);
            } else {
                runAwayGhostMode(blue_ghost_coordinate, neighboursGraph, pacman, road);
            }
        } else if (blue_ghost_coordinate.caught) {
            if (!blue_ghost_coordinate.roadToTrap.empty()) {
                runningToTrap(blue_ghost_coordinate);
            } else {
                CellRoad destination = {};
                if (findFreePlaceInTrap(blue_ghost_coordinate, ghostRoad, spawnPlace, destination, TotalScore)) {
                    int index = findSpawnPlace(destination, joinedRoad);
                    blue_ghost_coordinate.roadToTrap = getShortestPath(joinedNeighboursGraph,
                                                                       index,
                                                                       blue_ghost_coordinate.currentNodeSource,
                                                                       joinedRoad, OTHER);
                } else {
                    blue_ghost_coordinate.deleted = true;
                }
            }
        }

        if (red_ghost_coordinate.deleted || pink_ghost_coordinate.deleted || yellow_ghost_coordinate.deleted ||
            blue_ghost_coordinate.deleted) {
            if (!maxPoint) {
                TotalScore += 1600;
                maxPoint = 1;
            }
        }


        if (meetingPacmanWithGhosts(red_ghost_coordinate, yellow_ghost_coordinate,
                                    blue_ghost_coordinate,
                                    pink_ghost_coordinate, pacman, ghostRoad, startGame, direction,
                                    next_direction, respawn, spawnPlace, maxPoint) || pointsCoordinates.empty()) {
            TotalScore = 0;
            maxPoint = 0;
            pacman.lives = 2;
            respawnCharacters(red_ghost_coordinate, yellow_ghost_coordinate, blue_ghost_coordinate,
                              pink_ghost_coordinate, pacman, ghostRoad, startGame, direction, next_direction, respawn,
                              spawnPlace);
            pointsCoordinates = createPoints(road);
            bonusPointsCoordinates = createBonusPoints(pointsCoordinates, road);
        }


        pacman = tunnelSwap(pacman);
        pacmanRender.x = changeRender(pacman.x, WIDTH);
        pacmanRender.y = changeRender(pacman.y, HEIGHT);

        auto it = std::remove_if(pointsCoordinates.begin(), pointsCoordinates.end(), [&](const Cell &point) {
            bool inRangeHorizontally = (point.x > pacman.x && point.x < pacman.x + 10 && pacman.y == point.y) ||
                                       (point.x < pacman.x && point.x > pacman.x - 10 && pacman.y == point.y);
            bool inRangeVertically = (point.y > pacman.y && point.y < pacman.y + 10 && pacman.x == point.x) ||
                                     (point.y < pacman.y && point.y > pacman.y - 10 && pacman.x == point.x);

            if (inRangeHorizontally || inRangeVertically) {
                TotalScore += 10;
                return true;
            }
            return false;
        });

        pointsCoordinates.erase(it, pointsCoordinates.end());

        bool bonusEaten = false;

        auto itBonus = std::remove_if(bonusPointsCoordinates.begin(), bonusPointsCoordinates.end(),
                                      [&](const Cell &bonusPoint) {
                                          bool inRangeHorizontally =
                                                  (bonusPoint.x > pacman.x && bonusPoint.x < pacman.x + 20 && pacman.y
                                                   == bonusPoint.y) ||
                                                  (bonusPoint.x < pacman.x && bonusPoint.x > pacman.x - 20 && pacman.y
                                                   == bonusPoint.y);
                                          bool inRangeVertically =
                                                  (bonusPoint.y > pacman.y && bonusPoint.y < pacman.y + 20 && pacman.x
                                                   == bonusPoint.x) ||
                                                  (bonusPoint.y < pacman.y && bonusPoint.y > pacman.y - 20 && pacman.x
                                                   == bonusPoint.x);

                                          if (inRangeHorizontally || inRangeVertically) {
                                              TotalScore += 100;
                                              bonusEaten = true;
                                              return true;
                                          }
                                          return false;
                                      });

        bonusPointsCoordinates.erase(itBonus, bonusPointsCoordinates.end());


        if (bonusEaten) {
            setDeadAllGhosts(red_ghost_coordinate, yellow_ghost_coordinate, blue_ghost_coordinate,
                             pink_ghost_coordinate);
            t1 = chrono::high_resolution_clock::now();
        }

        if (TotalScore % 1000 == 0) {
            score_position.w = BAND_HEIGHT;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

        SDL_RenderClear(renderer);

        if (!printBoard(bounds_coordinatesRight, 33, 0, 0, 255)) return -1;
        if (!printBoard(bounds_coordinatesLeft, 33, 0, 0, 255)) return -1;
        if (!printGates(trap_gates, 255, 240, 0)) return -1;


        SDL_RenderCopy(renderer, pacman_bitmap, NULL, &pacmanRender);
        SDL_RenderCopy(renderer, score_message, NULL, &messageRender);


        renderGhosts(red_ghost_coordinate, yellow_ghost_coordinate, pink_ghost_coordinate, blue_ghost_coordinate);


        printPoint(pointsCoordinates);
        printBonusPoint(bonusPointsCoordinates);
        printLives(pacman);


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
                    if (count < 11) {
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
            if (count < 25) {
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
            if (count < 30) {
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

std::vector<Cell> createBonusPoints(std::vector<Cell> &pointsCoordinate, std::vector<CellRoad> road) {
    using namespace std;
    vector<Cell> bonusPointsCoordinate;
    vector<Cell> pointsRemoved;

    int index = 0;
    int setFirstCherry = 0;
    int setSecondCherry = 0;
    int setThirdCherry = 0;
    int removePoint = 0;

    for (auto &point: pointsCoordinate) {
        Cell bonus;

        if (point.x == 160 && point.y >= 80 && point.y <= 110) {
            if (setFirstCherry == 0) {
                bonus = point;
                pointsRemoved.push_back(bonus);
                bonusPointsCoordinate.push_back(bonus);
            }
            setFirstCherry++;
        } else if (point.x == 480 && point.y >= 80 && point.y < 140) {
            if (setThirdCherry == 0) {
                bonus = point;
                pointsRemoved.push_back(bonus);
                bonusPointsCoordinate.push_back(bonus);
            }
            setThirdCherry++;
        } else if (point.x > 480 && point.x < 640 && point.y == 80) {
            if (removePoint == 0) {
                bonus = point;
                pointsRemoved.push_back(bonus);
            }
            removePoint++;
        } else if (point.x >= 30 && point.x < 225 && point.y == 615) {
            bonus = point;
            if (setSecondCherry == 0) {
                pointsRemoved.push_back(bonus);
                bonusPointsCoordinate.push_back(bonus);
            } else if (setSecondCherry < 2) {
                pointsRemoved.push_back(bonus);
            }
            setSecondCherry++;
        } else if (point.x >= 415 && point.x <= 610 && point.y == 615) {
            if (index + 1 < static_cast<int>(pointsCoordinate.size())) {
                const auto &nextPoint = pointsCoordinate[index + 1];
                if (!(nextPoint.x >= 415 && nextPoint.x <= 610 && nextPoint.y == 615)) {
                    bonus = point;
                    pointsRemoved.push_back(bonus);
                    bonusPointsCoordinate.push_back(bonus);
                }
            } else {
                bonus = point;
                pointsRemoved.push_back(bonus);
                bonusPointsCoordinate.push_back(bonus);
            }
        }

        index++;
    }

    for (const auto &pointToRemove: pointsRemoved) {
        auto newEnd = remove_if(pointsCoordinate.begin(), pointsCoordinate.end(),
                                [&pointToRemove](const Cell &c) {
                                    return c.x == pointToRemove.x && c.y == pointToRemove.y;
                                });
        pointsCoordinate.erase(newEnd, pointsCoordinate.end());
    }

    return bonusPointsCoordinate;
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

std::vector<std::vector<CellRoad> > createGraphWithAllNeighbours(std::vector<CellRoad> &road) {
    using namespace std;

    vector<vector<CellRoad> > adjList(road.size());

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

void setGhostsPosition(std::vector<CellRoad> &ghostRoad, ghost_colors c, ghost &ghostCoordinate) {
    using namespace std;

    sort(ghostRoad.begin(), ghostRoad.end(), compareByLength);

    int count = 0;

    for (auto &road: ghostRoad) {
        switch (c) {
            case RED:
                if (count == 4) {
                    ghostCoordinate.x = road.x;
                    ghostCoordinate.y = road.y;
                }
                break;
            case YELLOW:
                if (count == 1) {
                    ghostCoordinate.x = road.x;
                    ghostCoordinate.y = road.y;
                }
                break;
            case BLUE:
                if (count == 2) {
                    ghostCoordinate.x = road.x;
                    ghostCoordinate.y = road.y;
                }
                break;
            case PINK:
                if (count == 3) {
                    ghostCoordinate.x = road.x;
                    ghostCoordinate.y = road.y;
                }
                break;
        }
        count++;
    }

    ghostCoordinate.d = DOWN;
}

void bfs(std::vector<std::vector<CellRoad> > &adjList, int startNode, std::vector<int> &par, std::vector<int> &dist,
         std::vector<CellRoad> &road, ghost_colors ghostColors) {
    CellRoad parent = road.at(startNode);

    int node = startNode;
    int modifiedNode = 0;
    int index = 0;

    switch (ghostColors) {
        case RED:
            break;
        case YELLOW:
            for (auto cell: road) {
                for (auto ghostCell: adjList[startNode]) {
                    if (cell.x == ghostCell.x && cell.y == ghostCell.y && cell.w == ghostCell.w) {
                        if (ghostCell.x > parent.x && ghostCell.x < parent.x + parent.w) {
                            node = index;
                        }
                    }
                }
                index++;
            }
            break;
        case BLUE:
            for (auto cell: road) {
                for (auto ghostCell: adjList[startNode]) {
                    if (cell.x == ghostCell.x && cell.y == ghostCell.y && cell.w == ghostCell.w) {
                        if (ghostCell.x < parent.x) {
                            node = index;
                        }
                    }
                }
                index++;
            }
            break;
        case PINK:
            for (auto cell: road) {
                for (auto ghostCell: adjList[startNode]) {
                    if (cell.x == ghostCell.x && cell.y == ghostCell.y && cell.w == ghostCell.w) {
                        if (ghostCell.x > parent.x) {
                            node = index;
                        }
                    }
                }
                index++;
            }
            break;
    }


    std::vector<int> queue;
    dist[node] = 0;
    queue.push_back(node);


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

std::vector<CellRoad> getShortestPath(std::vector<std::vector<CellRoad> > &adjList, int source, int destination,
                                      std::vector<CellRoad> &road, ghost_colors ghostColors) {
    using namespace std;

    vector<int> par(adjList.size(), -1);
    vector<int> dist(adjList.size(), 1e9);

    bfs(adjList, source, par, dist, road, ghostColors);

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

void printBonusPoint(const std::vector<Cell> &bonusPointsCoordinates) {
    using namespace std;

    for (const auto &point: bonusPointsCoordinates) {
        SDL_Rect rect;

        if (point.y == 80) {
            rect.x = point.x - 3 * BAND_WIDTH;
            rect.y = point.y - 2 * BAND_WIDTH;
        } else {
            rect.x = point.x;
            rect.y = point.y - 3 * BAND_WIDTH;
        }

        rect.w = 30;
        rect.h = 30;

        SDL_RenderCopy(renderer, bonus_point, NULL, &rect);
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

    struct Cell endBoard[4][1] = {
        {BOARD_START + BAND_HEIGHT, WINDOW_WIDTH - BAND_WIDTH},
        {8 * BAND_HEIGHT - BAND_WIDTH, WINDOW_WIDTH - BAND_WIDTH},
        {WINDOW_HEIGHT, WINDOW_WIDTH / 2},
        {BOARD_START, WINDOW_WIDTH / 2}
    };

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
        {0, 3, 0, 4},
        {1, 0, 2, 0},
        {2, 1, 2, 2},
        {0, 2, 1, 2},
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
                    band.y = (j > 2)
                                 ? previousY * 60 + BOARD_SCORE_HEIGHT
                                 : previousY * 60 +
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

    struct Cell trap[2][3] = {
        {3, 0, 4, 0, 3, 1},
        {5, 3, 6, 3, 5, 3}
    };

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
                        band.x = (j == 1)
                                     ? trap[i][j].x * 64 + WINDOW_WIDTH / 2
                                     : trap[i][j].x * 64 +
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

void stayInTrap(ghost &ghost, ghost_colors ghostColors, std::vector<CellRoad> &ghostRoad) {
    int count = 0;

    for (auto ghostPosition: ghostRoad) {
        switch (ghostColors) {
            case RED:
                break;
            case YELLOW:
                if (count == 1) {
                    if (ghost.y == ghostPosition.y) {
                        ghost.d = DOWN;
                    } else if (ghost.y == ghostPosition.y + ghostPosition.h) {
                        ghost.d = UP;
                    }
                }
                break;
            case BLUE:
                if (count == 2) {
                    if (ghost.y == ghostPosition.y) {
                        ghost.d = DOWN;
                    } else if (ghost.y == ghostPosition.y + ghostPosition.h) {
                        ghost.d = UP;
                    }
                }
                break;
            case PINK:
                if (count == 3) {
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
}

bool
leaveTheTrap(std::vector<CellRoad> &ghostRoad, ghost &ghostCoordinate, float &slowMoving,
             bool &readyToLeave) {
    using namespace std;

    CellRoad exitTrap = ghostRoad.at(4);
    bool key = false;

    for (auto road: ghostRoad) {
        if (ghostCoordinate.x == road.x && road.h < 45 && road.w == 0) {
            if (ghostCoordinate.y > road.y) {
                ghostCoordinate.d = UP;
            }
        } else if (road.w > 0) {
            if (ghostCoordinate.y == road.y) {
                if (ghostCoordinate.x < exitTrap.x) {
                    ghostCoordinate.d = RIGHT;
                } else if (ghostCoordinate.x > exitTrap.x) {
                    ghostCoordinate.d = LEFT;
                } else {
                    ghostCoordinate.d = UP;
                }
            }
        }
        /*cout << "ghost y :" << ghostCoordinate.y << "\n";
        cout << " X :" << road.x;
        cout << " Y :" << road.y;
        cout << " W :" << road.w;
        cout << " H :" << road.h << "\n";*/
    }

    switch (ghostCoordinate.d) {
        case UP:
            slowMoving += 0.5f;
            if (floorf(slowMoving) == slowMoving) {
                ghostCoordinate.y--;
                key = !key;
            }
            break;
        case DOWN:
            break;
        case LEFT:
            slowMoving += 0.5f;
            if (floorf(slowMoving) == slowMoving) {
                ghostCoordinate.x--;
                key = !key;
            }
            break;
        case RIGHT:
            slowMoving += 0.5f;
            if (floorf(slowMoving) == slowMoving) {
                ghostCoordinate.x++;
                key = !key;
            }
            break;
    }


    if (ghostCoordinate.y == exitTrap.y) {
        slowMoving = 0.0f;
        readyToLeave = !readyToLeave;
        return true;
    }


    return false;
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

void ghostMoving(std::vector<CellRoad> &ghostWay, ghost &ghostCoordinate,
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
}

bool escapeFromTrap(ghost &ghostCoordinate, std::vector<CellRoad> &road, bool &escape,
                    bool &escaped) {
    for (auto cell: road) {
        if (ghostCoordinate.x >= cell.x && ghostCoordinate.x <= cell.x + cell.w && ghostCoordinate.y == cell.y) {
            if (cell.w > 0) {
                if (escape) {
                    if (ghostCoordinate.x < cell.x + cell.w) {
                        ghostCoordinate.d = RIGHT;
                    } else {
                        escaped = !escaped;
                        return true;
                    }
                } else {
                    if (ghostCoordinate.x > cell.x) {
                        ghostCoordinate.d = LEFT;
                    } else {
                        escaped = !escaped;
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

    return false;
}

void movingInTrap(ghost &blinky, ghost &pinky, ghost &inky, ghost &clyde, std::vector<CellRoad> &ghostRoad) {
    int temp = 0;

    for (auto road: ghostRoad) {
        if (road.h < 45) {
            temp = road.y + road.h;
        }
    }


    if (blinky.trapped) {
        /*stayInTrap(blinky, RED, ghostRoad, blinkyRendering);*/
    }

    if (pinky.trapped) {
        stayInTrap(pinky, PINK, ghostRoad);
    }

    if (inky.trapped) {
        stayInTrap(inky, BLUE, ghostRoad);
    }

    if (clyde.trapped) {
        stayInTrap(clyde, YELLOW, ghostRoad);
    }
}

bool setEscapePosition(ghost &any, std::vector<CellRoad> &ghostRoad, bool &readyToEscape) {
    for (auto road: ghostRoad) {
        if (road.h < 45) {
            if (any.x >= road.x && any.x <= road.x + road.w && any.y >= road.y && any.y <= road.y + road.h) {
                if (any.y != road.y + road.h) {
                    any.d = DOWN;
                } else {
                    any.d = STOP;
                    readyToEscape = true;
                    return true;
                }
            }
        }
    }

    switch (any.d) {
        case DOWN:
            any.y++;
            break;
        case STOP:
            break;
    }

    return false;
}

void increaseCountGhosts(int totalScore, ghost &blinky, ghost &pinky, ghost &inky, ghost &clyde) {
    if (totalScore >= 300 && totalScore < 400 && pinky.trapped) {
        pinky.trapped = false;
    } else if (totalScore >= 400 && totalScore < 500 && inky.trapped) {
        inky.trapped = false;
    } else if (totalScore >= 500 && totalScore < 600 && clyde.trapped) {
        clyde.trapped = false;
    }
}

void setDeadAllGhosts(ghost &red, ghost &yellow, ghost &blue, ghost &pink) {
    if (!red.trapped && red.escaped && !red.caught) {
        red.dead = true;
    }

    if (!yellow.trapped && yellow.escaped && !yellow.caught) {
        yellow.dead = true;
    }

    if (!blue.trapped && blue.escaped && !blue.caught) {
        blue.dead = true;
    }

    if (!pink.trapped && pink.escaped && !pink.caught) {
        pink.dead = true;
    }
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

void renderGhosts(ghost &redCoordinate, ghost &yelloCoordinate,
                  ghost &pinkCoordinate, ghost &blueCoordinate) {
    SDL_Rect redRender = {changeRender(redCoordinate.x, WIDTH), changeRender(redCoordinate.y, HEIGHT), 30, 30};
    SDL_Rect yellowRender = {changeRender(yelloCoordinate.x, WIDTH), changeRender(yelloCoordinate.y, HEIGHT), 30, 30};
    SDL_Rect blueRender = {changeRender(blueCoordinate.x, WIDTH), changeRender(blueCoordinate.y, HEIGHT), 30, 30};
    SDL_Rect pinkRender = {changeRender(pinkCoordinate.x, WIDTH), changeRender(pinkCoordinate.y, HEIGHT), 30, 30};


    if (!redCoordinate.deleted) {
        if (redCoordinate.dead) {
            if (floorf(redCoordinate.printNextTexture) == redCoordinate.printNextTexture) {
                SDL_RenderCopy(renderer, dead_ghostV2, NULL, &redRender);
            } else {
                SDL_RenderCopy(renderer, dead_ghost, NULL, &redRender);
            }
        } else if (redCoordinate.caught) {
            SDL_RenderCopy(renderer, ghost_eyes, NULL, &redRender);
        } else {
            SDL_RenderCopy(renderer, red_ghost, NULL, &redRender);
        }
    }

    if (!yelloCoordinate.deleted) {
        if (yelloCoordinate.dead) {
            if (floorf(yelloCoordinate.printNextTexture) == yelloCoordinate.printNextTexture) {
                SDL_RenderCopy(renderer, dead_ghostV2, NULL, &yellowRender);
            } else {
                SDL_RenderCopy(renderer, dead_ghost, NULL, &yellowRender);
            }
        } else if (yelloCoordinate.caught) {
            SDL_RenderCopy(renderer, ghost_eyes, NULL, &yellowRender);
        } else {
            SDL_RenderCopy(renderer, yellow_ghost, NULL, &yellowRender);
        }
    }

    if (!blueCoordinate.deleted) {
        if (blueCoordinate.dead) {
            if (floorf(blueCoordinate.printNextTexture) == blueCoordinate.printNextTexture) {
                SDL_RenderCopy(renderer, dead_ghostV2, NULL, &blueRender);
            } else {
                SDL_RenderCopy(renderer, dead_ghost, NULL, &blueRender);
            }
        } else if (blueCoordinate.caught) {
            SDL_RenderCopy(renderer, ghost_eyes, NULL, &blueRender);
        } else {
            SDL_RenderCopy(renderer, blue_ghost, NULL, &blueRender);
        }
    }

    if (!pinkCoordinate.deleted) {
        if (pinkCoordinate.dead) {
            if (floorf(pinkCoordinate.printNextTexture) == pinkCoordinate.printNextTexture) {
                SDL_RenderCopy(renderer, dead_ghostV2, NULL, &pinkRender);
            } else {
                SDL_RenderCopy(renderer, dead_ghost, NULL, &pinkRender);
            }
        } else if (pinkCoordinate.caught) {
            SDL_RenderCopy(renderer, ghost_eyes, NULL, &pinkRender);
        } else {
            SDL_RenderCopy(renderer, pink_ghost, NULL, &pinkRender);
        }
    }
}


void
runAwayGhostMode(ghost &ghostCoordinate, std::vector<std::vector<CellRoad> > &neighboursRoad, pacman &pacmanCoordinate,
                 std::vector<CellRoad> &road) {
    using namespace std;

    random_device rd;
    mt19937 rdgen(rd());


    int currentNodeSourceGhost = ghostCoordinate.currentNodeSource;


    ghostCoordinate.ghostRunAway.push_back(road.at(currentNodeSourceGhost));

    const int maxAttempts = 10;
    const int stepsToChoose = 5;

    for (int i = 0; i < stepsToChoose; ++i) {
        bool foundNext = false;
        int attempts = 0;

        while (!foundNext && attempts < maxAttempts) {
            ++attempts;

            if (neighboursRoad[currentNodeSourceGhost].empty())
                break;

            uniform_int_distribution<size_t> d{0, neighboursRoad[currentNodeSourceGhost].size() - 1};
            CellRoad nextNeighbour = neighboursRoad[currentNodeSourceGhost][d(rdgen)];


            auto it = std::find_if(road.begin(), road.end(), [&nextNeighbour](const CellRoad &c) {
                return c.x == nextNeighbour.x && c.y == nextNeighbour.y && c.w == nextNeighbour.w;
            });

            if (it != road.end()) {
                int indexInRoad = std::distance(road.begin(), it);

                if (indexInRoad != pacmanCoordinate.currentNodeDestination) {
                    if (std::none_of(ghostCoordinate.ghostRunAway.begin(), ghostCoordinate.ghostRunAway.end(),
                                     [&nextNeighbour](const CellRoad &c) {
                                         return c.x == nextNeighbour.x && c.y == nextNeighbour.y && c.w == nextNeighbour
                                                .w;
                                     })) {
                        ghostCoordinate.ghostRunAway.push_back(nextNeighbour);
                        currentNodeSourceGhost = indexInRoad;
                        foundNext = true;
                    }
                }
            }
        }

        if (!foundNext) {
            break;
        }
    }

    // int count = 0;
    // int key = true;

    // CellRoad nextNeighbour = {};

    // for (int i = 0; i < 5; ++i) {
    //     key = true;
    //
    //     while (key) {
    //         count = 0;
    //         uniform_int_distribution<size_t> d{0, neighboursRoad[currentNodeSourceGhost].size() - 1};
    //
    //         nextNeighbour = neighboursRoad[currentNodeSourceGhost][d(rdgen)];
    //
    //         for (auto &cellRoad: road) {
    //             if (nextNeighbour.x == cellRoad.x && nextNeighbour.y == cellRoad.y && nextNeighbour.w == cellRoad.w) {
    //                 if (count != pacmanCoordinate.currentNodeDestination) {
    //                     if (std::find_if(ghostCoordinate.ghostRunAway.begin(), ghostCoordinate.ghostRunAway.end(),
    //                                      [&cr = cellRoad](const CellRoad &c) {
    //                                          return cr.x == c.x && cr.y == c.y && c.w == cr.w;
    //                                      }) != ghostCoordinate.ghostRunAway.end()) {
    //                     } else {
    //                         ghostCoordinate.ghostRunAway.push_back(nextNeighbour);
    //                         currentNodeSourceGhost = count;
    //                         key = false;
    //                     }
    //                 }
    //             }
    //             count++;
    //         }
    //     }
    // }
}

void runningAway(ghost &ghostCoordinate) {
    CellRoad currentRoad = {};
    CellRoad nextRoad = {};

    if (ghostCoordinate.ghostRunAway.size() > 1) {
        currentRoad = ghostCoordinate.ghostRunAway.front();
        nextRoad = ghostCoordinate.ghostRunAway[1];

        if (currentRoad.w > 0) {
            if (currentRoad.x <= ghostCoordinate.x && currentRoad.x + currentRoad.w >= ghostCoordinate.x) {
                if (nextRoad.x > ghostCoordinate.x) {
                    ghostCoordinate.d = RIGHT;
                } else if (nextRoad.x < ghostCoordinate.x) {
                    ghostCoordinate.d = LEFT;
                }

                if (ghostCoordinate.x == nextRoad.x) {
                    ghostCoordinate.ghostRunAway.erase(
                        remove_if(ghostCoordinate.ghostRunAway.begin(), ghostCoordinate.ghostRunAway.end(),
                                  [currentRoad](const CellRoad &c) {
                                      return c.x == currentRoad.x && c.y == currentRoad.y && c.w == currentRoad.w;
                                  }));
                    ghostCoordinate.d = STOP;
                }
            }
        } else {
            if (currentRoad.y <= ghostCoordinate.y && currentRoad.y + currentRoad.h >= ghostCoordinate.y) {
                if (nextRoad.y < ghostCoordinate.y) {
                    ghostCoordinate.d = UP;
                } else if (nextRoad.y > ghostCoordinate.y) {
                    ghostCoordinate.d = DOWN;
                }

                if (ghostCoordinate.y == nextRoad.y) {
                    ghostCoordinate.ghostRunAway.erase(
                        remove_if(ghostCoordinate.ghostRunAway.begin(), ghostCoordinate.ghostRunAway.end(),
                                  [currentRoad](const CellRoad &c) {
                                      return c.x == currentRoad.x && c.y == currentRoad.y && c.w == currentRoad.w;
                                  }));
                    ghostCoordinate.d = STOP;
                }
            }
        }
    } else {
        /*currentRoad = ghostCoordinate.ghostRunAway.front();*/
        ghostCoordinate.ghostRunAway.clear();
        /*ghostCoordinate.ghostRunAway.erase(
                remove_if(ghostCoordinate.ghostRunAway.begin(), ghostCoordinate.ghostRunAway.end(),
                          [currentRoad](const CellRoad &c) {
                              return c.x == currentRoad.x && c.y == currentRoad.y && c.w == currentRoad.w;
                          }));*/
    }

    ghostCoordinate.slowingGhost += 0.5f;

    switch (ghostCoordinate.d) {
        case UP:
            if (floorf(ghostCoordinate.slowingGhost) == ghostCoordinate.slowingGhost) {
                ghostCoordinate.y--;
            }
            break;
        case DOWN:
            if (floorf(ghostCoordinate.slowingGhost) == ghostCoordinate.slowingGhost) {
                ghostCoordinate.y++;
            }
            break;
        case LEFT:
            if (floorf(ghostCoordinate.slowingGhost) == ghostCoordinate.slowingGhost) {
                ghostCoordinate.x--;
            }
            break;
        case RIGHT:
            if (floorf(ghostCoordinate.slowingGhost) == ghostCoordinate.slowingGhost) {
                ghostCoordinate.x++;
            }
            break;
        case STOP:
            break;
    }
}

bool
meetingPacmanWithGhosts(ghost &redCoordinate, ghost &yellowCoordinate, ghost &blueCoordinate, ghost &pinkCoordinate,
                        pacman &pacmanCoordinate, std::vector<CellRoad> &ghostRoad, bool &startGame, int &direction,
                        int &nextDirection, bool &respawn, int &spawnPlace, int &maxPoint) {
    if (pacmanCoordinate.x == redCoordinate.x && pacmanCoordinate.y == redCoordinate.y) {
        if (redCoordinate.dead) {
            redCoordinate.slowingGhost = 0.0f;
            redCoordinate.dead = false;
            redCoordinate.caught = true;
        } else if (!redCoordinate.dead && !redCoordinate.caught) {
            if (pacmanCoordinate.lives != 0) {
                pacmanCoordinate.lives--;
                respawnCharacters(redCoordinate, yellowCoordinate,
                                  blueCoordinate, pinkCoordinate,
                                  pacmanCoordinate, ghostRoad, startGame, direction, nextDirection, respawn,
                                  spawnPlace);
                maxPoint = 0;
            } else {
                return true;
            }
        }
    }

    if (pacmanCoordinate.x == yellowCoordinate.x && pacmanCoordinate.y == yellowCoordinate.y) {
        if (yellowCoordinate.dead) {
            yellowCoordinate.slowingGhost = 0.0f;
            yellowCoordinate.dead = false;
            yellowCoordinate.caught = true;
        } else if (!yellowCoordinate.dead && !yellowCoordinate.caught) {
            if (pacmanCoordinate.lives != 0) {
                pacmanCoordinate.lives--;
                respawnCharacters(redCoordinate, yellowCoordinate,
                                  blueCoordinate, pinkCoordinate,
                                  pacmanCoordinate, ghostRoad, startGame, direction, nextDirection, respawn,
                                  spawnPlace);
                maxPoint = 0;
            } else {
                return true;
            }
        }
    }

    if (pacmanCoordinate.x == blueCoordinate.x && pacmanCoordinate.y == blueCoordinate.y) {
        if (blueCoordinate.dead) {
            blueCoordinate.slowingGhost = 0.0f;
            blueCoordinate.dead = false;
            blueCoordinate.caught = true;
        } else if (!blueCoordinate.dead && !blueCoordinate.caught) {
            if (pacmanCoordinate.lives != 0) {
                pacmanCoordinate.lives--;
                respawnCharacters(redCoordinate, yellowCoordinate,
                                  blueCoordinate, pinkCoordinate,
                                  pacmanCoordinate, ghostRoad, startGame, direction, nextDirection, respawn,
                                  spawnPlace);
                maxPoint = 0;
            } else {
                return true;
            }
        }
    }

    if (pacmanCoordinate.x == pinkCoordinate.x && pacmanCoordinate.y == pinkCoordinate.y) {
        if (pinkCoordinate.dead) {
            pinkCoordinate.slowingGhost = 0.0f;
            pinkCoordinate.dead = false;
            pinkCoordinate.caught = true;
        } else if (!pinkCoordinate.dead && !pinkCoordinate.caught) {
            if (pacmanCoordinate.lives != 0) {
                pacmanCoordinate.lives--;
                respawnCharacters(redCoordinate, yellowCoordinate,
                                  blueCoordinate, pinkCoordinate,
                                  pacmanCoordinate, ghostRoad, startGame, direction, nextDirection, respawn,
                                  spawnPlace);
                maxPoint = 0;
            } else {
                return true;
            }
        }
    }

    return false;
}

bool
findFreePlaceInTrap(ghost &ghostCoordinate, std::vector<CellRoad> &ghostRoad, int &spawnPlace, CellRoad &freePlace,
                    int &total_score) {
    int count = 0;

    for (auto road: ghostRoad) {
        if (spawnPlace < 1) {
            if (count == 1) {
                freePlace = road;
                spawnPlace++;
                total_score += 200;
                return true;
            }
        } else if (spawnPlace < 2) {
            if (count == 2) {
                freePlace = road;
                spawnPlace++;
                total_score += 400;
                return true;
            }
        } else if (spawnPlace < 3) {
            if (count == 3) {
                freePlace = road;
                spawnPlace++;
                total_score += 800;
                return true;
            }
        }
        count++;
    }

    return false;
}

size_t findSpawnPlace(CellRoad &destination, std::vector<CellRoad> &joinedRoad) {
    size_t index = 0;

    auto d = std::find_if(joinedRoad.begin(), joinedRoad.end(), [&cl = destination](const CellRoad &c) {
        return c.x == cl.x && c.y == cl.y && c.w == cl.w;
    });

    index = std::distance(joinedRoad.begin(), d);

    return index;
}

void runningToTrap(ghost &ghostCoordinate) {
    CellRoad lastElement = ghostCoordinate.roadToTrap.front();
    CellRoad nextElement = {};


    if (ghostCoordinate.roadToTrap.size() > 1) {
        nextElement = ghostCoordinate.roadToTrap[1];

        if (lastElement.w > 0) {
            if (lastElement.x <= ghostCoordinate.x && lastElement.x + lastElement.w >= ghostCoordinate.x) {
                if (nextElement.x > ghostCoordinate.x) {
                    ghostCoordinate.d = RIGHT;
                } else if (nextElement.x < ghostCoordinate.x) {
                    ghostCoordinate.d = LEFT;
                }

                if (ghostCoordinate.x == nextElement.x) {
                    ghostCoordinate.roadToTrap.erase(ghostCoordinate.roadToTrap.begin());
                    ghostCoordinate.d = STOP;
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
                    ghostCoordinate.roadToTrap.erase(ghostCoordinate.roadToTrap.begin());
                    ghostCoordinate.d = STOP;
                }
            }
        }
    } else {
        if (lastElement.h > 0) {
            if (ghostCoordinate.y < lastElement.y + lastElement.h) {
                ghostCoordinate.d = DOWN;
            } else {
                ghostCoordinate.d = STOP;
                clearAttributesBackToTrap(ghostCoordinate);
                clearAllRoads(ghostCoordinate);
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
}

void clearAllRoads(ghost &ghostCoordinate) {
    ghostCoordinate.ghostRunAway.clear();
    ghostCoordinate.ghostWay.clear();
    ghostCoordinate.roadToTrap.clear();
}

void clearAttributesBackToTrap(ghost &ghostCoordinate) {
    ghostCoordinate.readyToEscape = false;
    ghostCoordinate.readyToLeave = false;
    ghostCoordinate.escaped = false;
    ghostCoordinate.caught = false;
}

void clearAttributesRespawnCharacters(ghost &ghostCoordinate) {
    ghostCoordinate.d = STOP;
    ghostCoordinate.trapped = true;
    ghostCoordinate.caught = false;
    ghostCoordinate.addNewPath = false;
    ghostCoordinate.readyToEscape = false;
    ghostCoordinate.readyToLeave = false;
    ghostCoordinate.escaped = false;
    ghostCoordinate.deleted = false;
    clearAllRoads(ghostCoordinate);
}

void respawnCharacters(ghost &redCoordinate, ghost &yellowCoordinate,
                       ghost &blueCoordinate,
                       ghost &pinkCoordinate,
                       pacman &pacmanCoordinate, std::vector<CellRoad> &ghostRoad, bool &startGame, int &direction,
                       int &nextDirection, bool &respawn, int &spawnPlace) {
    pacmanCoordinate.x = PACMAN_RENDER_X;
    pacmanCoordinate.y = PACMAN_RENDER_Y;
    respawn = true;
    spawnPlace = 3;
    direction = 0;
    nextDirection = 0;
    setGhostsPosition(ghostRoad, RED, redCoordinate);
    clearAttributesRespawnCharacters(redCoordinate);
    setGhostsPosition(ghostRoad, YELLOW, yellowCoordinate);
    clearAttributesRespawnCharacters(yellowCoordinate);
    setGhostsPosition(ghostRoad, PINK, pinkCoordinate);
    clearAttributesRespawnCharacters(pinkCoordinate);
    setGhostsPosition(ghostRoad, BLUE, blueCoordinate);
    clearAttributesRespawnCharacters(blueCoordinate);
    startGame = false;
}

void printLives(pacman &pacmanCoordinate) {
    int temp = (BOARD_SCORE_HEIGHT - 30) / 2;

    if (pacmanCoordinate.lives == 2) {
        SDL_Rect live1;
        SDL_Rect live2;
        live1 = {WINDOW_WIDTH - 50, temp, 30, 30};
        live2 = {WINDOW_WIDTH - 90, temp, 30, 30};
        SDL_RenderCopy(renderer, pacman_bitmap, NULL, &live1);
        SDL_RenderCopy(renderer, pacman_bitmap, NULL, &live2);
    } else if (pacmanCoordinate.lives == 1) {
        SDL_Rect live1;
        live1 = {WINDOW_WIDTH - 30, temp, 30, 30};
        SDL_RenderCopy(renderer, pacman_bitmap, NULL, &live1);
    }
}

bool load() {
    using namespace std;

    SDL_Surface *temp1;
    temp1 = SDL_LoadBMP("../assets/Pacman.bmp");
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
    temp2 = SDL_LoadBMP("../assets/Point.bmp");
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


    Sans = TTF_OpenFont("../assets/Roboto-Italic.ttf", 24);
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
    temp4 = SDL_LoadBMP("../assets/Yellow_Ghost.bmp");
    if (!temp4) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }
    yellow_ghost = SDL_CreateTextureFromSurface(renderer, temp4);
    SDL_FreeSurface(temp4);


    SDL_Surface *temp5;
    temp5 = SDL_LoadBMP("../assets/Red_Ghost.bmp");
    if (!temp5) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }
    red_ghost = SDL_CreateTextureFromSurface(renderer, temp5);
    SDL_FreeSurface(temp5);


    SDL_Surface *temp6;
    temp6 = SDL_LoadBMP("../assets/Pink_Ghost.bmp");
    if (!temp6) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }
    pink_ghost = SDL_CreateTextureFromSurface(renderer, temp6);
    SDL_FreeSurface(temp6);


    SDL_Surface *temp7;
    temp7 = SDL_LoadBMP("../assets/Blue_Ghost.bmp");
    if (!temp7) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }
    blue_ghost = SDL_CreateTextureFromSurface(renderer, temp7);
    SDL_FreeSurface(temp7);


    SDL_Surface *temp8;
    temp8 = SDL_LoadBMP("../assets/Cherry.bmp");
    if (!temp8) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }
    bonus_point = SDL_CreateTextureFromSurface(renderer, temp8);
    SDL_FreeSurface(temp8);


    SDL_Surface *temp9;
    temp9 = SDL_LoadBMP("../assets/Dead_Ghost.bmp");
    if (!temp9) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }
    dead_ghost = SDL_CreateTextureFromSurface(renderer, temp9);
    SDL_FreeSurface(temp9);


    SDL_Surface *temp10;
    temp10 = SDL_LoadBMP("../assets/Dead_GhostV2.bmp");
    if (!temp10) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }
    dead_ghostV2 = SDL_CreateTextureFromSurface(renderer, temp10);
    SDL_FreeSurface(temp10);

    SDL_Surface *temp11;
    temp11 = SDL_LoadBMP("../assets/Ghost_Eyes.bmp");
    if (!temp11) {
        cout << "Error loading image: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }
    ghost_eyes = SDL_CreateTextureFromSurface(renderer, temp11);
    SDL_FreeSurface(temp11);

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
    SDL_DestroyTexture(ghost_eyes);
    SDL_DestroyTexture(dead_ghostV2);
    SDL_DestroyTexture(dead_ghost);
    SDL_DestroyTexture(bonus_point);
    SDL_DestroyTexture(pacman_bitmap);
    SDL_DestroyTexture(yellow_ghost);
    SDL_DestroyTexture(red_ghost);
    SDL_DestroyTexture(pink_ghost);
    SDL_DestroyTexture(blue_ghost);
    SDL_DestroyTexture(point_bitmap);
    SDL_DestroyTexture(score_message);
    SDL_DestroyTexture(scoreCount);
    delete (Sans);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
