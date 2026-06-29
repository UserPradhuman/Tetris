//Sk Asif Hossain CSE/25095/1495👑
//Pradhuman Singh Khichi CSE/25065/1465
//Kushagra Srivastava CSE/25045/1445
//Priyanshu Singh Shekhawat CSE/25069/1469
//Project - CSC211 (2025-2029 Batch)
//TETRIS Game in C


#include "raylib.h"
#include <stdlib.h>
#include <time.h>

//Transforming the page into a grid for easier navigation
#define ROWS 20
#define COLS 10
#define CELL 40

//Board
int board[ROWS][COLS]={0};

//PreDeclaring Sounds
Sound click;
Sound GO;
Sound clear; 
Sound pp;
Music bgm;

//Global variables
int score=0,lines=0,gravity=25,gameOver=0,paused=0,highScore=0;

//Shapes
char shapes[7][4][4] = {
    {"    ","####","    ","    "},//I
    {"    "," ## ","##  ","    "},//S
    {"    ","##  "," ## ","    "},//Z
    {"    "," #  ","### ","    "},//T
    {"    "," #  "," ###","    "},//L
    {"    ","  # ","### ","    "},//J
    {"    "," ## "," ## ","    "}//B
    };

//Current piece
char B[4][4];//Current Shape
char T[4][4];//Temporary Shape
int anchor[2]; //(Vertical/Horizontal)
int currentColor;
int nxtColor;

//Bag system
int bag[2][7] = {
    {0,1,2,3,4,5,6},//shapes
    {1,2,3,4,5,6,7}//colors(fixed 7 colors)
    };
int bagIndex=0;

//Colors
Color colors[8]={BLACK,SKYBLUE,BLUE,ORANGE,YELLOW,GREEN,RED,PURPLE};

//Bag Shuffling function
void shuffleBag(){
    for(int i=6;i>0;i--){
        int j=rand()%(i+1);
        int temp=bag[0][i];//shuffling shapes
        bag[0][i]=bag[0][j];
        bag[0][j]=temp;
        }
        
    srand(time(NULL));
    
    for(int i=6;i>0;i--){
        int j=rand()%(i+1);
        int temp=bag[1][i];//shuffling colours
        bag[1][i]=bag[1][j];
        bag[1][j]=temp;
        }
    }

//Accessing a piece from Bag to display next and generate current
void getPieceFromBag(int index,char out[4][4]){
    int shp=bag[0][index];
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            out[i][j]=shapes[shp][i][j];
    }

//Random shape generating function
void generatePiece(){
    getPieceFromBag(bagIndex,B);
    currentColor=bag[1][bagIndex];
    
    bagIndex++;//Index updation
    
    anchor[0]=0;//vertical
    anchor[1]=COLS/2 - 2;//Horizontal
}

//Function to check whether shape can move
int canMove(int newRow,int newCol,char shape[4][4]){
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(shape[i][j]=='#'){
                int r=newRow+i;
                int c=newCol+j;
                if(r<0 || r>=ROWS || c<0 || c>=COLS)
                    return 0;//Boundary check
                if(board[r][c]!=0)
                    return 0;//Already occupied check
                }
            }
        }
    return 1;
    }

//Function to rotate shape
void rotateShape(){
    char temp[4][4];
    
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            temp[i][j] = B[3-j][i];//90* rotation into temp
        
    int compare=0;//This is to see if we have B shaped shape or not
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            if(temp[i][j]!=B[i][j]) compare=1;
    
    if(canMove(anchor[0], anchor[1], temp) && compare==1){//First check then actually rotate
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                B[i][j] = temp[i][j];
        if(score>=5) score-=5;
        else score=0;
        }
    }

//Function to lock shape in place
void lockPiece(){
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            if(B[i][j]=='#')
                board[anchor[0]+i][anchor[1]+j]=currentColor;
    }

//Function to clear lines
void lineCheck(){
    lines=0;
    for(int i=ROWS-1;i>=0;i--){//Checking for full line
        int check=1;
        for(int j=0;j<COLS;j++)
            if(board[i][j]==0){
                check=0; 
                break;
                }
                
        if(check){//Actually clearing line
            for(int k=i;k>0;k--)
                for(int j=0;j<COLS;j++)
                    board[k][j]=board[k-1][j];
            for(int j=0;j<COLS;j++)
                board[0][j]=0; 
            
            lines++;//Counting no. of lines cleared in one go
            i++;//Updating index, in case line that moved down is also complete
            }   
        }
        
    //Scoring
    if(lines!=0)PlaySound(clear);
    if(lines==1)score+=100;
    else if(lines==2)score+=300;
    else if(lines==3)score+=500;
    else if(lines==4)score+=800;
    }
 
//Function to display next shape 
void nextShape(){
    if(bagIndex==7){
        shuffleBag();
        shuffleBag();
        bagIndex=0;
        }
    getPieceFromBag(bagIndex,T);
    nxtColor=bag[1][bagIndex];
    }   

//Main
int main(){
    
    //Main Window
    InitWindow(COLS*CELL*2,ROWS*CELL," TETRIS - ASIF(1495) , PRADHUMAN(1465) , kUSHAGRA(1445) , PRIYANSHU(1469)");
    SetTargetFPS(60);
    
    //Music
    //SFX
    InitAudioDevice();
    
    click=LoadSound("Click.mp3"); 
    GO=LoadSound("GameOver.mp3"); 
    clear=LoadSound("LineClearance.mp3"); 
    pp=LoadSound("Pause_Play.mp3"); 
    
    //BGM
    bgm=LoadMusicStream("TetrisBGM.mp3");
    bgm.looping=true;
    PlayMusicStream(bgm);
    SetMusicVolume(bgm,0.25f);
    
    int frameCounter=0;
    
    //Initiate the function manually for 1st time
    shuffleBag();
    shuffleBag();
    generatePiece();
    nextShape();
    
    //Central loop
    while (!WindowShouldClose()){
        
        //Updating gravity to increase game difficulty
        gravity=25-(score/500);
        if(gravity<5)
            gravity=5;
        
        UpdateMusicStream(bgm);
        
        if(IsKeyPressed(KEY_R)){//Reset game
            PlaySound(click);
            PlayMusicStream(bgm);
            
            for(int i=0;i<ROWS;i++)
                for(int j=0;j<COLS;j++)
                    board[i][j]=0;

            //Reset variables
            score=0;
            lines=0;
            gameOver=0;
            paused=0;
            bagIndex=0;
            frameCounter=0;

            //Reset bag + pieces
            shuffleBag();
            generatePiece();
            nextShape();
            }
        
        if(IsKeyPressed(KEY_SPACE) && !gameOver){//Play/Pause
            paused=!paused;
            PlaySound(pp);
            }
        
        if(!paused && !gameOver){
            //Mapping keys to different actions
            if(IsKeyPressed(KEY_A) && canMove(anchor[0],anchor[1]-1,B)){//Left
                anchor[1]--;
                PlaySound(click);
                }
            if(IsKeyPressed(KEY_D) && canMove(anchor[0],anchor[1]+1,B)){//Right
                anchor[1]++;
                PlaySound(click);
                }
            if(IsKeyPressed(KEY_S) && canMove(anchor[0]+1,anchor[1],B)){//Down
                anchor[0]++; 
                score+=2; 
                PlaySound(click);
                }
            if(IsKeyPressed(KEY_W)){//Rotate
                rotateShape();
                PlaySound(click);
                }
        
            //Gravity
            frameCounter++;
            if(frameCounter>gravity){
                frameCounter=0;
                if(canMove(anchor[0]+1,anchor[1],B))
                    anchor[0]++;
                else {
                    lockPiece(); 
                    lineCheck(); 
                    
                    // GAME OVER check before generating piece
                    getPieceFromBag(bagIndex,T);
                    
                    if(!canMove(0,COLS/2 -2,T)){
                        gameOver=1;
                        StopMusicStream(bgm);
                        PlaySound(GO);
                        continue;
                        }

                    generatePiece();
                    nextShape();
                    }
                }
            }

        BeginDrawing();
        ClearBackground(BLUE);

        //Game Board
        for(int i=0;i<ROWS;i++){
            for(int j=0;j<COLS;j++){
                DrawRectangle(j*CELL,i*CELL,CELL,CELL,colors[board[i][j]]);
                DrawRectangleLines(j*CELL,i*CELL,CELL,CELL,(Color){40,40,40,255});
                }
            }
         
        //Current Piece
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                if(B[i][j]=='#'){
                    DrawRectangle((anchor[1]+j)*CELL,(anchor[0]+i)*CELL,CELL,CELL,colors[currentColor]);
                    DrawRectangleLines((anchor[1]+j)*CELL,(anchor[0]+i)*CELL,CELL,CELL,(Color){40,40,40,255});
                    }

        int x=COLS*CELL + 20;

        //Play/Pause & Restart Boxes
        DrawRectangle(x,40,360,110,BLACK);
        DrawRectangleLines(x,40,360,110,WHITE);
        if(!gameOver){
            DrawText("SPACE  :  PLAY/PAUSE",x+40,60,24,YELLOW);
            DrawText("R  :  RESTART",x+100,105,24,YELLOW);
            }
         else 
             DrawText("press R to RESTART",x+45,80,24,YELLOW);

        //Display Score
        DrawText("SCORE",x+120,220,30,YELLOW);
        DrawRectangle(x,250,360,100,BLACK);
        DrawRectangleLines(x,250,360,100,WHITE);
        DrawText(TextFormat("%06d",score),x+55,265,70,RED);

        int gridX=COLS*CELL + 4*CELL/2;
        int gridY=ROWS*CELL - 6*CELL -60;
        
        //Drawing Next shape + Bottom 5x5 grid
        
          
        //Draw Next Shape
        if(!gameOver){
            
            DrawText(" NEXT SHAPE",gridX+45,gridY-30,20,YELLOW);
            for(int i=0;i<6;i++){
                for(int j=0;j<6;j++){
                    DrawRectangle(gridX + j*CELL,gridY + i*CELL,CELL,CELL,BLACK);
                    DrawRectangleLines(gridX + j*CELL, gridY + i*CELL,CELL,CELL,BLUE);
                    }
                }
            
            for(int i=0;i<4;i++)
                for(int j=0;j<4;j++)
                    if(T[i][j]=='#'){
                        DrawRectangle(gridX + (j+1)*CELL,gridY + (i+1)*CELL,CELL,CELL,colors[nxtColor]);
                        DrawRectangleLines(gridX + (j+1)*CELL, gridY + (i+1)*CELL,CELL,CELL,BLUE);
                        }
        }
        //GAME OVER CONDITION
        else{
            if(score>highScore){
                highScore=score;
                DrawText("NEW HIGH SCORE!!",gridX-65,gridY+20,60,RED);
            }
            DrawText(" NICE TRY!!",gridX-65,gridY+60,60,YELLOW);
        }
        
        
        //GameGrid Border
        for(int i=0;i<4;i++)
            DrawRectangleLines(i,i,COLS*CELL-2*i,ROWS*CELL-2*i,WHITE);

        //SideBarGrid Border
        for(int i=0;i<4;i++)
            DrawRectangleLines(COLS*CELL + i - 4,i,COLS*CELL - 2*i + 4,ROWS*CELL - 2*i,WHITE);

        // Overlay box
        if(gameOver || paused){
            int boxW=8*CELL;
            int boxH=3*CELL;

            int boxX=(COLS*CELL - boxW)/2;
            int boxY=(ROWS*CELL - boxH)/2;

            DrawRectangle(boxX,boxY,boxW,boxH,WHITE);
            DrawRectangleLines(boxX,boxY,boxW,boxH,BLACK);

            if(gameOver)
                DrawText("GAME OVER!!",boxX+60,boxY+40,30,RED);
            else if(paused)
                DrawText("GAME PAUSED",boxX+48,boxY+40,30,GREEN);
            }

        EndDrawing();
        }
    
    //Unloading Audio from Ram
    UnloadSound(click);
    UnloadSound(clear);
    UnloadSound(GO);
    UnloadSound(pp);
    UnloadMusicStream(bgm);
    CloseAudioDevice();
    
    CloseWindow();
    return 0;
    }