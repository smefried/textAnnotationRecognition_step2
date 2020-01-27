#include "bitmap.h"
#include "Position.h"

#include <random>
#include <vector>
#include <queue>
#include <iostream>

using namespace std;

std::random_device rd;
std::mt19937_64 gen {rd()};
std::uniform_int_distribution<> red_dist(1, 255);
std::uniform_int_distribution<> blue_dist(1, 255);
std::uniform_int_distribution<> green_dist(1, 255);

Pixel get_random_pixel(){
    Pixel randomPixle(red_dist(gen),blue_dist(gen),green_dist(gen));
    return randomPixle;
}

bool isBlack(Pixel rgb){
    return (rgb.red==0 & rgb.green==0 & rgb.blue == 0);
}

bool isWhite(Pixel rgb){
    return (rgb.red==255 & rgb.green==255 & rgb.blue == 255);
}

bool checkSlidingWindowConditions(vector <vector <Pixel> > & extBmp,
                                  Position topLeftCorner,
                                  int width,
                                  int length,
                                  bool horizontalConditions,
                                  Position & topLeftPos,
                                  Position & topRightPos,
                                  Position & bottomLeftPos,
                                  Position & bottomRightPos){
    int firstTopLeft;
    int lastBottomLeft;
    int firstTopRight;
    int lastBottomRight;

    if (isBlack(extBmp[topLeftCorner.x][topLeftCorner.y])) return false;
    if (isBlack(extBmp[topLeftCorner.x + width][topLeftCorner.y])) return false;
    if (isBlack(extBmp[topLeftCorner.x][topLeftCorner.y + length])) return false;
    if (isBlack(extBmp[topLeftCorner.x + width][topLeftCorner.y + length])) return false;

    if (horizontalConditions){
        firstTopLeft = firstTopRight = topLeftCorner.x;
        lastBottomLeft = lastBottomRight = topLeftCorner.x + width;
        while (!isBlack(extBmp[firstTopLeft][topLeftCorner.y])){
            if (firstTopLeft == topLeftCorner.x + width) return false;
            firstTopLeft++;
        }
        while (!isBlack(extBmp[firstTopRight][topLeftCorner.y + length])){
            if (firstTopRight == topLeftCorner.x + width) return false;
            firstTopRight++;
        }
        while (!isBlack(extBmp[lastBottomLeft][topLeftCorner.y])) lastBottomLeft--;
        while (!isBlack(extBmp[lastBottomRight][topLeftCorner.y + length])) lastBottomRight--;
        for (int i = firstTopLeft;i<=lastBottomLeft;i++){
            if (!isBlack(extBmp[i][topLeftCorner.y])) return false;
        }
        for (int i = firstTopRight;i<=lastBottomRight;i++){
            if (!isBlack(extBmp[i][topLeftCorner.y + length])) return false;
        }

        topLeftPos.x = firstTopLeft;
        topLeftPos.y = topLeftCorner.y;
        topRightPos.x = firstTopRight;
        topRightPos.y = topLeftCorner.y + length;
        bottomLeftPos.x = lastBottomLeft;
        bottomLeftPos.y = topLeftCorner.y;
        bottomRightPos.x = lastBottomRight;
        bottomRightPos.y = topLeftCorner.y + length;

    } else {
        firstTopLeft = lastBottomLeft = topLeftCorner.y;
        firstTopRight = lastBottomRight = topLeftCorner.y + length;
        while (!isBlack(extBmp[topLeftCorner.x][firstTopLeft])){
            if (firstTopLeft == topLeftCorner.y + length) return false;
            firstTopLeft++;
        }
        while (!isBlack(extBmp[topLeftCorner.x + width][lastBottomLeft])){
            if (lastBottomLeft == topLeftCorner.y + length) return false;
            lastBottomLeft++;
        }
        while (!isBlack(extBmp[topLeftCorner.x][firstTopRight])) firstTopRight--;
        while (!isBlack(extBmp[topLeftCorner.x + width][lastBottomRight])) lastBottomRight--;
        for (int i = firstTopLeft;i<=firstTopRight;i++){
            if (!isBlack(extBmp[topLeftCorner.x][i])) return false;
        }
        for (int i = lastBottomLeft;i<=lastBottomRight;i++){
            if (!isBlack(extBmp[topLeftCorner.x + width][i])) return false;
        }

        topLeftPos.x = topLeftCorner.x;
        topLeftPos.y = firstTopLeft;
        topRightPos.x = topLeftCorner.x + width;
        topRightPos.y = firstTopRight;
        bottomLeftPos.x = topLeftCorner.x;
        bottomLeftPos.y = lastBottomLeft;
        bottomRightPos.x = topLeftCorner.x + width;
        bottomRightPos.y = lastBottomRight;
    }
    return true;
}

void addSlideTopInfoAux(vector <vector <Pixel> > & extBmp,
                         vector < vector <bool> > & notUsed,
                         queue <Position> & pixelsProcessed,
                         Position topLeftPos,
                         Position topRightPos,
                         Position topMiddlePos,
                         Position bottomMiddlePos,
                         Position p){
    if (isBlack(extBmp[p.x][p.y]) & notUsed[p.x][p.y] & (p.x <= topLeftPos.x) ) {
        if (topMiddlePos.y==bottomMiddlePos.y){
            if ((p.y >= topLeftPos.y) & (p.y <= topRightPos.y)) {
                notUsed[p.x][p.y] = false;
                pixelsProcessed.push(p);
            }
        }
        else {
            double m = ((double)((int)bottomMiddlePos.x-(int)topMiddlePos.x))/((double)((int)bottomMiddlePos.y-(int)topMiddlePos.y));
            int b1 = topLeftPos.x - (int) (m * (double) topLeftPos.y);
            int b2 = topRightPos.x - (int) (m * (double) topRightPos.y);
            if (bottomMiddlePos.y > topMiddlePos.y){
                if (   (p.x <= ((int) (m * (double) p.y) + b1) )
                     & (p.x >= ((int) (m * (double) p.y) + b2) ) ){
                    notUsed[p.x][p.y]=false;
                    pixelsProcessed.push(p);
                }
            }
            else {
                if (   (p.x >= ((int) (m * (double) p.y) + b1))
                     & (p.x <= ((int) (m * (double) p.y) + b2)) ){
                    notUsed[p.x][p.y]=false;
                    pixelsProcessed.push(p);
                }
            }

        }
    }
}

vector <Position> addSlideTopInfo(vector <vector <Pixel> > & extBmp,
                                  Position topLeftPos,
                                  Position topRightPos,
                                  Position bottomLeftPos,
                                  Position bottomRightPos ){
    Position topMiddlePos(topLeftPos.x,topLeftPos.y+(topRightPos.y-topLeftPos.y)/2);
    Position bottomMiddlePos(bottomLeftPos.x,bottomLeftPos.y+(bottomRightPos.y-bottomLeftPos.y)/2);
    vector <bool> notUsedLine(extBmp[0].size(),true);
    vector < vector <bool> > notUsed(extBmp.size(),notUsedLine);
    queue <Position> pixelsProcessed;
    vector <Position> pixelsFound;
    pixelsProcessed.push(topLeftPos);
    while (!pixelsProcessed.empty()) {
        Position frontPos = pixelsProcessed.front();
        Position upP(frontPos.x - 1, frontPos.y);
        Position downP(frontPos.x + 1, frontPos.y);
        Position leftP(frontPos.x, frontPos.y - 1);
        Position rightP(frontPos.x, frontPos.y + 1);
        Position upLeftP(frontPos.x - 1, frontPos.y - 1);
        Position upRightP(frontPos.x - 1, frontPos.y + 1);
        Position downLeftP(frontPos.x + 1, frontPos.y - 1);
        Position downRightP(frontPos.x + 1, frontPos.y + 1);
        pixelsFound.push_back(frontPos);
        addSlideTopInfoAux(extBmp,notUsed,pixelsProcessed,topLeftPos,topRightPos,
                           topMiddlePos,bottomMiddlePos,upP);
        addSlideTopInfoAux(extBmp,notUsed,pixelsProcessed,topLeftPos,topRightPos,
                           topMiddlePos,bottomMiddlePos,downP);
        addSlideTopInfoAux(extBmp,notUsed,pixelsProcessed,topLeftPos,topRightPos,
                           topMiddlePos,bottomMiddlePos,leftP);
        addSlideTopInfoAux(extBmp,notUsed,pixelsProcessed,topLeftPos,topRightPos,
                           topMiddlePos,bottomMiddlePos,rightP);
        addSlideTopInfoAux(extBmp,notUsed,pixelsProcessed,topLeftPos,topRightPos,
                           topMiddlePos,bottomMiddlePos,upLeftP);
        addSlideTopInfoAux(extBmp,notUsed,pixelsProcessed,topLeftPos,topRightPos,
                           topMiddlePos,bottomMiddlePos,upRightP);
        addSlideTopInfoAux(extBmp,notUsed,pixelsProcessed,topLeftPos,topRightPos,
                           topMiddlePos,bottomMiddlePos,downLeftP);
        addSlideTopInfoAux(extBmp,notUsed,pixelsProcessed,topLeftPos,topRightPos,
                           topMiddlePos,bottomMiddlePos,downRightP);
        pixelsProcessed.pop();
    }
    return pixelsFound;
}

void addSlideRightInfoAux(vector <vector <Pixel> > & extBmp,
                        vector < vector <bool> > & notUsed,
                        queue <Position> & pixelsProcessed,
                        Position rightTopPos,
                        Position rightBottomPos,
                        Position rightMiddlePos,
                        Position leftMiddlePos,
                        Position p){
    if (isBlack(extBmp[p.x][p.y]) & notUsed[p.x][p.y] & (p.y >= rightTopPos.y) ) {
        if (rightMiddlePos.x==leftMiddlePos.x){
            if ((p.x >= rightTopPos.x) & (p.x <= rightBottomPos.x)) {
                notUsed[p.x][p.y] = false;
                pixelsProcessed.push(p);
            }
        }
        else {
            double m = ((double)((int)leftMiddlePos.x-(int)rightMiddlePos.x))/((double)((int)leftMiddlePos.y-(int)rightMiddlePos.y));
            int b1 = rightTopPos.x - (int) (m * (double) rightTopPos.y);
            int b2 = rightBottomPos.x - (int) (m * (double) rightBottomPos.y);
            if (   (p.x >= ((int) (m * (double) p.y) + b1) )
                   & (p.x <= ((int) (m * (double) p.y) + b2) ) ){
                notUsed[p.x][p.y]=false;
                pixelsProcessed.push(p);
            }
        }
    }
}

vector <Position> addSlideRightInfo(vector <vector <Pixel> > & extBmp,
                                    Position leftTopPos,
                                    Position rightTopPos,
                                    Position leftBottomPos,
                                    Position rightBottomPos ){
    Position rightMiddlePos(rightTopPos.x + (rightBottomPos.x - rightTopPos.x)/2 ,rightTopPos.y);
    Position leftMiddlePos(leftTopPos.x + (leftBottomPos.x - leftTopPos.x)/2 ,leftTopPos.y);
    vector <bool> notUsedLine(extBmp[0].size(),true);
    vector < vector <bool> > notUsed(extBmp.size(),notUsedLine);
    queue <Position> pixelsProcessed;
    vector <Position> pixelsFound;
    pixelsProcessed.push(rightTopPos);
    while (!pixelsProcessed.empty()) {
        Position frontPos = pixelsProcessed.front();
        Position upP(frontPos.x - 1, frontPos.y);
        Position downP(frontPos.x + 1, frontPos.y);
        Position leftP(frontPos.x, frontPos.y - 1);
        Position rightP(frontPos.x, frontPos.y + 1);
        Position upLeftP(frontPos.x - 1, frontPos.y - 1);
        Position upRightP(frontPos.x - 1, frontPos.y + 1);
        Position downLeftP(frontPos.x + 1, frontPos.y - 1);
        Position downRightP(frontPos.x + 1, frontPos.y + 1);
        pixelsFound.push_back(frontPos);
        addSlideRightInfoAux(extBmp,notUsed,pixelsProcessed,rightTopPos,rightBottomPos,
                             rightMiddlePos,leftMiddlePos,upP);
        addSlideRightInfoAux(extBmp,notUsed,pixelsProcessed,rightTopPos,rightBottomPos,
                             rightMiddlePos,leftMiddlePos,downP);
        addSlideRightInfoAux(extBmp,notUsed,pixelsProcessed,rightTopPos,rightBottomPos,
                             rightMiddlePos,leftMiddlePos,leftP);
        addSlideRightInfoAux(extBmp,notUsed,pixelsProcessed,rightTopPos,rightBottomPos,
                             rightMiddlePos,leftMiddlePos,rightP);
        addSlideRightInfoAux(extBmp,notUsed,pixelsProcessed,rightTopPos,rightBottomPos,
                             rightMiddlePos,leftMiddlePos,upLeftP);
        addSlideRightInfoAux(extBmp,notUsed,pixelsProcessed,rightTopPos,rightBottomPos,
                             rightMiddlePos,leftMiddlePos,upRightP);
        addSlideRightInfoAux(extBmp,notUsed,pixelsProcessed,rightTopPos,rightBottomPos,
                             rightMiddlePos,leftMiddlePos,downLeftP);
        addSlideRightInfoAux(extBmp,notUsed,pixelsProcessed,rightTopPos,rightBottomPos,
                             rightMiddlePos,leftMiddlePos,downRightP);

        pixelsProcessed.pop();
    }
    return pixelsFound;
}

void addSlideLeftInfoAux(vector <vector <Pixel> > & extBmp,
                          vector < vector <bool> > & notUsed,
                          queue <Position> & pixelsProcessed,
                          Position leftTopPos,
                          Position leftBottomPos,
                          Position rightMiddlePos,
                          Position leftMiddlePos,
                          Position p){
    if (isBlack(extBmp[p.x][p.y]) & notUsed[p.x][p.y] & (p.y <= leftTopPos.y) ) {
        if (rightMiddlePos.x==leftMiddlePos.x){
            if ((p.x >= leftTopPos.x) & (p.x <= leftBottomPos.x)) {
                notUsed[p.x][p.y] = false;
                pixelsProcessed.push(p);
            }
        }
        else {
            double m = ((double)((int)leftMiddlePos.x-(int)rightMiddlePos.x))/((double)((int)leftMiddlePos.y-(int)rightMiddlePos.y));
            int b1 = leftTopPos.x - (int) (m * (double) leftTopPos.y);
            int b2 = leftBottomPos.x - (int) (m * (double) leftBottomPos.y);
            if (   (p.x >= ((int) (m * (double) p.y) + b1) )
                   & (p.x <= ((int) (m * (double) p.y) + b2) ) ){
                notUsed[p.x][p.y]=false;
                pixelsProcessed.push(p);
            }
        }
    }
}

vector <Position> addSlideLeftInfo(vector <vector <Pixel> > & extBmp,
                                   Position leftTopPos,
                                   Position rightTopPos,
                                   Position leftBottomPos,
                                   Position rightBottomPos ){
    Position rightMiddlePos(rightTopPos.x + ((int)rightBottomPos.x - (int)rightTopPos.x)/2 ,rightTopPos.y);
    Position leftMiddlePos(leftTopPos.x + ((int)leftBottomPos.x - (int)leftTopPos.x)/2 ,leftTopPos.y);
    vector <bool> notUsedLine(extBmp[0].size(),true);
    vector < vector <bool> > notUsed(extBmp.size(),notUsedLine);
    queue <Position> pixelsProcessed;
    vector <Position> pixelsFound;
    pixelsProcessed.push(leftTopPos);
    while (!pixelsProcessed.empty()) {
        Position frontPos = pixelsProcessed.front();
        Position upP(frontPos.x - 1, frontPos.y);
        Position downP(frontPos.x + 1, frontPos.y);
        Position leftP(frontPos.x, frontPos.y - 1);
        Position rightP(frontPos.x, frontPos.y + 1);
        Position upLeftP(frontPos.x - 1, frontPos.y - 1);
        Position upRightP(frontPos.x - 1, frontPos.y + 1);
        Position downLeftP(frontPos.x + 1, frontPos.y - 1);
        Position downRightP(frontPos.x + 1, frontPos.y + 1);
        pixelsFound.push_back(frontPos);
        addSlideLeftInfoAux(extBmp,notUsed,pixelsProcessed,leftTopPos,leftBottomPos,
                             rightMiddlePos,leftMiddlePos,upP);
        addSlideLeftInfoAux(extBmp,notUsed,pixelsProcessed,leftTopPos,leftBottomPos,
                             rightMiddlePos,leftMiddlePos,downP);
        addSlideLeftInfoAux(extBmp,notUsed,pixelsProcessed,leftTopPos,leftBottomPos,
                             rightMiddlePos,leftMiddlePos,leftP);
        addSlideLeftInfoAux(extBmp,notUsed,pixelsProcessed,leftTopPos,leftBottomPos,
                             rightMiddlePos,leftMiddlePos,rightP);
        addSlideLeftInfoAux(extBmp,notUsed,pixelsProcessed,leftTopPos,leftBottomPos,
                             rightMiddlePos,leftMiddlePos,upLeftP);
        addSlideLeftInfoAux(extBmp,notUsed,pixelsProcessed,leftTopPos,leftBottomPos,
                             rightMiddlePos,leftMiddlePos,upRightP);
        addSlideLeftInfoAux(extBmp,notUsed,pixelsProcessed,leftTopPos,leftBottomPos,
                             rightMiddlePos,leftMiddlePos,downLeftP);
        addSlideLeftInfoAux(extBmp,notUsed,pixelsProcessed,leftTopPos,leftBottomPos,
                             rightMiddlePos,leftMiddlePos,downRightP);

        pixelsProcessed.pop();
    }
    return pixelsFound;
}


void addSlideBottomInfoAux(vector <vector <Pixel> > & extBmp,
                        vector < vector <bool> > & notUsed,
                        queue <Position> & pixelsProcessed,
                        Position bottomLeftPos,
                        Position bottomRightPos,
                        Position topMiddlePos,
                        Position bottomMiddlePos,
                        Position p){
    if (isBlack(extBmp[p.x][p.y]) & notUsed[p.x][p.y] & (p.x >= bottomLeftPos.x) ) {
        if (topMiddlePos.y==bottomMiddlePos.y){
            if ((p.y >= bottomLeftPos.y) & (p.y <= bottomRightPos.y)) {
                notUsed[p.x][p.y] = false;
                pixelsProcessed.push(p);
            }
        }
        else {
            double m = ((double)((int)bottomMiddlePos.x-(int)topMiddlePos.x))/((double)((int)bottomMiddlePos.y-(int)topMiddlePos.y));
            int b1 = bottomLeftPos.x - (int) (m * (double) bottomLeftPos.y);
            int b2 = bottomRightPos.x - (int) (m * (double) bottomRightPos.y);
            if (bottomMiddlePos.y > topMiddlePos.y){
                if (   (p.x <= ((int) (m * (double) p.y) + b1) )
                       & (p.x >= ((int) (m * (double) p.y) + b2) ) ){
                    notUsed[p.x][p.y]=false;
                    pixelsProcessed.push(p);
                }
            }
            else {
                if (   (p.x >= ((int) (m * (double) p.y) + b1))
                       & (p.x <= ((int) (m * (double) p.y) + b2)) ){
                    notUsed[p.x][p.y]=false;
                    pixelsProcessed.push(p);
                }
            }

        }
    }
}

vector <Position> addSlideBottomInfo(vector <vector <Pixel> > & extBmp,
                                  Position topLeftPos,
                                  Position topRightPos,
                                  Position bottomLeftPos,
                                  Position bottomRightPos ){
    Position topMiddlePos(topLeftPos.x,topLeftPos.y+((int)topRightPos.y-(int)topLeftPos.y)/2);
    Position bottomMiddlePos(bottomLeftPos.x,bottomLeftPos.y+((int)bottomRightPos.y-(int)bottomLeftPos.y)/2);
    vector <bool> notUsedLine(extBmp[0].size(),true);
    vector < vector <bool> > notUsed(extBmp.size(),notUsedLine);
    queue <Position> pixelsProcessed;
    vector <Position> pixelsFound;
    pixelsProcessed.push(bottomLeftPos);
    while (!pixelsProcessed.empty()) {
        Position frontPos = pixelsProcessed.front();
        Position upP(frontPos.x - 1, frontPos.y);
        Position downP(frontPos.x + 1, frontPos.y);
        Position leftP(frontPos.x, frontPos.y - 1);
        Position rightP(frontPos.x, frontPos.y + 1);
        Position upLeftP(frontPos.x - 1, frontPos.y - 1);
        Position upRightP(frontPos.x - 1, frontPos.y + 1);
        Position downLeftP(frontPos.x + 1, frontPos.y - 1);
        Position downRightP(frontPos.x + 1, frontPos.y + 1);
        pixelsFound.push_back(frontPos);
        addSlideBottomInfoAux(extBmp,notUsed,pixelsProcessed,bottomLeftPos,bottomRightPos,
                           topMiddlePos,bottomMiddlePos,upP);
        addSlideBottomInfoAux(extBmp,notUsed,pixelsProcessed,bottomLeftPos,bottomRightPos,
                           topMiddlePos,bottomMiddlePos,downP);
        addSlideBottomInfoAux(extBmp,notUsed,pixelsProcessed,bottomLeftPos,bottomRightPos,
                           topMiddlePos,bottomMiddlePos,leftP);
        addSlideBottomInfoAux(extBmp,notUsed,pixelsProcessed,bottomLeftPos,bottomRightPos,
                           topMiddlePos,bottomMiddlePos,rightP);
        addSlideBottomInfoAux(extBmp,notUsed,pixelsProcessed,bottomLeftPos,bottomRightPos,
                           topMiddlePos,bottomMiddlePos,upLeftP);
        addSlideBottomInfoAux(extBmp,notUsed,pixelsProcessed,bottomLeftPos,bottomRightPos,
                           topMiddlePos,bottomMiddlePos,upRightP);
        addSlideBottomInfoAux(extBmp,notUsed,pixelsProcessed,bottomLeftPos,bottomRightPos,
                           topMiddlePos,bottomMiddlePos,downLeftP);
        addSlideBottomInfoAux(extBmp,notUsed,pixelsProcessed,bottomLeftPos,bottomRightPos,
                           topMiddlePos,bottomMiddlePos,downRightP);
        pixelsProcessed.pop();
    }
    return pixelsFound;
}

vector <Position> addFromExt(vector <vector <Pixel> > & extBmp, Position pos){
    vector <bool> notUsedLine(extBmp[0].size(),true);
    vector < vector <bool> > notUsed(extBmp.size(),notUsedLine);
    queue <Position> pixelsProcessed;
    vector <Position> pixelsFound;
    if (isBlack(extBmp[pos.x][pos.y]) & (pos.x >= 10)
                                      & (pos.x < extBmp.size() -10)
                                      & (pos.y >= 10)
                                      & (pos.y < extBmp[0].size() -10)  ) {
        pixelsProcessed.push(pos);
        while (!pixelsProcessed.empty()) {
            Position frontPos = pixelsProcessed.front();
            Position upP(frontPos.x - 1, frontPos.y);
            Position downP(frontPos.x + 1, frontPos.y);
            Position leftP(frontPos.x, frontPos.y - 1);
            Position rightP(frontPos.x, frontPos.y + 1);
            pixelsFound.push_back(frontPos);
            if (isBlack(extBmp[frontPos.x - 1][frontPos.y])
                & notUsed[frontPos.x - 1][frontPos.y]) {
                notUsed[frontPos.x - 1][frontPos.y]=false;
                pixelsProcessed.push(upP);
            }
            if (isBlack(extBmp[frontPos.x + 1][frontPos.y])
                & notUsed[frontPos.x + 1][frontPos.y]) {
                notUsed[frontPos.x + 1][frontPos.y]=false;
                pixelsProcessed.push(downP);
            }
            if (isBlack(extBmp[frontPos.x][frontPos.y - 1])
                & notUsed[frontPos.x][frontPos.y - 1]) {
                notUsed[frontPos.x][frontPos.y - 1]=false;
                pixelsProcessed.push(leftP);
            }
            if (isBlack(extBmp[frontPos.x][frontPos.y + 1])
                & notUsed[frontPos.x][frontPos.y + 1]) {
                notUsed[frontPos.x][frontPos.y + 1]=false;
                pixelsProcessed.push(rightP);
            }
            pixelsProcessed.pop();
        }
    }
    return pixelsFound;
}

vector < vector <Pixel> > toExt24 (vector < vector <Pixel> > original ){
    Pixel whitePixel = Pixel(255,255,255);
    unsigned int columnOrigSize = original.size();
    unsigned int rowOrigSize = original[0].size();
    vector <Pixel> whitePixelLine(rowOrigSize+20,whitePixel);
    vector < vector <Pixel> > result(10,whitePixelLine);
    vector < vector <Pixel> > endLines = result;
    vector <Pixel> whiteSpace(10,whitePixel);
    for (vector <Pixel> line: original){
        line.insert(cbegin(line),cbegin(whiteSpace),cend(whiteSpace));
        line.insert(cend(line),cbegin(whiteSpace),cend(whiteSpace));
        result.push_back(line);
    }
    result.insert(cend(result),cbegin(endLines),cend(endLines));
    return result;
}

void saveExt(vector < vector <Pixel> > extBmp, string filename){
    vector < vector <Pixel> > result;
    for (int i=10;i<extBmp.size()-10;i++){
        vector <Pixel> line;
        for (int j=10;j<extBmp[0].size()-10;j++){
            line.push_back(extBmp[i][j]);
        }
        result.push_back(line);
    }
    Bitmap image;
    image.fromPixelMatrix(result);
    image.save(filename);
}

int main()
{
    Bitmap image;
    vector < vector <Pixel> > bmp;
    vector < vector <Position> > letterObjects;
    Pixel rgb;

    //read a file example.bmp and convert it to a pixel matrix
    image.open("input2.bmp");

    //verify that the file opened was a valid image
    bool validBmp = image.isImage();

    if( validBmp == true )
    {
        bmp = image.toPixelMatrix();
        vector < vector <Pixel> > extBmp = toExt24(bmp);
        vector < vector <Pixel> > horizontalBmp = extBmp;
        int extBmpWidth = extBmp.size();
        int extBmpLength = extBmp[0].size();
        vector <int> horizontalLevelLine(extBmpLength,0);
        vector< vector <int> > horizontalLevel(extBmpWidth,horizontalLevelLine);

        Pixel redRGB(255,0,0);
        Pixel greenRGB(0,255,0);
        Position topLeftPos,topRightPos,bottomLeftPos,bottomRightPos;
        int width = 15;
        int length = 20;
        for (int i = 0; i<extBmp.size() - width; i++){
            for (int j = 0; j<extBmp[0].size() - length;j++){
                vector <Position> tempPos1,tempPos2;
                Position p(i,j);
                if (checkSlidingWindowConditions(extBmp,p,width,length,true,topLeftPos,topRightPos,bottomLeftPos,bottomRightPos)){
                    tempPos1=addSlideLeftInfo(extBmp,topLeftPos,topRightPos, bottomLeftPos, bottomRightPos);
                    for (int k=0;k<tempPos1.size();k++){
                        horizontalLevel[tempPos1[k].x][tempPos1[k].y]++;
                    }
                    tempPos2=addSlideRightInfo(extBmp,topLeftPos,topRightPos, bottomLeftPos, bottomRightPos);
                    for (int k=0;k<tempPos2.size();k++){
                        horizontalLevel[tempPos2[k].x][tempPos2[k].y]++;
                    }
                }

            }
        }

        int maxLevel=0;
        for (int i = 0; i<extBmp.size(); i++){
            for (int j = 0; j<extBmp[0].size();j++){
                if (horizontalLevel[i][j]>maxLevel) maxLevel=horizontalLevel[i][j];
            }
        }
        int diviedor = maxLevel/255;
        for (int i = 0; i<extBmp.size(); i++){
            for (int j = 0; j<extBmp[0].size();j++){
                if(!isWhite(horizontalBmp[i][j])) horizontalBmp[i][j].blue = horizontalLevel[i][j]/(diviedor+1);
            }
        }

        /*
        letterObjects.push_back(addSlideLeftInfo(extBmp,topLeftPos,topRightPos, bottomLeftPos, bottomRightPos));
        for (Position p: letterObjects[0]){
            extBmp[p.x][p.y] = redRGB;
        }
        letterObjects.push_back(addSlideRightInfo(extBmp,topLeftPos,topRightPos, bottomLeftPos, bottomRightPos));
        for (Position p: letterObjects[1]){
            extBmp[p.x][p.y] = redRGB;
        }
        extBmp[topLeftPos.x][topLeftPos.y] = greenRGB;
        extBmp[topRightPos.x][topRightPos.y] = greenRGB;
        extBmp[bottomLeftPos.x][bottomLeftPos.y] = greenRGB;
        extBmp[bottomRightPos.x][bottomRightPos.y] = greenRGB;
         */
        image.fromPixelMatrix(horizontalBmp);
        image.save("test01.bmp");
        /*
        for (unsigned int i=10;i<extBmp.size()-10;i++){
            for (unsigned int j=10;j<extBmp[i].size()-10;j++){
                if (isBlack(extBmp[i][j])){
                    Position lookfor(i,j);
                    letterObjects.push_back(addFromExt(extBmp,lookfor));
                }
            }
        }
        image.fromPixelMatrix(extBmp);
        image.save("cleanResult.bmp");

        for (unsigned int i=0;i<letterObjects.size();i++){
            Pixel randomRGB = get_random_pixel();
            for (Position p: letterObjects[i]){
                extBmp[p.x][p.y] = randomRGB;
            }
        }
        saveExt(extBmp,"colorResult1.bmp");
        image.fromPixelMatrix(extBmp);
        image.save("colorResult2.bmp");
         */
    }



    return 0;
}