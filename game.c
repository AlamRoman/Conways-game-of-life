#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define width_board 75
#define height_board 75
#define UNIT 8
#define GENERATION_PER_SECOND 5
#define TARGET_FPS 60
#define MAX_HISTORY 10
#define MAX_PUNTI_UNDO 1000

#define screenWidth 1150
#define screenHeight 650

typedef struct punti{
    int x;
    int y;
    int ultimoStato;
}Punti;

float SCALE_X=1,SCALE_Y=1;

Punti history[MAX_HISTORY][MAX_PUNTI_UNDO];
int history_size_gruppi[MAX_HISTORY];
int size_history=0;

int arr[width_board][height_board];

bool start = false;

Color coloreNero = {24,28,19,255};
Color coloreAzzurro = {203, 245, 237, 255};

void riempi_arr(){
    int i,j;

    for ( i = 0; i < width_board; i++)
    {
        for(j=0;j<height_board;j++){
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
    int temp[width_board][height_board];

    for ( i = 0; i < width_board; i++)
    {
        for(j=0;j<height_board;j++){
            temp[i][j]=arr[i][j];
        }
    }

    for ( i = 1; i < width_board-1; i++)
    {
        for(j=1;j<height_board-1;j++){
            n=count_neighbours(i,j);
            //alive or dead
            if(n<2 || n > 3){
                temp[i][j]=0;
            }else if(n == 3){
                temp[i][j]=1;
            }
        }
    }

    for ( i = 0; i < width_board; i++)
    {
        for(j=0;j<height_board;j++){
            arr[i][j]=temp[i][j];
        }
    }
}

void full_screen(){

    if (IsWindowFullscreen())
    {
        //window
        ToggleFullscreen();
        SetWindowSize(screenWidth,screenHeight);

        SCALE_X=1;
        SCALE_Y=1;
    }else
    {
        //fullscreen
        int monitor=GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor),GetMonitorHeight(monitor));
        ToggleFullscreen();

        SCALE_X=(float)GetMonitorWidth(monitor)/screenWidth;
        SCALE_Y=(float)GetMonitorHeight(monitor)/screenHeight;
    }
}

int main(){
    InitWindow(screenWidth,screenHeight,"Game of life");
    SetTargetFPS(TARGET_FPS);

    //set window icon
    Image icon;
    icon = LoadImage("img/icon.png");
    SetWindowIcon(icon);

    Image cursorImage = LoadImage("img/pen_cursor.png");
    ImageResize(&cursorImage,30,30);
    Texture cursor = LoadTextureFromImage(cursorImage);

    int i,j;

    int pos_x_iniziale=(int)GetScreenWidth()*0.05;
    int pos_y_iniziale=(int)GetScreenHeight()*0.04;

    Rectangle board={pos_x_iniziale,pos_y_iniziale,width_board*UNIT*SCALE_X,height_board*UNIT*SCALE_Y};

    //reset array
    riempi_arr();

    int frameCount = TARGET_FPS / GENERATION_PER_SECOND;

    //punti
    Punti *gruppo_punti;
    int size_gruppo_punti=0;
    gruppo_punti = (Punti*)malloc(size_gruppo_punti*sizeof(Punti));

    while(!WindowShouldClose()){

        //fullscreen
        if (IsKeyPressed(KEY_F))
        {
            full_screen();

            pos_x_iniziale=(int)GetScreenWidth()*0.05;
            pos_y_iniziale=(int)GetScreenHeight()*0.04;

            board.width=width_board*UNIT*SCALE_X;
            board.height=height_board*UNIT*SCALE_Y;
            board.x=pos_x_iniziale;
            board.y=pos_y_iniziale;

            if (IsWindowFullscreen())
            {
                ImageResize(&cursorImage,30*SCALE_X,30*SCALE_Y);
                cursor = LoadTextureFromImage(cursorImage);
            }else
            {
                ImageResize(&cursorImage,30,30);
                cursor = LoadTextureFromImage(cursorImage);
            }  
        }

        if(start){
            //se frameCount è uguale a zero allora passa alla prossima generazione
            if(!frameCount){
                next_generation();
                frameCount = TARGET_FPS / GENERATION_PER_SECOND;
            }
            frameCount--;
        }else {
            if(CheckCollisionPointRec(GetMousePosition(),board) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){


                int x = (int)(GetMouseY()-pos_y_iniziale)/UNIT/SCALE_X;
                int y = (int)(GetMouseX()-pos_x_iniziale)/UNIT/SCALE_Y;

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
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(),board)){

            int n_punti_da_salvare;
            if(size_gruppo_punti>MAX_PUNTI_UNDO){
                //da stampare che è stato raggiunto il numero massimo di punti cancellabili
                n_punti_da_salvare=MAX_PUNTI_UNDO;
            }else{
                n_punti_da_salvare=size_gruppo_punti;
            }

            for(i=0;i<n_punti_da_salvare;i++){
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

        }

        //reset array
        if(IsKeyPressed(KEY_R) && !start){
            riempi_arr();
            //reset anche history
            size_history=0;
        }

        BeginDrawing();
            ClearBackground(coloreAzzurro);

            for ( i = 0; i < height_board; i++)
            {
                for(j=0;j<width_board;j++){

                    if(arr[i][j]){
                        //live cell
                        DrawRectangle(pos_x_iniziale+j*UNIT*SCALE_X,pos_y_iniziale+i*UNIT*SCALE_Y,UNIT*SCALE_X,UNIT*SCALE_Y,WHITE);
                    }else{
                        //dead cell
                        DrawRectangle(pos_x_iniziale+j*UNIT*SCALE_X,pos_y_iniziale+i*UNIT*SCALE_Y,(UNIT+1)*SCALE_X,(UNIT+1)*SCALE_Y,BLACK);
                        DrawRectangleLines(pos_x_iniziale+j*UNIT*SCALE_X,pos_y_iniziale+i*UNIT*SCALE_Y,UNIT*SCALE_X,UNIT*SCALE_Y,coloreNero);
                    }

                }
            }
            DrawRectangleLinesEx((Rectangle){pos_x_iniziale,pos_y_iniziale,(width_board*UNIT*SCALE_X)+1,(height_board*UNIT*SCALE_Y)+1},UNIT*((SCALE_X+SCALE_Y)/2),GRAY);

            if(CheckCollisionPointRec(GetMousePosition(),board) && IsCursorOnScreen()){
                HideCursor();
                if (IsWindowFullscreen())
                {
                    DrawTexture(cursor,GetMouseX()-UNIT/SCALE_X,GetMouseY()-UNIT/SCALE_Y,WHITE);
                }else
                {
                    DrawTexture(cursor,GetMouseX(),GetMouseY(),WHITE);
                }
                
            }else
            {
                ShowCursor();
            }

        EndDrawing();
    }

    UnloadTexture(cursor);
    CloseWindow();

    return 0;
}
