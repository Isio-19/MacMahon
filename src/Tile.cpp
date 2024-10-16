#include "../include/Tile.h"

Tile::Tile() {}

Tile::Tile(char left, char top, char right, char bottom, int id) {
    leftColor = left;
    topColor = top;
    rightColor = right;
    bottomColor = bottom;
    tileId = id;
}

char Tile::getLeftColor() { return leftColor; }

char Tile::getTopColor() { return topColor; }

char Tile::getRightColor() { return rightColor; }

char Tile::getBottomColor() { return bottomColor; }

void Tile::setLeftColor(char color) { leftColor = color; }

void Tile::setTopColor(char color) { topColor = color; }

void Tile::setRightColor(char color) { rightColor = color; }

void Tile::setBottomColor(char color) { bottomColor = color; }

void Tile::setId(int number) { tileId = number; }

std::string Tile::getColors() { 
    std::string returnString{leftColor};
    returnString += topColor;
    returnString += rightColor;
    returnString += bottomColor;

    return returnString;
}

int Tile::getId() { return tileId; }

std::vector<std::vector<SDL_Vertex>> Tile::getVertex(float x, float y, int tileSize) {
    // map for the colors
    std::map<char, SDL_Color> colors;
    colors['R'] = SDL_Color{255, 0, 0, 255};
    colors['G'] = SDL_Color{0, 255, 0, 255};
    colors['B'] = SDL_Color{0, 0, 255, 255};

    std::map<int, char> converterIntChar;
    converterIntChar[0] = leftColor;
    converterIntChar[1] = topColor;
    converterIntChar[2] = rightColor;
    converterIntChar[3] = bottomColor;

    SDL_FPoint points[5] = {
        SDL_FPoint{x*tileSize, y*tileSize},
        SDL_FPoint{x*tileSize + tileSize, y*tileSize},
        SDL_FPoint{x*tileSize, y*tileSize + tileSize},
        SDL_FPoint{x*tileSize + tileSize, y*tileSize + tileSize},
        SDL_FPoint{x*tileSize + tileSize/2, y*tileSize + tileSize/2}
    };
    
    std::map<int, std::vector<SDL_FPoint>> converterSidePoint;    
    converterSidePoint[0] = {points[4], points[0], points[2]};
    converterSidePoint[1] = {points[4], points[0], points[1]};
    converterSidePoint[2] = {points[4], points[3], points[1]};
    converterSidePoint[3] = {points[4], points[2], points[3]};

    std::vector<std::vector<SDL_Vertex>> returnVector;

    for (int side=0; side<4; side++) {
        std::vector<SDL_Vertex> sideVector;
        
        // the color and coordinates of the vertex to add for this side
        char colorChar = converterIntChar.find(side)->second;
        SDL_Color color = colors.find(colorChar)->second;
        std::vector<SDL_FPoint> VertexPoints = converterSidePoint.find(side)->second;

        for (int vertex=0; vertex<VertexPoints.size(); vertex++) {
            SDL_Vertex returnVertex = {VertexPoints[vertex], color, {1, 1}};
            sideVector.push_back(returnVertex);
        } 

        returnVector.push_back(sideVector);
    }

    return returnVector;
}

std::vector<std::vector<int>> Tile::getLines(int x, int y, int tileSize) {
    std::vector<std::vector<int>> returnVector;

    SDL_Point points[4]= {
        SDL_Point{x*tileSize, y*tileSize},
        SDL_Point{x*tileSize + tileSize, y*tileSize},
        SDL_Point{x*tileSize, y*tileSize + tileSize},
        SDL_Point{x*tileSize + tileSize, y*tileSize + tileSize}
    };

    std::vector<int> pointOfLine;
    pointOfLine.push_back(points[0].x);
    pointOfLine.push_back(points[0].y);
    pointOfLine.push_back(points[3].x);
    pointOfLine.push_back(points[3].y);
    returnVector.push_back(pointOfLine);
    
    pointOfLine.clear();
    pointOfLine.push_back(points[1].x-1);
    pointOfLine.push_back(points[1].y);
    pointOfLine.push_back(points[2].x);
    pointOfLine.push_back(points[2].y-1);
    returnVector.push_back(pointOfLine);

    return returnVector;
}

SDL_Rect Tile::getOutline(int x, int y, int tileSize) {
    int posX = x * tileSize;
    int posY = y * tileSize;
    
    SDL_Rect returnOutline{posX, posY, tileSize, tileSize};

    return returnOutline;
}