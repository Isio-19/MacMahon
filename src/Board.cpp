#include "../include/Board.h"

bool globalSolution = false;
Tile** globalBoard;
int globalWidth;
int globalHeight;
std::stack<Tile> tileStack;
std::mutex accessingStack;

Board::Board() {}

Board::Board(std::string filename) {
    std::string fileContent;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Couldn't open requested file" << std::endl;
        return;
    }

    std::getline(file, fileContent);
    std::string delimiter = " ";

    int pos = fileContent.find(delimiter);
    width = stoi(fileContent.substr(0, pos));
    fileContent.erase(0, pos+1);
    height = stoi(fileContent);

    board = new Tile*[height];
    for (int i=0; i<height; i++) {
        board[i] = new Tile[width];

        for (int j=0; j<width; j++)
            board[i][j] = Tile();
    }

    sizeArray = width*height;
    tilesToBePlaced = new Tile[sizeArray];

    int index = 0;

    while (std::getline(file, fileContent)) {
        char left, top, right, bottom;
        left = fileContent.at(0);
        top = fileContent.at(2);
        right = fileContent.at(4);
        bottom = fileContent.at(6);
        
        Tile tile(left, top, right, bottom, index);

        tilesToBePlaced[index++] = tile;
    }

    file.close();

    globalSolution = false;
    globalWidth = width;
    globalHeight = height;

    globalBoard = new Tile*[globalHeight];
    for (int i=0; i<globalHeight; i++) {
        globalBoard[i] = new Tile[globalWidth];
        for (int j=0; j<globalWidth; j++) {
            globalBoard[i][j] = Tile();
        }
    }
}

Board::Board(Board &boardToCopy) {
    Tile** boardCopy = boardToCopy.getBoard();

    width = boardToCopy.getWidth();
    height = boardToCopy.getHeight();

    board = new Tile*[height];
    for (int i=0; i<height; i++) {
        board[i] = new Tile[width];

        for (int j=0; j<width; j++)
            board[i][j] = Tile();
    }

    for (int i=0; i<height; i++)
        for (int j=0; j<width; j++)
            board[i][j] = boardCopy[i][j];

    Tile* tilesToCopy = boardToCopy.getTilesToPlace();

    sizeArray = boardToCopy.getSizeArray();
    tilesToBePlaced = new Tile[width*height];

    Tile* arrayToCopy = boardToCopy.getTilesToPlace();

    for (int i=0; i<sizeArray; i++)
        tilesToBePlaced[i] = arrayToCopy[i];
}

Board::~Board() {
    for (int i=0; i<height; i++)
        delete board[i];
    delete board;

    delete tilesToBePlaced;
}

int Board::getWidth() { return width; }

int Board::getHeight() { return height; }

Tile** Board::getBoard() { return board; }

Tile* Board::getTilesToPlace() { return tilesToBePlaced; }

int Board::getSizeArray() { return sizeArray; }

bool Board::getSolution() { return solutionFound; }

bool Board::checkColor(Tile tile, int x, int y) {
    if (x == 0 and y == 0) {
        if (tile.getLeftColor() != tile.getTopColor())
            return false;

        boardEdgeColor = tile.getLeftColor();
    }   

    if (x == 0)
        if (tile.getLeftColor() != boardEdgeColor)
            return false;

    if (y == 0)
        if (tile.getTopColor() != boardEdgeColor)
            return false;
    
    if (x == width-1)
        if (tile.getRightColor() != boardEdgeColor)
            return false;

    if (y == height-1)
        if (tile.getBottomColor() != boardEdgeColor)
            return false;

    if (x != 0)
        if (tile.getLeftColor() != board[y][x-1].getRightColor())
            return false;
    
    if (y != 0)
        if (tile.getTopColor() != board[y-1][x].getBottomColor())
            return false;

    // TODO: do the other directions if perimeter optimisation

    return true;
}

bool Board::solveBoardSequential(int x, int y) {
    if (sizeArray == 0) {
        globalSolution = true;

        for (int i=0; i<height; i++) 
            for (int j=0; j<width; j++) 
                globalBoard[i][j] = board[i][j];

        return true;
    }

    // a solution has been found else where, hence the false
    if (globalSolution)
        return false;

    std::vector<std::string> tileChecked; 
    
    for (int testTile=0; testTile<sizeArray; testTile++) {
        Tile currentTile = tilesToBePlaced[testTile];

        std::string tileColor = currentTile.getColors();
        bool tileUsed = false;

        for (int i=0; i<tileChecked.size(); i++)
            if (tileColor == tileChecked[i]) {
                tileUsed = true;
                break;
            }

        if (tileUsed)
            continue;

        tileChecked.push_back(tileColor);

        // set and check perimeter color
        if (!checkColor(currentTile, x, y))
            continue;
        
        board[y][x] = currentTile;

        // delete currentTile from tileToBePlaced
        sizeArray--;
        for (int i=testTile; i<sizeArray; i++)
            tilesToBePlaced[i] = tilesToBePlaced[i+1];
         
        int newX = x+1, newY = y;
        if (newX == width) {
            newX = 0;
            newY++;
        }

        // if the branches with currentTile arent't valid, put back currentTile in tilesToBePlaced
        if (solveBoardSequential(newX, newY)) 
            return true;

        sizeArray++;
        for (int i=sizeArray-1; i>testTile; i--)
            tilesToBePlaced[i] = tilesToBePlaced[i-1];
        
        tilesToBePlaced[testTile] = currentTile;
    }
    return false;
}

// this function is used for ThreadPools and ForLoops
bool Board::solveBoardSequential(int index) {
    // a solution has been found else where, hence the false
    if (globalSolution)
        return false;

    Tile tile = tilesToBePlaced[index]; 

    if (tile.getLeftColor() != tile.getTopColor())
        return false;

    boardEdgeColor = tile.getLeftColor();

    sizeArray--;
    for (int i=index; i<sizeArray; i++)
        tilesToBePlaced[i] = tilesToBePlaced[i+1];
    
    board[0][0] = tile;

    if (solveBoardSequential(1, 0))
        return true;

    sizeArray++;
    for (int i=sizeArray-1; i>index; i--)
        tilesToBePlaced[i] = tilesToBePlaced[i-1];
    
    tilesToBePlaced[index] = tile;

    return false;
}

void Board::operator()(int type, int index) {
    if (type == 0) {
        this->solveBoardSequential(index);
        return;
    } if (type == 1) {
        this->ThreadPoolFunction();
        return;
    }
}

bool Board::ThreadPoolFunction() {
    while (!tileStack.empty()) {
        // solution found elsewhere
        if (globalSolution)
            return false;

        accessingStack.lock();
        Tile tileToTest = tileStack.top();
        tileStack.pop();
        accessingStack.unlock();

        if (solveBoardSequential(tileToTest.getId())) {
            return true;
        }
    }

    // explored every tiles
    return false;
}

bool Board::solveBoardThreadPool(int nbThread) {
    for (int i=0; i<sizeArray; i++)
        tileStack.push(tilesToBePlaced[i]);

    std::vector<std::thread> threadsToLaunch;
    for (int i=0; i<nbThread; i++) {
        Board copyBoard(*this);
        std::thread th(copyBoard, 1, NULL);
        threadsToLaunch.push_back(std::move(th));
    }
    
    for (std::thread &thread : threadsToLaunch) 
        thread.join();

    if (!globalSolution) {
        // clear the stack for next calls of the function
        while (!tileStack.empty())
            tileStack.pop();
    
        return false;
    }

    for (int i=0; i<globalHeight; i++)
        for (int j=0; j<globalWidth; j++)
            board[i][j] = globalBoard[i][j];

    return true;
}

bool Board::solveBoardForLoop() {
    std::vector<std::thread> threadVector;

    for(int i=0; i<sizeArray; i++) {
        Board copyBoard(*this);
        std::thread th(copyBoard, 0, i);
        threadVector.push_back(std::move(th));
    }

    for (std::thread &thread : threadVector)
        thread.join();

    if (!globalSolution)
        return false;

    for (int i=0; i<globalHeight; i++)
        for (int j=0; j<globalWidth; j++)
            board[i][j] = globalBoard[i][j];

    return true;
}

void printSpecialChar(int type, int length = 1) {
    std::map<int, wchar_t> converter;
    converter[0] = ' ';
    converter[1] = L'\u2500';      // "─"
    converter[2] = L'\u2502';      // "│"
    converter[3] = L'\u250C';      // "┌"
    converter[4] = L'\u2510';      // "┐"
    converter[5] = L'\u2514';      // "└"
    converter[6] = L'\u2518';      // "┘"
    converter[7] = L'\u251C';      // "├"
    converter[8] = L'\u2524';      // "┤"
    converter[9] = L'\u252C';      // "┬"
    converter[10] = L'\u2534';     // "┴"
    converter[11] = L'\u253C';     // "┼"

    setlocale(LC_CTYPE, "");
    // _setmode(_fileno(stdout), _O_U16TEXT);

    for (int i=0; i<length; i++)
        std::wcout << converter.find(type)->second;

    // _setmode(_fileno(stdout), _O_TEXT);
    return;
}

std::string terminalCharacterString(char color) {
    std::map<char, int> colorMap;
    colorMap['R'] = 41;
    colorMap['G'] = 42;
    colorMap['B'] = 44;

    std::string returnString;
    int terminalColor = colorMap.find(color)->second;

    returnString += "\033[";
    returnString += std::to_string(terminalColor);
    returnString += "m"; 
    returnString += color ;
    returnString += "\033[0m";

    return returnString;
}

int Board::printBoardTerminal() {
    // must be odd and >= 3
    int sizeTile = 9;

    setlocale(LC_CTYPE, "");
    // _setmode(_fileno(stdout), _O_U16TEXT);

    //beginning line
    printSpecialChar(3);
    for (int nbTile=0; nbTile<width; nbTile++) {
        if (nbTile != 0)
            printSpecialChar(9);

        printSpecialChar(1, sizeTile);
    }
    printSpecialChar(4);
    std::cout << std::endl;

    // _setmode(_fileno(stdout), _O_TEXT);

    // tile content
    for (int i=0; i<height; i++) {
        for (int line=0; line<3; line++) {
            for (int j=0; j<width; j++) {
                printSpecialChar(2);

                if (line == 0) {
                    //top line
                    printSpecialChar(0, (sizeTile-1)/2);   
                    std::cout << terminalCharacterString(board[i][j].getTopColor());
                    printSpecialChar(0, (sizeTile-1)/2);   
                } else if (line == 1) {
                    //middle line
                    std::cout << terminalCharacterString(board[i][j].getLeftColor());
                    printSpecialChar(0, sizeTile-2);
                    std::cout << terminalCharacterString(board[i][j].getRightColor());
                } else {
                    //bottom line
                    printSpecialChar(0, (sizeTile-1)/2);   
                    std::cout << terminalCharacterString(board[i][j].getBottomColor());
                    printSpecialChar(0, (sizeTile-1)/2);   
                }
            }
            printSpecialChar(2);
            std::cout << std::endl;
        }

        // between tiles
        if (i != height-1) {
            printSpecialChar(7);
            for (int nbTile=0; nbTile<width; nbTile++) {
                if (nbTile != 0)
                    printSpecialChar(11);

                printSpecialChar(1, sizeTile);
            }
            printSpecialChar(8);
            std::cout << std::endl;
        }
    }
    
    // end line
    printSpecialChar(5);
    for (int nbTile=0; nbTile<width; nbTile++) {
        if (nbTile != 0)
            printSpecialChar(10);

        printSpecialChar(1, sizeTile);
    }

    printSpecialChar(6);
    std::cout << std::endl;
    return 0;
}

int Board::printBoard() {
    SDL_Init( SDL_INIT_EVERYTHING );
    
    const int TILE_SIZE = 101;
    const int WIDTH = width*TILE_SIZE, HEIGHT = height*TILE_SIZE;
    SDL_Window *window = SDL_CreateWindow("Affichage du résultat de l'algorithme", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL) {
        std::cout << "Something went wrong:" << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Event windowEvent;
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            if (SDL_QUIT == windowEvent.type)
                break;
        }

        std::vector<std::vector<SDL_Vertex>> vertex;
        std::vector<std::vector<int>> pointCoordinates;
        SDL_Rect outline;

        SDL_RenderClear(renderer);

        for (int i=0; i<height; i++) {
            for (int j=0; j<width; j++) {
                Tile currentTile = board[i][j];

                vertex = currentTile.getVertex((float) j, (float) i, (float) TILE_SIZE);
                pointCoordinates = currentTile.getLines(j, i, TILE_SIZE);
                outline = currentTile.getOutline(j, i, TILE_SIZE);

                // color of the tile
                for (int it=0; it<vertex.size(); it++)
                    SDL_RenderGeometry(renderer, NULL, vertex[it].data(), vertex[it].size(), NULL, 0);

                // crossing lines of the tile
                SDL_RenderDrawLine(renderer, pointCoordinates[0][0], pointCoordinates[0][1], pointCoordinates[0][2], pointCoordinates[0][3]);
                SDL_RenderDrawLine(renderer, pointCoordinates[1][0], pointCoordinates[1][1], pointCoordinates[1][2], pointCoordinates[1][3]);

                // // outline of the tile
                SDL_RenderDrawRect(renderer, &outline);
            } 
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}