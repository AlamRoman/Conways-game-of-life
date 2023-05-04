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
#define MAX_HISTORY 10
#define MAX_PUNTI_UNDO 100

#define screenWidth 1150
#define screenHeight 650

typedef struct punti{
    int x;
    int y;
    int ultimoStato;
}Punti;

Punti history[MAX_HISTORY][MAX_PUNTI_UNDO];
int history_size_gruppi[MAX_HISTORY];
int size_history=0;

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

int main(){
    InitWindow(screenWidth,screenHeight,"Game of life");
    SetTargetFPS(TARGET_FPS);

    int i,j;

    int pos_x_iniziale=50;
    int pos_y_iniziale=25;

    Rectangle canvas={pos_x_iniziale,pos_y_iniziale,width*UNIT,height*UNIT};

    riempi_arr();


    int delay = TARGET_FPS / GENARATION_PER_SECOND;

    //punti
    Punti *gruppo_punti;
    int size_gruppo_punti=0;
    gruppo_punti = (Punti*)malloc(size_gruppo_punti*sizeof(Punti));

    while(!WindowShouldClose()){

        if(start){
            //se delay è uguale a zero allora passa alla prossima generazione
            if(!delay){
                next_generation();
                delay = TARGET_FPS / GENARATION_PER_SECOND;
            }
            delay--;
        }else {
            if(CheckCollisionPointRec(GetMousePosition(),canvas) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){


                int x = (int)(GetMouseY()-pos_y_iniziale)/UNIT;
                int y = (int)(GetMouseX()-pos_x_iniziale)/UNIT;

                if(arr[x][y]==0){
                    size_gruppo_punti++;
                    gruppo_punti = (Punti*)realloc(gruppo_punti,size_gruppo_punti*sizeof(Punti));
                    gruppo_punti[size_gruppo_punti-1].x = x;
                    gruppo_punti[size_gruppo_punti-1].y = y;
                    gruppo_punti[size_gruppo_punti-1].ultimoStato = arr[x][y];
                }

                arr[x][y]=1;
            }
        }

        //aggiunge il gruppo di punti nella history
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(),canvas)){
            printf("Array creato, size history %d\n",size_history);
            if(size_gruppo_punti>MAX_PUNTI_UNDO){
                size_gruppo_punti=MAX_PUNTI_UNDO;
            }

            for(i=0;i<size_gruppo_punti;i++){
                history[size_history][i]=gruppo_punti[size_gruppo_punti-i-1];
            }

            history_size_gruppi[size_history]=size_gruppo_punti;

            //se la lunghezza della history è maggiore al massimo, allora cancello
            //il primo elemento e faccio il shift dei elementi (FIFO)
            if(size_history>=MAX_HISTORY-1){

                for(i=0;i<MAX_HISTORY-1;i++){
                    for(j=0;j<history_size_gruppi[i+1];j++){
                        history[i][j]=history[i+1][j];
                    }
                }
            }else{
                size_history++;
            }
            size_gruppo_punti=0;
        }

        if(IsKeyPressed(KEY_SPACE)){
            start=!start;
            //se la simulazione è partita, allora non si puo fare undo dei punti disegnati
            size_history=0;
        }

        //undo
        if(IsKeyPressed(KEY_Z)){

            if(size_history<=0){
                //da modificare e stampare in grafica
                printf("\nLimite undo raggiunto\n");
            }

            //modifica l'array con l'ultimo stato dei punti
            for(i=0;i<history_size_gruppi[size_history-1];i++){
                arr[history[size_history-1][i].x][history[size_history-1][i].y]=history[size_history-1][i].ultimoStato;
            }

            if(size_history>0){
                size_history--;
            }
            printf("\nn : %d\n",size_history);
        }

        //reset array
        if(IsKeyPressed(KEY_R) && !start){
            riempi_arr();
            //reset anche history
            size_history=0;
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
