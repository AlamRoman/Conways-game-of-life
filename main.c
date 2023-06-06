#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define width_board 75
#define height_board 75
#define UNIT 8
#define GENERATION_PER_SECOND 7
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

typedef struct button{
    Image image;
    Texture texture;
    int posX;
    int posY;
    int sizeX;
    int sizeY;
    Rectangle rect;
    bool isPressed;
}Button;

float SCALE_X=1,SCALE_Y=1;

Punti history[MAX_HISTORY][MAX_PUNTI_UNDO];
int history_size_gruppi[MAX_HISTORY];
int size_history=0;

int arr[width_board][height_board];

bool start = false;

int time_count=0;
int genaration_count=0;
int population=0;

Color coloreNero = {24,28,19,255};
Color coloreBianco = {222, 255, 254, 255};

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

    population=0;

    for ( i = 0; i < width_board; i++)
    {
        for(j=0;j<height_board;j++){
            temp[i][j]=arr[i][j];

            if(arr[i][j]==1){
                population++;
            }
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

    genaration_count++;
}

void full_screen(){

    int monitor=GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(monitor),GetMonitorHeight(monitor));
    ToggleFullscreen();

    SCALE_X=(float)GetMonitorWidth(monitor)/screenWidth;
    SCALE_Y=(float)GetMonitorHeight(monitor)/screenHeight;
}

int main(){
    InitWindow(screenWidth,screenHeight,"Game of life");
    SetTargetFPS(TARGET_FPS);

    full_screen();

    InitAudioDevice();
	Music music = LoadMusicStream("music/music.mp3");
	SetMusicVolume(music, 0.1);
	PlayMusicStream(music);

    int i,j;

    int pos_x_iniziale=(int)GetScreenWidth()*0.05;
    int pos_y_iniziale=(int)GetScreenHeight()*0.04;

    int frame_count = 0;

    char error_message[100];
    int error_message_time=0;

    bool show_start_screen=true;

    Rectangle board={pos_x_iniziale,pos_y_iniziale,width_board*UNIT*SCALE_X,height_board*UNIT*SCALE_Y};

    //reset array
    riempi_arr();

    int time_until_next_gen = TARGET_FPS / GENERATION_PER_SECOND;

    //punti
    Punti *gruppo_punti;
    int size_gruppo_punti=0;
    gruppo_punti = (Punti*)malloc(size_gruppo_punti*sizeof(Punti));

    //set window icon
    Image icon;
    icon = LoadImage("img/icon.png");
    SetWindowIcon(icon);

    //load pen cursor texture
    Image cursorImage = LoadImage("img/pen_cursor.png");
    ImageResize(&cursorImage,30*SCALE_X,30*SCALE_Y);
    Texture cursor_pen = LoadTextureFromImage(cursorImage);

    Font font_subway = LoadFontEx("font/SUBWAY.ttf", 96, 0, 0);
    Font font_arcade = LoadFontEx("font/ARCADE.TTF", 96, 0, 0);

    //Buttons

    //start button
    Button btn_start;
    btn_start.image = LoadImage("img/start.png");
    btn_start.sizeX=200;
    btn_start.sizeY=75;
    ImageResize(&btn_start.image,btn_start.sizeX,btn_start.sizeY);
    btn_start.texture = LoadTextureFromImage(btn_start.image);
    btn_start.posX=GetScreenWidth()*0.75;
    btn_start.posY=GetScreenHeight()*0.75;
    btn_start.rect = (Rectangle){btn_start.posX,btn_start.posY,btn_start.sizeX,btn_start.sizeY};

    Image pause_image = LoadImage("img/pause.png");
    ImageResize(&pause_image,btn_start.sizeX,btn_start.sizeY);
    Texture pause_texture = LoadTextureFromImage(pause_image);

    Image resume_image = LoadImage("img/resume.png");
    ImageResize(&resume_image,btn_start.sizeX,btn_start.sizeY);
    Texture resume_texture = LoadTextureFromImage(resume_image);

    //reset button
    Button btn_reset;
    btn_reset.image = LoadImage("img/reset.png");
    btn_reset.sizeX=200;
    btn_reset.sizeY=75;
    ImageResize(&btn_reset.image,btn_reset.sizeX,btn_reset.sizeY);
    btn_reset.texture= LoadTextureFromImage(btn_reset.image);
    btn_reset.posX= GetScreenWidth()*0.65;
    btn_reset.posY= GetScreenHeight()*0.6;
    btn_reset.rect= (Rectangle){btn_reset.posX,btn_reset.posY,btn_reset.sizeX,btn_reset.sizeY};

    Image reset_off_image = LoadImage("img/reset_off.png");
    ImageResize(&reset_off_image,btn_reset.sizeX,btn_reset.sizeY);
    Texture reset_off_texture = LoadTextureFromImage(reset_off_image);

    //undo button
    Button btn_undo;
    btn_undo.image = LoadImage("img/undo.png");
    btn_undo.sizeX=200;
    btn_undo.sizeY=75;
    ImageResize(&btn_undo.image,btn_undo.sizeX,btn_undo.sizeY);
    btn_undo.texture = LoadTextureFromImage(btn_undo.image);
    btn_undo.posX = GetScreenWidth()*0.85;
    btn_undo.posY = GetScreenHeight()*0.6;
    btn_undo.rect = (Rectangle){btn_undo.posX,btn_undo.posY,btn_undo.sizeX,btn_undo.sizeY};

    Image undo_off_image = LoadImage("img/undo_off.png");
    ImageResize(&undo_off_image,btn_undo.sizeX,btn_undo.sizeY);
    Texture undo_off_texture = LoadTextureFromImage(undo_off_image);

    //exit button
    Button btn_exit;
    btn_exit.image = LoadImage("img/exit.png");
    btn_exit.sizeX=125;
    btn_exit.sizeY=100;
    ImageResize(&btn_exit.image,btn_exit.sizeX,btn_exit.sizeY);
    btn_exit.texture = LoadTextureFromImage(btn_exit.image);
    btn_exit.posX = GetScreenWidth()*0.93;
    btn_exit.posY = GetScreenHeight()*0.01;
    btn_exit.rect = (Rectangle){btn_exit.posX,btn_exit.posY,btn_exit.sizeX,btn_exit.sizeY};

    //start screen button
    Button btn_start_screen;
    btn_start_screen.image = LoadImage("img/start2.png");
    btn_start_screen.sizeX=400;
    btn_start_screen.sizeY=150;
    ImageResize(&btn_start_screen.image,btn_start_screen.sizeX,btn_start_screen.sizeY);
    btn_start_screen.texture = LoadTextureFromImage(btn_start_screen.image);
    btn_start_screen.posX=GetScreenWidth()*0.4;
    btn_start_screen.posY=GetScreenHeight()*0.45;
    btn_start_screen.rect = (Rectangle){btn_start_screen.posX,btn_start_screen.posY,btn_start_screen.sizeX,btn_start_screen.sizeY};

    //start screen background image
    Image bg_image = LoadImage("img/start_screen_bg.png");
    int monitor=GetCurrentMonitor();
    ImageResize(&bg_image,GetMonitorWidth(monitor),GetMonitorHeight(monitor));
    Texture bg = LoadTextureFromImage(bg_image);

    while(!WindowShouldClose()){

        UpdateMusicStream(music);

        if(show_start_screen){
            //draw start screen
            BeginDrawing();
                ClearBackground(WHITE);
                DrawTexture(bg,0,0,WHITE);
                DrawTexture(btn_start_screen.texture,btn_start_screen.posX,btn_start_screen.posY,WHITE);
            EndDrawing();

            if (( CheckCollisionPointRec(GetMousePosition(),btn_start_screen.rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) ))
            {
                show_start_screen=false;
            }
            continue;
        }

        frame_count++;

        if(start){
            //se time_until_next_gen e' uguale a zero allora passa alla prossima generazione
            if(!time_until_next_gen){
                next_generation();
                time_until_next_gen = TARGET_FPS / GENERATION_PER_SECOND;
            }
            time_until_next_gen--;

            //calcolo tempo passato
            if(frame_count == TARGET_FPS){
                time_count++;
            }
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

        if(frame_count == TARGET_FPS){
            if(error_message_time>0){
                error_message_time--;
            }
            frame_count = 0;
        }

        if(error_message_time<=0){
            strcpy(error_message,"\0");
        }

        //aggiunge il gruppo di punti nella history
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(),board)){

            int n_punti_da_salvare;
            if(size_gruppo_punti>MAX_PUNTI_UNDO){
                n_punti_da_salvare=MAX_PUNTI_UNDO;
            }else{
                n_punti_da_salvare=size_gruppo_punti;
            }

            for(i=0;i<n_punti_da_salvare;i++){
                history[size_history][i]=gruppo_punti[size_gruppo_punti-i-1];
            }

            history_size_gruppi[size_history]=size_gruppo_punti;

            //se la lunghezza della history e' maggiore al massimo, allora cancello
            //il primo elemento e faccio il shift dei elementi (FIFO)
            if(size_history>=MAX_HISTORY-1){

                for(i=0;i<MAX_HISTORY-1;i++){
                    history_size_gruppi[i]=history_size_gruppi[i+1];

                    for(j=0;j<history_size_gruppi[i];j++){
                        history[i][j]=history[i+1][j];
                    }
                }

            }else{
                size_history++;
            }
            size_gruppo_punti=0;
        }

        if (( CheckCollisionPointRec(GetMousePosition(),btn_exit.rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) ))
        {
            break;
        }


        btn_start.isPressed = ( CheckCollisionPointRec(GetMousePosition(),btn_start.rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) );

        if(IsKeyPressed(KEY_SPACE) || btn_start.isPressed ){
            start=!start;

            if(!start){
                btn_start.texture = resume_texture;
            }else{
                btn_start.texture = pause_texture;
            }

            //se la simulazione e' partita, allora non si puo fare undo dei punti disegnati
            size_history=0;

            frame_count=0;
        }

        //undo
        btn_undo.isPressed = ( CheckCollisionPointRec(GetMousePosition(),btn_undo.rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) );

        if(IsKeyPressed(KEY_Z) || btn_undo.isPressed){

            if(size_history<=0){
                strcpy(error_message,"Limite undo raggiunto!\n");
                error_message_time=5;
                //printf("\nLimite undo raggiunto\n");
            }

            if(history_size_gruppi[size_history-1]>MAX_PUNTI_UNDO){
                //stampa che e' stato raggiunto il numero massimo di punti cancellabili
                strcpy(error_message,"Massimo numero di punti\ncancellabili raggiunto!\n");
                error_message_time=5;
            }

            //modifica l'array con l'ultimo stato dei punti
            for(i=0;i<history_size_gruppi[size_history-1];i++){
                arr[history[size_history-1][i].x][history[size_history-1][i].y]=history[size_history-1][i].ultimoStato;
            }

            if(size_history>0){
                size_history--;
            }

        }

        //reset board
        btn_reset.isPressed = ( CheckCollisionPointRec(GetMousePosition(),btn_reset.rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) );

        if( (IsKeyPressed(KEY_R) || btn_reset.isPressed) && !start){
            riempi_arr();

            //reset history
            size_history=0;

            //reset counters
            time_count=0;
            genaration_count=0;
            population=0;
            frame_count=0;
        }

        BeginDrawing();
            ClearBackground(coloreBianco);

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
                //hide original cursor
                HideCursor();
                DrawTexture(cursor_pen,GetMouseX()-UNIT/(SCALE_X*1.1),GetMouseY()-UNIT/(SCALE_Y*1.1),WHITE);

            }else
            {
                //show original cursor
                ShowCursor();
            }

            DrawTexture(btn_start.texture,btn_start.posX,btn_start.posY,WHITE);

            //draw reset and undo buttons
            if(start){
                DrawTexture(reset_off_texture,btn_reset.posX,btn_reset.posY,WHITE);
                DrawTexture(undo_off_texture,btn_undo.posX,btn_undo.posY,WHITE);
            }else{
                DrawTexture(btn_reset.texture,btn_reset.posX,btn_reset.posY,WHITE);
                DrawTexture(btn_undo.texture,btn_undo.posX,btn_undo.posY,WHITE);
            }

            DrawTexture(btn_exit.texture,btn_exit.posX,btn_exit.posY,WHITE);

            //draw counters
            DrawTextEx(font_subway,TextFormat("TIME : %.3d", time_count),(Vector2){(GetScreenWidth()*0.65),(GetScreenHeight()*0.05)},40,2,BLACK);
            DrawTextEx(font_subway,TextFormat("GENARATION : %.3d", genaration_count),(Vector2){(GetScreenWidth()*0.65),(GetScreenHeight()*0.12)},40,2,BLACK);
            DrawTextEx(font_subway,TextFormat("POPULATION : %.3d", population),(Vector2){(GetScreenWidth()*0.65),(GetScreenHeight()*0.19)},40,2,BLACK);

            DrawTextEx(font_arcade,TextFormat("%s",error_message),(Vector2){(GetScreenWidth()*0.65),(GetScreenHeight()*0.35)},50,2,BLACK);

        EndDrawing();
    }

    free(gruppo_punti);

    UnloadTexture(cursor_pen);
    UnloadTexture(btn_start.texture);
    UnloadTexture(pause_texture);
    UnloadTexture(resume_texture);
    UnloadTexture(btn_reset.texture);
    UnloadTexture(reset_off_texture);
    UnloadTexture(btn_undo.texture);
    UnloadTexture(undo_off_texture);
    UnloadTexture(btn_start_screen.texture);
    UnloadTexture(btn_exit.texture);

    UnloadMusicStream(music);
    CloseAudioDevice();

    CloseWindow();

    return 0;
}
