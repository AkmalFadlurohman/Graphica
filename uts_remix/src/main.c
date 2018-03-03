#include "headers/fbp.h"

#define MOUSE_SPEED 4

const char *template_bitmap_font = "data/template_bitmap_font.io";

void drawMainMenu(BitmapFont* bf, int x, int y);
void closeProgram(BitmapFont* bf, int mouse_y);
void showVecLetters(BitmapFont* bf, int mouse_y);
void show_plane1(BitmapFont* bf, int mouse_y);
void show_plane2(BitmapFont* bf, int mouse_y);
void show_plane3(BitmapFont* bf, int mouse_y);
void openMap(BitmapFont* bf, int mouse_y);

int window_x;
int window_y;
int max_window_x;
int max_window_y;

Mouse *mouse;

int main(int argc, char **argv) {
	initializeFBP();
    viewport_x = 200;
    viewport_y = 500;

    window_x = (vinfo.xres - viewport_width) / 2;
    window_y = (vinfo.yres - viewport_height) / 2;
    max_window_x = window_x + viewport_width;
    max_window_y = window_y + viewport_height;

    mouse = initMouse(window_x, window_y, max_window_x, max_window_y, MOUSE_SPEED);
    if (mouse == 0) {
        return 0;
    }

    BitmapFont* bitmapFont = initBitmapFont(template_bitmap_font);

    critColor = rgbaToInt(250,250,250,0);
    frameColor = rgbaToInt(247,247,247,0);

    
    while(RUNNING) {
        scanMouse(mouse);
        if(mouse->isEvent) {
            clearScreen();
            clearViewPort(rgbaToInt(25,25,25,25));
            drawMainMenu(bitmapFont, window_x, window_y);
            drawPointer(mouse);
            if (mouse->isRightClick) {
                mouse->positionY = window_y;
            }
            if (mouse->isLeftClick) {
                // showVecLetters(bitmapFont, mouse->positionY);
                show_plane1(bitmapFont, mouse->positionY);
                show_plane2(bitmapFont, mouse->positionY);
                show_plane3(bitmapFont, mouse->positionY);
                openMap(bitmapFont, mouse->positionY);
                closeProgram(bitmapFont, mouse->positionY);
            }
        }
    }
    printf("bye!\n");

    munmap(fbp, screensize);
    close(fbfd);
}

void closeProgram(BitmapFont* bf, int mouse_y) {
    int line_height_5 = bf->char_height*5;
    int line_height_3 = bf->char_height*3;

    int upperBound = window_y + 2*line_height_5 + 6*line_height_3;
    int lowerBound = window_y + 2*line_height_5 + 7*line_height_3;
    
    if (mouse_y < lowerBound && mouse_y > upperBound) {
        clearViewPort(rgbaToInt(0,0,0,0));
        clearScreen();
        render();
        exit(0);
    }
}

void drawMainMenu(BitmapFont* bf, int x, int y) {
    int line_height_5 = bf->char_height*5;
    int line_height_3 = bf->char_height*3;
    drawBitmapString(bf, x, y, "MENU UTAMA", 5);
    drawBitmapString(bf, x, y + 2*line_height_5, " > FONT", 3);
    drawBitmapString(bf, x, y + 2*line_height_5 + line_height_3, " > ANIMASI PESAWAT 1", 3);
    drawBitmapString(bf, x, y + 2*line_height_5 + 2*line_height_3, " > ANIMASI PESAWAT 2", 3);
    drawBitmapString(bf, x, y + 2*line_height_5 + 3*line_height_3, " > ANIMASI PESAWAT 3", 3);
    drawBitmapString(bf, x, y + 2*line_height_5 + 4*line_height_3, " > PETA ITB", 3);
    drawBitmapString(bf, x, y + 2*line_height_5 + 6*line_height_3, "KELUAR", 2);
}

void showVecLetters(BitmapFont* bf, int mouse_y) {
    viewport_x = 0;
    viewport_y = 0;
    int line_height_5 = bf->char_height*5;
    int line_height_3 = bf->char_height*3;

    int upperBound = window_y + 2*line_height_5;
    int lowerBound = window_y + 2*line_height_5 + line_height_3;
    
    if (mouse_y < lowerBound && mouse_y > upperBound) {
        clearScreen();
        clearViewPort(rgbaToInt(0,0,0,0));
        
        render();

        loadLetters("assets/VecLetterSpec.txt");    

        char input[100];
        printf("%s: ", "Masukkan input");
        scanf("%99[0-9a-zA-Z ]", input);

        for(int i = 0; input[i]; i++) {
            input[i] = toupper(input[i]);
        }

        for (int i=0; i<vinfo.yres/17; i++) {
          printf("\n");
        }
        printf("\n");

        int offsetX = MARGIN_HORIZONTAL;
        int offsetY = MARGIN_VERTICAL;
        system("clear");

        COLOR = rgbaToInt(255, 225, 0, 0);
        BORDER_COLOR = rgbaToInt(255, 0, 0, 0);
        
        for (int i=0;i<strlen(input);i++) {
            if (input[i] == ' ') {
                offsetX += MARGIN_HORIZONTAL*4;
                continue;
            }
            drawLetters(input[i], &offsetX, &offsetY);
        }

        char c = 0;
        while (RUNNING && c != ESC) {
            render();
            scanf("%c", &c);
        }
    }

    return;
}

void show_plane1(BitmapFont* bf, int mouse_y) {
    int line_height_5 = bf->char_height*5;
    int line_height_3 = bf->char_height*3;

    int upperBound = window_y + 2*line_height_5 + line_height_3;
    int lowerBound = window_y + 2*line_height_5 + 2*line_height_3;
    
    if (mouse_y < lowerBound && mouse_y > upperBound) {
    	viewport_x = 200;
        viewport_y = 500;
    	char* fileName = "assets/plane_bitmap.txt";

    	struct f_Image* plane = f_loadImage(fileName);
        plane->posX = vinfo.xres;
        int t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0, t6 = 0;
        centerX = vinfo.xres/2/SCALE, fullY = vinfo.yres/SCALE - 1;
        int delay = 0;
    	
    	system("clear");

        int stop = 0;
        while (RUNNING && stop == 0) {
            plane->posX--;
            if (plane->posX < -plane->width) {
                plane->posX = vinfo.xres;
                plane->posY += 7;
            }
            drawObject(plane, 1);

            drawLaser(centerX, fullY, 10, -10, SCALE, &t1);
            drawLaser(vinfo.xres/18, vinfo.yres/9-1, 5, -3, 9, &t2);
            drawLaser(centerX, fullY, 0, -10, SCALE, &t3);
            drawLaser(centerX, fullY, -10, -10, SCALE, &t4);
            drawLaser(vinfo.xres/20, vinfo.yres/10-1, -5, -10, 10, &t5);
            drawLaser(centerX, fullY, -3, -2, SCALE, &t6);
            if (delay %10 == 0) {
                t1++; t2++; t3++; t4++; t5++; t6++;
            }
            delay++;

            usleep(3000);
        }
        f_freeImage(plane);
    }
}

void show_plane2(BitmapFont* bf, int mouse_y) {

    int line_height_5 = bf->char_height*5;
    int line_height_3 = bf->char_height*3;

    int upperBound = window_y + 2*line_height_5 + 2*line_height_3;
    int lowerBound = window_y + 2*line_height_5 + 3*line_height_3;

    if (mouse_y < lowerBound && mouse_y > upperBound) {

    	viewport_x = 0;
        viewport_y = 0;
    	loadLetters("assets/plane2.txt");

        for (int i=0; i<vinfo.yres/17; i++) {
          printf("\n");
        }
        printf("\n");
        int f;
        int marginX = 150*SCALE;
        int posX = marginX;
        int posY = vinfo.yres/2;
        system("clear");
        float degree = 10;
        float wingDeg = 0;
        float zoom = 1;
        while (RUNNING) {
            clearScreen();
            clearViewPort(rgbaToInt(135,206,250,0));
            wingDeg+=10;
            if(posX < vinfo.xres/(1.5*SCALE) && zoom == 1){
                if(degree < 10){
                    degree+=1;

                } else if(degree == 10){
                    posX+=4;

                }else{
                    degree = 10;
                }
            }
            if(posX >= vinfo.xres/(1.5*SCALE) && zoom < 3){
                if(degree > 0){
                    degree-=1;
                    posX+=5;

                } else if(degree == 0){
                    zoom+= 0.025;

                }else{
                    degree = 0;
                }
            }
            if(zoom >= 3 && posX >marginX){
                if(degree > -10){
                    degree-=1;
                    posX-=6;

                } else if(degree == -10){
                    posX-=8;

                }else{
                    degree = -10;
                }
          

            }
            if(posX < marginX){
                posX = marginX;
            }
            if(posX == marginX && zoom !=1){
                if(degree < 0){
                    degree+=1;

                } else if(degree == 0){
                    zoom-= 0.025;

                }else{
                    degree = 0;
                }
            }
            if(zoom < 1){
                zoom =1;
            }
            drawVector('C', posX, posY, rgbaToInt(0,0,255,0), rgbaToInt(0,0,150,0), degree, 50, 50, zoom);
            drawVector('B', posX, posY, rgbaToInt(0,255,0,0), rgbaToInt(0,150,0,0), degree, 50, 50, zoom);
            drawVector('A', posX, posY, rgbaToInt(255,0,0,0), rgbaToInt(150,0,0,0), degree, 50, 50, zoom);
            drawVector('D', posX, posY, rgbaToInt(0,255,255,0), rgbaToInt(0,150,150,0),wingDeg, 50, 60, zoom);
            render();
            usleep(30000);
       }
    }
}

void show_plane3(BitmapFont* bf, int mouse_y) {

    int line_height_5 = bf->char_height*5;
    int line_height_3 = bf->char_height*3;

    int upperBound = window_y + 2*line_height_5 + 3*line_height_3;
    int lowerBound = window_y + 2*line_height_5 + 4*line_height_3;

    if (mouse_y < lowerBound && mouse_y > upperBound) {
        viewport_x = 200;
        viewport_y = 500;
        // Initialize vector objects
        VectorPath* badan_bawah = createVectorPathFromFile("assets/plane3/badan_bawah.txt");
        if (badan_bawah == NULL) {
            printf("Failed to load badan bawah\n");
            return;
        }
        VectorPath* sayap_utama = createVectorPathFromFile("assets/plane3/sayap.txt");
        if (sayap_utama == NULL) {
            printf("Failed to load sayap utama\n");
            return;
        }
        VectorPath* sayap_belakang = createVectorPathFromFile("assets/plane3/sayap_belakang.txt");
        if (sayap_belakang == NULL) {
            printf("Failed to load sayap belakang\n");
            return;
        }
        VectorPath* baling_baling = createVectorPathFromFile("assets/plane3/baling2.txt");
        if (baling_baling == NULL) {
            printf("Failed to load baling-baling\n");
            return;
        }

        int count = 0;
        int dx = 10;

        void translatePlane() {
            translatePath(badan_bawah, 200, 500);
            translatePath(sayap_belakang, 200, 500);
            translatePath(sayap_utama, 200, 500);
            translatePath(baling_baling, 200, 500);
        }


        dilatatePath(badan_bawah, 50, 50, 2);
        dilatatePath(sayap_belakang, 50, 50, 2);
        dilatatePath(sayap_utama, 50, 55, 2);
        dilatatePath(baling_baling, 50, 60, 2);

        clearScreen();

        char c = 0;
        // Start animation and render
        system("/bin/stty raw");

        while (RUNNING && (c=getchar()) != 27) {

            clearViewPort(rgbaToInt(135,206,250,0));
            rotatePath(baling_baling, 10,  50, 60);

            // drawVectorPathClipping(sayap_belakang, rgbaToInt(0,0,0,0),rgbaToInt(107,107,107,0), 500, 500);
            // drawVectorPathClipping(badan_bawah, rgbaToInt(2,2,2,0),rgbaToInt(48,60,165,0), 500, 500);
            // drawVectorPathClipping(sayap_utama, rgbaToInt(1,1,1,0),rgbaToInt(196,0,0,0), 500, 500);
            // drawVectorPathClipping(baling_baling, rgbaToInt(3,3,3,0),rgbaToInt(102,66,0,0), 500, 500);

            drawVectorPath(sayap_belakang, rgbaToInt(0,0,0,0),rgbaToInt(107,107,107,0), 500, 500);
            drawVectorPath(badan_bawah, rgbaToInt(2,2,2,0),rgbaToInt(48,60,165,0), 500, 500);
            drawVectorPath(sayap_utama, rgbaToInt(1,1,1,0),rgbaToInt(196,0,0,0), 500, 500);
            drawVectorPath(baling_baling, rgbaToInt(3,3,3,0),rgbaToInt(102,66,0,0), 500, 500);

            render();

            if(c == 'w' || c == 'W'){
                viewport_y -= VIEWPORT_SPEED;
            } else if(c == 'a' || c == 'A'){
                viewport_x -= VIEWPORT_SPEED;
            } else if(c == 's' || c == 'S'){
                viewport_y += VIEWPORT_SPEED;
            } else if(c == 'd' || c == 'D'){
                viewport_x += VIEWPORT_SPEED;
            } else if(c == 'z' || c == 'Z'){
                dilatatePath(badan_bawah, 50, 50, 1.1);
                dilatatePath(sayap_belakang, 50, 50, 1.1);
                dilatatePath(sayap_utama, 50, 55, 1.1);
                dilatatePath(baling_baling, 50, 60, 1.1);
            } else if(c == 'x' || c == 'X'){
                dilatatePath(badan_bawah, 50, 50, 0.9);
                dilatatePath(sayap_belakang, 50, 50, 0.9);
                dilatatePath(sayap_utama, 50, 55, 0.9);
                dilatatePath(baling_baling, 50, 60, 0.9);
            }  else if (c == ESC) {
                freeVectorPath(sayap_belakang);
                freeVectorPath(badan_bawah);
                freeVectorPath(sayap_utama);
                freeVectorPath(baling_baling);
            }

            if(viewport_x < 0)
                viewport_x = 0;
            if(viewport_y < 0)
                viewport_y = 0;
            if(viewport_x > WORLD_WIDTH - viewport_width)
                viewport_x = WORLD_WIDTH - viewport_width;
            if(viewport_y > WORLD_HEIGHT - viewport_height)
                viewport_y = WORLD_HEIGHT - viewport_height;
            usleep(3000);
        }
        system("/bin/stty cooked");
    }
}

void openMap(BitmapFont* bf, int mouse_y) {
    int line_height_5 = bf->char_height*5;
    int line_height_3 = bf->char_height*3;

    int upperBound = window_y + 2*line_height_5 + 4*line_height_3;
    int lowerBound = window_y + 2*line_height_5 + 5*line_height_3;

    if (mouse_y < lowerBound && mouse_y > upperBound) {
    	viewport_x = 200;
        viewport_y = 500;
    	int renderRoad = 1;
        int renderBuilding = 1;

        int numOfGedung = 25;
        VectorPath** gedung = createVectorPathFromSVG("assets/map_buildings.txt", numOfGedung);
        VectorPath** jalan = createVectorPathFromSVG("assets/map_roads.txt", 1);

    	char c = 0;
    	while (RUNNING && c != ESC) {
    		clearScreen();
            clearViewPort(rgbaToInt(25,25,25,25));
            if (renderBuilding == 1) {
                for (int i = 0; i < numOfGedung; i++) {
                    if (i % 3 == 0) {
                        drawVectorPath(gedung[i], rgbaToInt(255,255,200 + i,0), rgbaToInt(0,0,100 + i * 5,0), 0, 0);
                    } else if (i % 3 == 1) {
                        drawVectorPath(gedung[i], rgbaToInt(255,255,200 + i,0), rgbaToInt(0,100 + i * 5,0,0), 0, 0);
                    } else {
                        drawVectorPath(gedung[i], rgbaToInt(255,255,200 + i,0), rgbaToInt(100 + i * 5,0,0,0), 0, 0);
                    }
                }

                drawCircle(340, 580, 10, rgbaToInt(255,0,0,0), rgbaToInt(0,0,255,0));
                drawCircle(340, 470, 15, rgbaToInt(9,255,0,0), rgbaToInt(0,255,0,0));
            }

            if (renderRoad == 1) {
                drawVectorPath(jalan[0], rgbaToInt(255,255,199,0), rgbaToInt(100,100,100,0), 0, 0);
            }

            render();

            scanf("%c", &c);
            if(c == 'w' || c == 'W'){
                viewport_y -= VIEWPORT_SPEED;
            } else if(c == 'a' || c == 'A'){
                viewport_x -= VIEWPORT_SPEED;
            } else if(c == 's' || c == 'S'){
                viewport_y += VIEWPORT_SPEED;
            } else if(c == 'd' || c == 'D'){
                viewport_x += VIEWPORT_SPEED;
            } else if (c == ESC) {
            	for (int i = 0; i < numOfGedung; i++) {
            		freeVectorPath(gedung[i]);
                }
            	freeVectorPath(jalan[0]);

            	free(gedung);
            	free(jalan);
            }
    	}
    }

}