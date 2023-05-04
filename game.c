#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define width 75
#define height 75
#define UNIT 8
#define GENARATION_PER_SECOND 4
#define TARGET_FPS 60

#define screenWidth 1150
#define screenHeight 650

int arr[width][height];

bool start = false;

Color nero = {24,28,19,255};

void riempi_arr(){
    int i,j;

    for ( i = 0; i < width; i++)
    {
        for(j=0;j<height;j++){
            arr[i][j]=0;
        }
    }

}

int count_neighbours(int i,int j){
    int count=0;

    if(arr[i-1][j-1]){
        count++;
    }
    if(arr[i-1][j]){
        count++;
    }
    if(arr[i-1][j+1]){
        count++;
    }
    if(arr[i][j-1]){
        count++;
    }
    if(arr[i][j+1]){
        count++;
    }
    if(arr[i+1][j-1]){
        count++;
    }
    if(arr[i+1][j]){
        count++;
    }
    if(arr[i+1][j+1]){
        count++;
    }
    return count;
}

void next_generation(){
    int i,j,n;
    int temp[width][height];

    for ( i = 0; i < width; i++)
    {
        for(j=0;j<height;j++){
            temp[i][j]=arr[i][j];
        }
    }

    for ( i = 1; i < width-1; i++)
    {
        for(j=1;j<height-1;j++){
            n=count_neighbours(i,j);
            //alive or dead
            if(n<2 || n > 3){
                temp[i][j]=0;
            }else if(n == 3){
                temp[i][j]=1;
            }
        }
    }

    for ( i = 0; i < width; i++)
    {
        for(j=0;j<height;j++){
            arr[i][j]=temp[i][j];
        }
    }
}

void check_pause_start(){
    if(IsKeyPressed(KEY_SPACE)){
            start=!start;
    }
}

int main(){
    InitWindow(screenWidth,screenHeight,"Game of life");
    SetTargetFPS(TARGET_FPS);

    int i,j;

    int pos_x_iniziale=50;
    int pos_y_iniziale=25;

    Rectangle canvas={pos_x_iniziale,pos_y_iniziale,width*UNIT,height*UNIT};

    riempi_arr();

    int last_cell_x;
    int last_cell_y;

    int delay = TARGET_FPS / GENARATION_PER_SECOND;

    bool vuoto=true;

    while(!WindowShouldClose()){

        if(start){
            //se delay � uguale a zero allora passa alla prossima generazione
            if(!delay){
                next_generation();
                delay = TARGET_FPS / GENARATION_PER_SECOND;
            }
            delay--;
        }else {
            if(CheckCollisionPointRec(GetMousePosition(),canvas) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                int current_cell_x = (int)(GetMouseY()-pos_y_iniziale)/UNIT;
                int current_cell_y = (int)(GetMouseX()-pos_x_iniziale)/UNIT;

                if (arr[current_cell_x][current_cell_y]!=1)
                {
                    last_cell_x=current_cell_x;
                    last_cell_y=current_cell_y;
                }

                arr[current_cell_x][current_cell_y]=1;
                vuoto=false;
            }
        }

        check_pause_start();

        if(IsKeyPressed(KEY_Z) && !vuoto){
            arr[last_cell_x][last_cell_y]=0;
        }

        //reset array
        if(IsKeyPressed(KEY_R) && !start){
            riempi_arr();
            vuoto=true;
        }

        BeginDrawing();
            ClearBackground(WHITE);
            DrawRectangleRec(canvas,RED);

            for ( i = 0; i < height; i++)
            {
                for(j=0;j<width;j++){
                    if(arr[i][j]){
                        //live cell
                        DrawRectangle(pos_x_iniziale+j*UNIT,pos_y_iniziale+i*UNIT,UNIT,UNIT,WHITE);
                    }else{
                        //dead cell
                        DrawRectangle(pos_x_iniziale+j*UNIT,pos_y_iniziale+i*UNIT,UNIT,UNIT,BLACK);
                        DrawRectangleLines(pos_x_iniziale+j*UNIT,pos_y_iniziale+i*UNIT,UNIT,UNIT,nero);
                    }

                }
            }
            DrawRectangleLinesEx(canvas,UNIT,GRAY);

        EndDrawing();
    }
    CloseWindow();

    return 0;
}
