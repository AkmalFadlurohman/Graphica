#include "fbp.h"

// mouse
const char *device_mouse = "/dev/input/mice";

//Game world
unsigned int world[WORLD_WIDTH][WORLD_HEIGHT]; 
int viewport_x;
int viewport_y;
int viewport_width = 640;
int viewport_height = 480;

//FBP variables
char *fbp = 0;
int centerX = 0;
int fullY = 0;
int frameColor;
int critColor;
long int screensize = 0;
int fbfd = 0;

//VecLetters variables
int letterCount, letterHeight;
struct VecLetter** letters;
int COLOR = 0;
int BORDER_COLOR = 0;
int MARGIN_VERTICAL = 3;
int MARGIN_HORIZONTAL = 3;

//Bitmap plane variables

// F0
void drawPointer(Mouse* m) {
    char* pointer = "100000000000011000000000001110000000000111100000000011111000000001111110000000111111100000011111111000001111111110000111111111100011111111111001111111111110111111111111111111111100001111011111000111001111100011000011111001000001111100000000011111000000001111100000000011111000000001111100000000011100000000000100";
    int x = m->positionX; int y = m->positionY;
    int width = 13; int height = 24;
    int initx = x, inity = y;

    for (x = initx; x < initx+width; x++) {
        if (x > m->screen_max_x)
            continue;
        for (y = inity; y < inity+height; y++) {
            if (y > m->screen_max_y)
                break;
            if (pointer[(y-inity)*width+(x-initx)]=='1')
                f_drawPixel(x, y, rgbaToInt(255,255,255,0));    
        }
    }
}

// F1 | FBP Manipulations
void initializeFBP() {

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }
    printf("Detected display: %dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }

    if(viewport_width > vinfo.xres || viewport_height > vinfo.yres){
        printf("Ukuran viewport salah\n");

        exit(5);
    }
}

void clearViewPort(int color){
    long int location;

    for(int x = 0; x < viewport_width + viewport_x; x++){
        for(int y = 0; y < viewport_height + viewport_y; y++){
            world[x][y] = color;
        }
    }
}

void clearScreen(){
    long int location;
    int color = rgbaToInt(10,10,10,0);
    for(int x = 0; x < vinfo.xres; x++){
        for(int y = 0; y < vinfo.yres; y++){
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = color;
            *(fbp + location + 1) = color >> 8;
            *(fbp + location + 2) = color >> 16;
            *(fbp + location + 3) = color >> 24; 
        }
    }
}

void render(){
    long int location;
    int color;
    int offsetX = (vinfo.xres - viewport_width)/2;
    int offsetY = (vinfo.yres - viewport_height)/2;


    for(int x = 0; x < viewport_width; x++){
        for(int y = 0; y < viewport_height; y++){
            int worldx = x + viewport_x;
            int worldy = y + viewport_y;
            if(isValidPoint(worldx,worldy)){
                color = world[worldx][worldy];
            }
            else{
                color = rgbaToInt(0,0,0,0);
            }
            location = (x+offsetX+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+offsetY+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = color;
            *(fbp + location + 1) = color >> 8;
            *(fbp + location + 2) = color >> 16;
            *(fbp + location + 3) = color >> 24;
        }
    }
}

void drawPixel(int x, int y, unsigned int color) {
    int i = 0, j = 0;
    x = x*SCALE; y = y*SCALE;
    for (i = 0; i < SCALE; i++) {
        for (j = 0; j < SCALE; j++) {
            world[x+i][y+j] = color;
        }
    }
}

unsigned int rgbaToInt(int r, int g, int b, int a) {
    return a << 24 | r << 16 | g << 8 | b;
}

unsigned int getPixelColor(int x, int y) {
    if(!isValidPoint(x,y)){
        return 0;
    }
    x = x * SCALE;
    y = y * SCALE;

   return world[x][y];
}

// F2-A | Bitmap plane functions
void drawPixelWithScale(int x, int y, unsigned int color, int scale) {
    long int location;
    int i = 0, j = 0;
    x = x*scale; y = y*scale;
    for (i = 0; i < scale; i++)
        for (j = 0; j < scale; j++) {
            location = (x+i+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+j+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = color;
            *(fbp + location + 1) = color >> 8;
            *(fbp + location + 2) = color >> 16;
            *(fbp + location + 3) = color >> 24;
        }
}

void drawObject(struct f_Image* obj, int dir /*0 = right, 1 = left*/){
    for (int i = 0; i <  obj->height; i++) {
        for (int j = 0; j < obj->width ; j++){
                if(dir == 0){
                    if(j+obj->posX < vinfo.xres && j+obj->posX >= 0 && i+obj->posY <vinfo.yres && i+obj->posY  >=0)
                        f_drawPixel(j+obj->posX,i+obj->posY,obj->bitmap[i*obj->width+j]->color);

                } else{
                   if(obj->width-j + obj->posX < vinfo.xres && obj->width-j + obj->posX >= 0 && i +obj->posY <vinfo.yres && i +obj->posY >=0)
                        f_drawPixel(obj->width-j + obj->posX,i +obj->posY,obj->bitmap[i*obj->width+j]->color);
                }
            
        }
    }
}

void f_drawPixel(int x, int y, unsigned int color) {
    long int location;
    int i = 0, j = 0;
    x = x*SCALE; y = y*SCALE;
    for (i = 0; i < SCALE; i++)
        for (j = 0; j < SCALE; j++) {
            location = (x+i+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+j+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = color;
            *(fbp + location + 1) = color >> 8;
            *(fbp + location + 2) = color >> 16;
            *(fbp + location + 3) = color >> 24;
        }
}

// F2-B | Line drawing functions
int isValidPoint(int x, int y) {
    if (x >= 0 && x < WORLD_WIDTH/SCALE && y >=0 && y < WORLD_HEIGHT/SCALE)
        return 1;

    return 0;
}

void drawLineLow(double x0, double y0, double x1, double y1, unsigned int color) {
    double dx, dy, D;
    dx = x1 - x0;
    dy = y1 - y0;
    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    D = 2 * dy - dx;
    int y = y0;


    for (double x = x0; x <= x1; x++) {
        if (isValidPoint(x, y) == 0)
            return;
        drawPixel(x, y, color);
        if (D > 0) {
            y += yi;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }

    
    if (isValidPoint(round(x0), round(y0)))
        drawPixel(x0, y0, color);
    if (isValidPoint(round(x1), round(y1)))
        drawPixel(x1, y1, color);
}

void drawLineHigh(double x0, double y0, double x1, double y1, unsigned int color) {
    double dx, dy, D;
    dx = x1 - x0;
    dy = y1 - y0;
    int xi = 1;



    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }


    D = 2 * dx - dy;
    int x = x0;

    for (double y = y0; y <= y1; y++) {
        if (isValidPoint(x, y) == 0)
            return;
        drawPixel(x, y, color);
        if (D > 0) {
            x += xi;
            D -= 2 * dy;
        }

        D += 2 * dx;
    }
        if (isValidPoint(round(x0), round(y0)))
        drawPixel(x0, y0, color);
    if (isValidPoint(round(x1), round(y1)))
        drawPixel(x1, y1,color);
}

void drawVectorLine(VectorPoint* point1, VectorPoint* point2, unsigned int color, int offsetX, int offsetY) {
    double x1,y1,x2,y2;
    x1 = round(point1->x);
    y1 = round(point1->y);
    x2 = round(point2->x);
    y2 = round(point2->y);
    if (abs(y2 - y1) < abs(x2 - x1)) {
        if (x1 > x2) {
            drawLineLow(x2 + offsetX, y2 + offsetY, x1 + offsetX, y1 + offsetY, color);
        } else {
            drawLineLow(x1 + offsetX, y1 + offsetY, x2 + offsetX, y2 + offsetY, color);
        }
    } else {
        if (y1 > y2) {
            drawLineHigh(x2 + offsetX, y2 + offsetY, x1 + offsetX, y1 + offsetY, color);
        } else {
            drawLineHigh(x1 + offsetX, y1 + offsetY, x2 + offsetX, y2 + offsetY, color);
        }
    }
}

// F2-C | VecLetters drawing Functions
int isValidPointScale(int x, int y, int scale) {
    if (x >= 0 && x < vinfo.xres/scale && y >=0 && y < vinfo.yres/scale)
        return 1;

    return 0;
}

void drawLine_line(struct Line* line, int color, int offsetX, int offsetY) {
    if (abs(line->points[1]->y - line->points[0]->y) < abs(line->points[1]->x - line->points[0]->x)) {
        if (line->points[0]->x > line->points[1]->x) {
            drawLineLow(line->points[1]->x + offsetX, line->points[1]->y + offsetY, line->points[0]->x + offsetX, line->points[0]->y + offsetY, color);
        } else {
            drawLineLow(line->points[0]->x + offsetX, line->points[0]->y + offsetY, line->points[1]->x + offsetX, line->points[1]->y + offsetY, color);
        }
    } else {
        if (line->points[0]->y > line->points[1]->y) {
            drawLineHigh(line->points[1]->x + offsetX, line->points[1]->y + offsetY, line->points[0]->x + offsetX, line->points[0]->y + offsetY, color);
        } else {
            drawLineHigh(line->points[0]->x + offsetX, line->points[0]->y + offsetY, line->points[1]->x + offsetX, line->points[1]->y + offsetY, color);
        }
    }
}

void drawLineWithScale(double x0, double y0, double x1, double y1, int color, int scale) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            drawLineLowWithScale(x1, y1, x0, y0, color, scale);
        } else {
            drawLineLowWithScale(x0, y0, x1, y1, color, scale);
        }
    } else {
        if (y0 > y1) {
            drawLineHighWithScale(x1, y1, x0, y0, color, scale);
        } else {
            drawLineHighWithScale(x0, y0, x1, y1, color, scale);
        }
    }
}

void drawLineHighWithScale(double x0, double y0, double x1, double y1, int color, int scale) {
    double dx, dy, D;
    dx = x1 - x0;
    dy = y1 - y0;
    int xi = 1;

    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    D = 2 * dx - dy;
    int x = x0;

    for (double y = y0; y < y1; y++) {
        if (isValidPointScale(x, y, scale) == 0)
            return;
        drawPixelWithScale(x, y, color, scale);
        if (D > 0) {
            x += xi;
            D -= 2 * dy;
        }

        D += 2 * dx;
    }
}

void drawLineLowWithScale(double x0, double y0, double x1, double y1, int color, int scale) {
    double dx, dy, D;
    dx = x1 - x0;
    dy = y1 - y0;
    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    D = 2 * dy - dx;
    int y = y0;

    for (double x = x0; x < x1; x++) {
        if (isValidPointScale(x, y, scale) == 0)
            return;
        drawPixelWithScale(x, y, color, scale);
        if (D > 0) {
            y += yi;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
}

void loadLetters(char* fileName) {
    FILE * specFile;
    size_t bufferSize = 80;
    char *buffer = malloc(bufferSize * sizeof(char));
    int i = 0, ret = 0, letterWidth, letterLineCount, letterCrit;
    char letterName,*lineList,*critList,*lineBuff,*critBuff;
    
    
    specFile = fopen(fileName, "r");
    if (specFile == NULL) {
        printf("Failed to open specification file\n");
        exit(0);
    }
    ret = fscanf(specFile, "%d %d\n", &letterCount, &letterHeight);
    if (ret != 2) {
        printf("Failed to read specification file\n");
        exit(1);
    } else {
        printf("Number of char: %d\n", letterCount);
        printf("Height of char: %d\n", letterHeight);
    }
    
    letters = malloc(letterCount*sizeof(struct VecLetter*));
    
    while ((getline(&buffer, &bufferSize, specFile) != -1) && (i < letterCount)) {
        lineList = (char*) malloc(256 * sizeof(char));
        critList = (char*) malloc(256 * sizeof(char));
        lineBuff = (char*) malloc(18 * sizeof(char));
        critBuff = (char*) malloc(18 * sizeof(char));
        ret = 0;
        ret = sscanf(buffer, "%c|%d|%d|%d|%[^|\n]|%[^\n]s", &letterName, &letterWidth, &letterLineCount, &letterCrit, lineList, critList);
        if (ret != 6) {
            printf("Error reading line %d\n",i+2);
        } else {
            letters[i] = vecLetterInit(letterName, 0, 0, letterHeight, letterWidth, letterLineCount, letterCrit);
            lineBuff = strtok(lineList," ");
            int j = 0;
            while (lineBuff != NULL) {
                double x1,y1,x2,y2;
                sscanf(lineBuff,"<(%lf,%lf),(%lf,%lf)>",&x1,&y1,&x2,&y2);
                letters[i]->lines[j] = lineInit(x1,y1,x2,y2);
                lineBuff = strtok(NULL," ");
                j++;
            }
            j = 0;
            critBuff = strtok(critList," ");
            while (critBuff != NULL) {
                double x,y;
                sscanf(critBuff,"(%lf,%lf)",&x,&y);
                letters[i]->critPoints[j] = pointInit(x,y);
                critBuff = strtok(NULL," ");
                j++;
            }
        }
        i++;
    }
    
    free(buffer);
    fclose(specFile);
}

int drawLetters(char c, int* x, int* y) {
    // Find letter index
    int i = 0; int found = 0, idx = 0; 
    while (i < letterCount && found == 0) {
        if (letters[i]->name == c) {
            idx = i;
            found = 1;
        }
        i++;
    }

    if (found == 0) {
        return 0;
    }

    if (*x + MARGIN_HORIZONTAL + letters[idx]->width >= vinfo.xres / SCALE) {
        *x = MARGIN_HORIZONTAL;
        *y += letterHeight + MARGIN_VERTICAL;
    }
    *x += MARGIN_HORIZONTAL;

    // Draw letter border
    for (int j = 0; j < letters[idx]->numOfLines; j++) {
        drawLine_line(letters[idx]->lines[j], rgbaToInt(255, 0, 0, 0), *x, *y);
    }

    fillLetter(letters[idx], COLOR, BORDER_COLOR, *x, *y);

    *x += letters[idx]->width;
    return letters[idx]->width;    
};

void fillLetter(struct VecLetter* vecletter, unsigned int color, unsigned int boundaryColor, int offsetX, int offsetY) {
    int isFilling = -1;
    int critExist = 0;
    if (vecletter->critPoints != NULL) {
        critExist = 1;
    }
    int critCounter = 0;
    
    // int isInside = 0;

    for (int j = offsetY; j <= vecletter->height + offsetY; j++) {
        isFilling = -1;
        for (int i= offsetX; i <= vecletter->width + offsetX; i++) {
            //crit point
            if (critCounter < vecletter->numOfCritPoints && critExist == 1 && i == vecletter->critPoints[critCounter]->x+offsetX && j == vecletter->critPoints[critCounter]->y+offsetY) {
                critCounter++;
                continue;
            }

            if (getPixelColor(i, j) == boundaryColor) {
                int initI = i;
                while(getPixelColor(i, j) == boundaryColor && i <= vecletter->width + offsetX) {
                    i++;
                }

                if (critCounter < vecletter->numOfCritPoints && critExist == 1 && i == vecletter->critPoints[critCounter]->x + offsetX && j == vecletter->critPoints[critCounter]->y + +offsetY) {
                    critCounter++;
                    // continue;
                } else {
                    isFilling *= -1;
                }
            }
            if (i <= vecletter->width + offsetX) {
                if (isFilling > 0) {
                    drawPixel(i, j, color);
                }
            }
            
        }
    }
}

// F2-D | Bitmap line functions
void drawLine(double x0, double y0, double x1, double y1, int color) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            drawLineLow(x1, y1, x0, y0, color);
        } else {
            drawLineLow(x0, y0, x1, y1, color);
        }
    } else {
        if (y0 > y1) {
            drawLineHigh(x1, y1, x0, y0, color);
        } else {
            drawLineHigh(x0, y0, x1, y1, color);
        }
    }
}

void drawLaser(int x0, int y0, int dx, int dy, int scale, int* pt) {
    int t = *pt;
    if (isValidPointScale(x0 + dx * t, y0 + dy * t, scale) == 0) {
        t = 1; *pt = 1;
    }

    drawLineWithScale(x0, y0, x0 + dx * t, y0 + dy * t, rgbaToInt(0,0,0,0), scale);
    x0 = x0 + dx * t;
    y0 = y0 + dy * t;
    drawLineWithScale(x0, y0, x0 + dx * t, y0 + dy * t, rgbaToInt(0,255,0,0), scale);
}

// F3 | VectorPath exclusive functions
int getPointCode(int x, int y, double offsetX, double offsetY){
    int viewport_max_x = viewport_x + viewport_width;
    int viewport_max_y = viewport_y + viewport_height;
    int code = 0;
    if(x + offsetX < viewport_x){
        code = code + LEFT;
    }
    if(x + offsetX> viewport_max_x -1){
        code = code + RIGHT;
    }
    if(y + offsetY< viewport_y){
        code = code + TOP;
    }
    if(y + offsetY> viewport_max_y -1){
        code = code + BOTTOM;
    }
    return code;
}

VectorPath * vectorClipping (VectorPath * path, int offsetX, int offsetY){
    VectorPath * clipped = NULL;
    if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {


        VectorPoint** currentPoint = path->firstPoint;
        VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;
        int isFirst = 1;
        int pointke=1;
        do {

            //Clipping code
            int code_point1 = getPointCode(currentPoint[0]->x,currentPoint[0]->y, offsetX, offsetY);
            int code_point2 = getPointCode(nextPoint[0]->x,nextPoint[0]->y, offsetX, offsetY);
            pointke++;

            int xmax = viewport_x + viewport_width;
            int ymax = viewport_y + viewport_height;
            int ymin = viewport_y;
            int xmin = viewport_x;
            double x1 = currentPoint[0]->x + offsetX;
            double y1 = currentPoint[0]->y + offsetY;
            double x2 = nextPoint[0]->x + offsetY;
            double y2 = nextPoint[0]->y +offsetX;


            if(code_point1 == 0 && code_point2 == 0){
                
                if(isFirst == 1){
                    clipped = createVectorPath(createVectorPoint(x2-offsetX, y2- offsetY));
                    isFirst = 0;

                } else {
                    appendToPath(clipped, createVectorPoint(x2-offsetX, y2 - offsetY));
                }

            } else if(code_point1 != 0 && code_point2 != 0){
                //donothing

            } else{

                double x, y;

                int code_point_ex = code_point1 ? code_point1 : code_point2;
                if (code_point_ex & TOP)
                {
                    x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1);
                    y = ymin;
                }     
                else if (code_point_ex & BOTTOM)
                {
                    x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1);
                    y = ymax;
                }
                else if (code_point_ex & RIGHT)
                {
                    y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1);
                    x = xmax;
                }
                else
                {
                    y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1);
                    x = xmin;
                }
                //point 1 diluar, point 2 di dalem, simpen v1' dan v2
                if (code_point_ex == code_point1){
                    if(isFirst == 1){
                        clipped = createVectorPath(createVectorPoint(x-offsetX, y- offsetY));
                        appendToPath(clipped, createVectorPoint(x2-offsetX, y2 - offsetY));

                        isFirst = 0;

                    } else {
                        appendToPath(clipped, createVectorPoint(x-offsetX, y - offsetY));
                        appendToPath(clipped, createVectorPoint(x2-offsetX, y2 - offsetY));

                    }


                } else {
                    if(isFirst == 1){
                        clipped = createVectorPath(createVectorPoint(x-offsetX, y- offsetY));

                        isFirst = 0;

                    } else {
                        appendToPath(clipped, createVectorPoint(x-offsetX, y - offsetY));

                    }

                }


            }



            currentPoint = nextPoint;
            if (currentPoint[0] != NULL) {
                nextPoint = currentPoint[0]->nextPoint;
            }

        } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);

        
        if(clipped != NULL){
            if(clipped->numOfPoints > 2){
                enclosePath(clipped);
                checkForMinMaxUpdate(clipped);

                 return clipped;        

            }            
        }
        return NULL;

    }
    return NULL;

}

int drawVectorPathClipping(VectorPath* path, unsigned int boundaryColor, unsigned int fillColor, int offsetX, int offsetY) {
    if (path != NULL) {
        VectorPath * toDraw = vectorClipping(path, offsetX, offsetY);
        if(toDraw != NULL){

            if (toDraw->firstPoint[0] != NULL && toDraw->firstPoint[0]->nextPoint[0] != NULL) {
                VectorPoint** currentPoint = toDraw->firstPoint;
                VectorPoint** nextPoint = toDraw->firstPoint[0]->nextPoint;

                do {
                    if (nextPoint[0] != NULL) {
                        drawVectorLine(currentPoint[0], currentPoint[0]->nextPoint[0], boundaryColor, offsetX, offsetY);
                    }


                    currentPoint = nextPoint;

                    if (currentPoint[0] != NULL) {
                        nextPoint = currentPoint[0]->nextPoint;
                    }
                } while (currentPoint[0] != NULL && currentPoint[0] != toDraw->firstPoint[0]);
            } else {
                return 0;
            }
            fillVector(toDraw, fillColor, boundaryColor, offsetX, offsetY);            
        } else{
            return 0;
        }



    } else {
        return 0;
    }

    return 1;
}

int drawVectorPath(VectorPath* path, unsigned int boundaryColor, unsigned int fillColor, int offsetX, int offsetY) {
    if (path != NULL) {

        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            do {
                if (nextPoint[0] != NULL) {
                    drawVectorLine(currentPoint[0], currentPoint[0]->nextPoint[0], boundaryColor, offsetX, offsetY);
                }


                currentPoint = nextPoint;

                if (currentPoint[0] != NULL) {
                    nextPoint = currentPoint[0]->nextPoint;
                }
            } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);
        } else {
            return 0;
        }
    } else {
        return 0;
    }

   fillVector(path, fillColor, boundaryColor, offsetX, offsetY);

   return 1;
}

int rotatePath(VectorPath* path, float degree, int originX, int originY) {
    if (path != NULL) {
        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            double t=(22*degree)/(180*7);

            do {
                double x1 = currentPoint[0]->x;
                double y1 = currentPoint[0]->y;

                currentPoint[0]->x = ((x1-originX)*cos(t))-((y1-originY)*sin(t)) + originX;
                currentPoint[0]->y = ((x1-originX)*sin(t))+((y1-originY)*cos(t)) +originY;

                currentPoint = nextPoint;
                if (currentPoint[0] != NULL) {
                    nextPoint = currentPoint[0]->nextPoint;
                }
            } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);
        } else {
            return 0;
        }
    } else {
        return 0;
    }

   checkForMinMaxUpdate(path);
    return 1;
}

int dilatatePath(VectorPath* path, int originX, int originY, float zoom) {
    if (path != NULL) {
        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            do {
                double x1 = currentPoint[0]->x;
                double y1 = currentPoint[0]->y;
                currentPoint[0]->x = (x1 - originX) * zoom + originX;
                currentPoint[0]->y = (y1 - originY) * zoom + originY;


                currentPoint = nextPoint;
                if (currentPoint[0] != NULL) {
                    nextPoint = currentPoint[0]->nextPoint;
                }
            } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);
        } else {
            return 0;
        }
    } else {
        return 0;
    }

    checkForMinMaxUpdate(path);
    return 1;
}

int translatePath(VectorPath* path, int dx, int dy) {
    if (path != NULL) {
        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            do {
                double x1 = currentPoint[0]->x;
                double y1 = currentPoint[0]->y;
                currentPoint[0]->x = (x1 + dx);
                currentPoint[0]->y = (y1 + dy);

                currentPoint = nextPoint;
                if (currentPoint[0] != NULL) {
                    nextPoint = currentPoint[0]->nextPoint;
                }
            } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);
        } else {
            return 0;
        }
    } else {
        return 0;
    }

    checkForMinMaxUpdate(path);
    return 1;
}

void fillVector(VectorPath* path, unsigned int fillColor, unsigned int boundaryColor, int offsetX, int offsetY) {
    int isFilling = -1;

    int count = 0;
    determineCriticalPoint(path);
    drawCritPoint(path, offsetX, offsetY, boundaryColor);

    if (checkIfPathIsClosed(path)) {
        for (int j = path->minY-1 + offsetY; j <= path->maxY + offsetY; j++) {
            isFilling = -1;
            for (int i = path->minX + offsetX -1; i <= path->maxX + offsetX; i++) {
                if (getPixelColor(i, j) == critColor) {
                    drawPixel(i,j, boundaryColor);
                    continue;
                } else {
                    if (getPixelColor(i, j) == boundaryColor || getPixelColor(i, j) == critColor) {
                        while(getPixelColor(i, j) == boundaryColor && i <= path->maxX + offsetX && getPixelColor(i, j) != critColor) {
                            i++;
                        }

                        if (getPixelColor(i, j) == critColor) {
                            drawPixel(i,j, boundaryColor);

                        } else {
                            isFilling *= -1;
                        }
                    }
                    if (i <= path->maxX+ offsetX) {
                        if (isFilling > 0) {
                            drawPixel(i, j, fillColor);
                        }
                    }
                }
            }
        }
    }

}

void determineCriticalPoint(VectorPath* vecPath) {

    if (checkIfPathIsClosed(vecPath) == 1) {
        if (vecPath->firstPoint[0] != NULL)
        {
            VectorPoint **currentToCheck = vecPath->firstPoint;
            VectorPoint **nextToCheck = vecPath->firstPoint[0]->nextPoint;
            VectorPoint **prevToCheck = vecPath->firstPoint[0]->prevPoint;

            int nextX, prevX, currentX, nextY, currentY, prevY;
            do
            {    currentToCheck[0]->isCrit = 0;
                nextX = round(nextToCheck[0]->x);
                currentX = round(currentToCheck[0]->x);
                nextY = round(nextToCheck[0]->y);
                currentY = round(currentToCheck[0]->y);
                prevY = round(prevToCheck[0]->y);

                if ((nextY > currentY && prevY > currentY)
                    || (nextY < currentY && prevY < currentY))
                {
                    currentToCheck[0]->isCrit = 1;
                }
                
                if (nextY == currentY) {
                    int nextnextY = round(nextToCheck[0]->nextPoint[0]->y);
                    if ((nextnextY > currentY && prevY > currentY)
                        || (nextnextY < currentY && prevY < currentY))
                        {
                            if (nextX > currentX) {
                                nextToCheck[0]->isCrit = 1;

                            } else {
                                currentToCheck[0]->isCrit = 1;

                            }
                        }
                }

                prevToCheck = currentToCheck;
                currentToCheck = nextToCheck;
                if (currentToCheck[0] != NULL)
                {
                    nextToCheck = currentToCheck[0]->nextPoint;
                }
            } while (currentToCheck[0] != NULL && currentToCheck[0] != vecPath->firstPoint[0]);

        }
        else
        {
            printf("Path is empty\n");
        }

        return;

    } else {
        return;
    }
}

void drawCritPoint(VectorPath* path, int offsetX, int offsetY, unsigned int boundaryColor){
    if (path != NULL) {
        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            do {

                if(currentPoint[0]->isCrit){
                    int i = round(currentPoint[0]->x) + offsetX;
                    int j = round(currentPoint[0]->y)+ offsetY;
                    while(isValidPoint(i,j) && getPixelColor(i,j) == boundaryColor){
                        i++;
                    }    
                    drawPixel(i-1, j, critColor);

                }

                currentPoint = nextPoint;
                if (currentPoint[0] != NULL) {
                    nextPoint = currentPoint[0]->nextPoint;
                }
            } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);
        } else {
            return;
        }
    } else {
        return;
    } 
}


// F4 | Circle functions
void drawCircle(int x0, int y0, int radius, unsigned int boundaryColor, unsigned int fillColor) {
    unsigned int circleFrame[radius * 2][radius * 2];

    int isValidPointOnFrame(int x, int y) {
        if (x >= 0 && x < radius * 2/SCALE && y >=0 && y < radius * 2/SCALE)
            return 1;

        return 0;
    }

    void drawPixelOnFrame(int x, int y, unsigned int color) {
        int i = 0, j = 0;
        x = x*SCALE; y = y*SCALE;
        for (i = 0; i < SCALE; i++)
            for (j = 0; j < SCALE; j++) {
                circleFrame[x+i][y+j] = color;
            }
    }

    unsigned int getPixelColorOnFrame(int x, int y) {
        if(!isValidPointOnFrame(x,y)){
            return 0;
        }
        x = x * SCALE;
        y = y * SCALE;

       return circleFrame[x][y];
    }

    int isCircleCritPoint(int x, int y) {
        return ((y == 1 || y == (2 * radius) - 1));
    }

    int isOnBoundary(int x, int y) {
        return getPixelColorOnFrame(x, y) == boundaryColor;
    }
    void fillCircle() {
        int isFilling = -1;

        for (int j = 0; j <= radius * 2; j++) {
            isFilling = -1;
            for (int i = 0; i <= radius * 2; i++) {
                if (getPixelColorOnFrame(i, j)) {
                    while (isOnBoundary(i, j)) {
                        i++;
                    }
                    isFilling *= -1;
                }

                if (isCircleCritPoint(i, j)) { 
                    continue;
                }

                if (i < radius * 2) {
                    if (isFilling > 0) {
                        drawPixelOnFrame(i, j, fillColor);
                    }
                }

                // if (i == 0 || j == 0 || i == (radius * 2) - 1 || j == (radius * 2) - 1) {
                //     drawPixelOnFrame(i, j, rgbaToInt(255,0,0,0));
                // }
            }
        }
    }

    void drawCircle() {
        int minX = x0 - radius;
        int maxX = x0 + radius;
        int minY = y0 - radius;
        int maxY = y0 + radius;

        for (int x = 0; x < radius * 2; x++) {
            for (int y = 0; y < radius * 2; y++) {
                if (circleFrame[x][y] == boundaryColor || circleFrame[x][y] == fillColor) {
                    drawPixel(x + minX, y + minY, circleFrame[x][y]);
                }
            }
        }
    }

    void clearFrame() {
        for (int i = 0; i < radius * 2; i++) {
            for (int j = 0; j < radius * 2; j++) {
                unsigned int clearColor = rgbaToInt(0,0,0,0);
                if (boundaryColor == clearColor || fillColor == clearColor) {
                    clearColor = rgbaToInt(255,255,255,255);
                } else if (boundaryColor == rgbaToInt(255,255,255,255) || fillColor == rgbaToInt(255,255,255,255)) {
                    clearColor = rgbaToInt(0,255,255,255);
                }
                circleFrame[i][j] = clearColor;
            }
        }
    }

    int centerX = radius;
    int centerY = radius;
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    clearFrame();
    while (x >= y)
    {
        drawPixelOnFrame(centerX + x, centerY + y, boundaryColor);
        drawPixelOnFrame(centerX + y, centerY + x, boundaryColor);
        drawPixelOnFrame(centerX - y, centerY + x, boundaryColor);
        drawPixelOnFrame(centerX - x, centerY + y, boundaryColor);
        drawPixelOnFrame(centerX - x, centerY - y, boundaryColor);
        drawPixelOnFrame(centerX - y, centerY - x, boundaryColor);
        drawPixelOnFrame(centerX + y, centerY - x, boundaryColor);
        drawPixelOnFrame(centerX + x, centerY - y, boundaryColor);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }

    fillCircle();
    drawCircle();
}

// F5 | vector plane functions
int drawVector(char c, int x, int y, unsigned int border_color, unsigned int fill_color, float degree, int originX, int originY, float zoom) {
    // Find vector index
    int i = 0; int found = 0, idx = 0; 
    while (i < letterCount && found == 0) {
        if (letters[i]->name == c) {
            idx = i;
            found = 1;
        }
        i++;
    }

    if (found == 0) {
        return 0;
    }
    int min_X = 9999;
    int min_Y = 9999;
    int max_X = -1;
    int max_Y = -1;
    // Draw Vector border
    for (int j = 0; j < letters[idx]->numOfLines; j++) {

        //create rotated lines
        double x1,y1;
        x1 = letters[idx]->lines[j]->points[0]->x;
        y1 = letters[idx]->lines[j]->points[0]->y;
        //dilatate lines
        x1 = (x1 - originX)*zoom + originX;
        y1 = (y1 - originY)*zoom + originY;

        double t=(22*degree)/(180*7);
        double b1=((x1-originX)*cos(t))-((y1-originY)*sin(t)) + originX;
        double b2=((x1-originX)*sin(t))+((y1-originY)*cos(t)) +originY;

        double x2,y2;
        x2 = letters[idx]->lines[j]->points[1]->x;
        y2 = letters[idx]->lines[j]->points[1]->y;
        //dilatate lines
        x2 = (x2 - originX)*zoom + originX;
        y2 = (y2 - originY)*zoom + originY;
        double c1=((x2-originX)*cos(t))-((y2-originY)*sin(t)) + originX;
        double c2=((x2-originX)*sin(t))+((y2-originY)*cos(t)) +originY;

        //update min/max Y
        if(b1 < min_X)
            min_X = b1;
        if(c1 < min_X)
            min_X = c1; 
        if(b2 < min_Y)
            min_Y = b2;
        if(c2 < min_Y)
            min_Y = c2; 

        if(b1 > max_X)
            max_X = b1;
        if(c1 > max_X)
            max_X = c1; 
        if(b2 > max_Y)
            max_Y = b2;
        if(c2 > max_Y)
            max_Y = c2; 

       struct Line * line = lineInit(b1,b2,c1,c2);
       drawLine_line(line, border_color, x, y);
       freeLine(line);
    }

    fillPlane(letters[idx], fill_color, border_color, x + min_X, y+ min_Y, x + max_X, y+ max_Y );

    return letters[idx]->width;    
};

int checkUp(int i,int j, unsigned int boundaryColor){
    return (getPixelColor(i-1,j-1) == boundaryColor || getPixelColor(i+1,j-1) == boundaryColor || getPixelColor(i,j-1) == boundaryColor);
}
int checkDown(int i,int j, unsigned int boundaryColor){
    return (getPixelColor(i-1,j+1) == boundaryColor || getPixelColor(i,j+1) == boundaryColor || getPixelColor(i+1,j+1) == boundaryColor);
}
int checkLeft(int i, int j, unsigned int boundaryColor){
    return (getPixelColor(i-1,j) == boundaryColor) ;
}

int isCritPoint(int i, int j, unsigned int boundaryColor){
    
    if(getPixelColor(i,j) != boundaryColor){
        return 0;
    }else {

        int up = checkDown(i,j,boundaryColor);
        int down = checkDown(i,j, boundaryColor);
        if(up && down) {
            return 0;
        }
 

        return 1;
    } 

    
}

void fillPlane(struct VecLetter* vecletter, unsigned int color, unsigned int boundaryColor, int minX, int minY, int maxX, int maxY) {
    int isFilling = -1;

    for (int j = minY; j <= maxY; j++) {
        isFilling = -1;
        for (int i= minX; i <= maxX; i++) {
            //crit point
            if (isCritPoint(i, j, boundaryColor)) {
                continue;
            } else
            if (getPixelColor(i, j) == boundaryColor) {
                int initI = i;
                while(getPixelColor(i, j) == boundaryColor && i <= maxX) {
                    i++;
                }

                if (isCritPoint(i, j,boundaryColor)) {

                } else {
                    isFilling *= -1;
                }
            }
            if (i <= maxX) {
                if (isFilling > 0) {
                    drawPixel(i, j, color);
                }
            }
            
        }
    }
}

// Bitmap Font
BitmapFont* initBitmapFont(const char *filename) {
    BitmapFont* bf = (BitmapFont*) malloc(sizeof(BitmapFont));
    
    bf->char_height = 0;
    bf->num_of_char = 0;
    bf->char_index = 0;
    bf->char_width = 0;
    bf->font = 0;

    FILE *fptr;
    size_t buffer_size = 80;
    char *buffer = malloc(buffer_size * sizeof(char));
    int i = 0, ret = 0;

    // Read from external file: filename
    fptr = fopen(filename, "r");
    if (fptr == NULL){
        perror("Cannot open file \n");
        exit(0);
    }

    ret = fscanf(fptr, "%d %d\n", &bf->num_of_char, &bf->char_height);
    if (ret != 2) {
        perror ("Failed to read font specification!\n");
    } else {
        printf("num_of_char: %d\n", bf->num_of_char);
        printf("char_height: %d\n", bf->char_height);
    }
    bf->char_index = (char*) malloc(bf->num_of_char * sizeof(char));
    bf->font = (char**) malloc(bf->num_of_char * sizeof(char*));
    bf->char_width = (int*) malloc(bf->num_of_char * sizeof(int));

    while (-1 != getline(&buffer, &buffer_size, fptr) && 1 < bf->num_of_char) {
        (bf->font)[i] = (char*) malloc(buffer_size * sizeof(char));
        ret = 0; ret = sscanf(buffer, "%c|%d|%[^\n]s", &bf->char_index[i], &bf->char_width[i], bf->font[i]);
        if (ret != 3) {
            printf("Error reading line %d, only read %d\n", i+2, ret);
        }
        i++;
    }

    free(buffer);
    fclose(fptr);

    return bf;
}

int getBitmapCharIndex(BitmapFont *bf, char c) {
    int i = 0;
    for (i = 0; i < bf->num_of_char; i++) {
        if (bf->char_index[i] == c)
            return i;
    }
    return -1;
}

int drawBitmapChar(BitmapFont *bf, int x, int y, char c, int scale) {
    int i = 0, j = 0, k = 0;
    int idx = getBitmapCharIndex(bf, c);
    int width = 4, pixel_length = 0;
    char* pixel = 0;

    if (idx == -1) {
        return 0;
    }
    
    width = bf->char_width[idx];
    pixel = bf->font[idx];
    pixel_length = strlen(pixel);

    x = x / scale; y = y / scale;

    for (j = 0; j < bf->char_height+2; j++)
        for (i = 0; i < width+2; i++) {
            if ((i == 0) || (i == width+1) || (j == 0) || (j == bf->char_height+1))
                continue;
            else {
                if (k >= pixel_length) {
                    continue;
                }
                if (pixel[k] == '1')
                    drawPixelWithScale(i+x, j+y, rgbaToInt(255,255,255,0), scale);
                k++;
            }
        }

    return (width + 2) * scale;
}

void drawBitmapString(BitmapFont *bf, int x, int y, char* text, int scale) {
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        x += drawBitmapChar(bf,x,y,text[i], scale);
    }
}

// Mouse

Mouse* initMouse(int screen_min_x, int screen_min_y, int screen_max_x, int screen_max_y, int speed) {
    Mouse* m = (Mouse*) malloc(sizeof(Mouse));
    
    m->fd = open(device_mouse, O_RDWR);
    if(m->fd == -1) {
        printf("ERROR Opening %s\n", device_mouse);
        return 0;
    }

    m->screen_max_x = screen_max_x;
    m->screen_max_y = screen_max_y;
    m->screen_min_x = screen_min_x;
    m->screen_min_y = screen_min_y;
    m->speed = speed;

    m->isEvent = 0;
    m->positionX = screen_min_x;
    m->positionY = screen_min_y;
    m->isRightClick = 0;
    m->isLeftClick = 0;

    return m;
}

void scanMouse(Mouse* m) {
    int bytes, left, right, middle;
    signed char x, y;
    unsigned char data[3];

    bytes = read(m->fd, data, sizeof(data));

    if(bytes > 0) {
        left = data[0] & 0x1;
        right = data[0] & 0x2;
        middle = data[0] & 0x4;

        x = data[1];
        y = data[2];
        
        // printf("x=%d, y=%d, left=%d, middle=%d, right=%d\n", x, y, left, middle, right);
        if (x < 0)
            m->positionX -= m->speed;
        if (x > 0)
            m->positionX += m->speed;
        if (y > 0)
            m->positionY -= m->speed;
        if (y < 0)
            m->positionY += m->speed;

        if (m->positionY < m->screen_min_y) m->positionY = m->screen_min_y;
        if (m->positionX < m->screen_min_x) m->positionX = m->screen_min_x;
        if (m->positionY > m->screen_max_y) m->positionY = m->screen_max_y;
        if (m->positionX > m->screen_max_x) m->positionX = m->screen_max_x;

        if (right > 0) m->isRightClick = 1; else m->isRightClick = 0;
        if (left > 0) m->isLeftClick = 1; else m->isLeftClick = 0;

        m->isEvent = 1;
    } else {
        m->isEvent = 0;
    }
}

int scanLeftClick(Mouse* m) {
    int bytes, left, right, middle;
    signed char x, y;
    unsigned char data[3];

    bytes = read(m->fd, data, sizeof(data));

    return bytes > 0;
}