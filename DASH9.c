#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

//================= CONSTANTES
#define WIDTH 800
#define HEIGHT 800
#define GRAVITY 0.6f
#define SIZENAME 10
#define MAXPLAYER 5
#define TILE_SIZE 60
#define MAX_ROWS 10
#define MAX_COLS 300
#define EASY 10
#define NORMAL 20
#define HARD 30

//======================================================================================
//====== MENU
typedef enum Menu { INICIO = 0, GAMEPLAY = 1, DIFFICULTY = 2, GAMEOVER = 3, RANKING = 4 } Menu;

//======================================================================================
//========= STRUCTS
typedef struct Jogador
{
    char name[50];
    int points;

} Player;

typedef struct Difficulty
{
    int score_threshold;
    int gap;
    int inc_gap;
    int difmaxheight;
    int inc_difmaxheight;
    int velocityobstacles;
    int inc_velocityobstacles;
} Difficulty;

typedef struct Square
{
    Vector2 position;
    int size;
    float velocity;
    bool isJumping;
    bool onGround;
       int timepower;

} Square;
//======================================================================================
//======== VARIAVEIS GLOBAIS
Square square;
Player squares[5];
Player new_square;
Sound som_gameover;
Sound som_jump;
Sound som_power;

Texture2D background;
Texture2D backgroundHard;
Texture2D backgroundNormal;
Texture2D backgroundEasy;
Texture2D icon;
Texture2D logo;

int chooselevel = NORMAL;
char levelMatrix[MAX_ROWS][MAX_COLS];
int posmapa=0;

Menu menu = INICIO;

//======================================================================================
//========== DECLARACAO DE FUNCOES
void LoadRanking(Player squares[5]);
void UpdateRanking(void);
void ShowRanking(Player players[5]);
void DrawRanking(void);
void Menudifficulty(void);
void Arqdifficulty(const char* Arquivodifficulty);
void InitVariables(void);
void Gameplay(void);
void Inicio(void);
void Gameover(void);
void DrawAll(void);
void MakesNewRank(void);
void SomJump(void);
void SomEnd(void);
void SomPower(void);
void LoadAssets(void);
void UnloadAssets(void);
void DrawSquare(void);
void HandleCollisions(void);

//======================================================================================
//====== MAIN
int main(void)
{
    InitWindow(WIDTH, HEIGHT, "INF DASH");
    SetTargetFPS(60);
    InitAudioDevice();
    LoadAssets();

    // LOOP DE TELAS
    while(!WindowShouldClose())
    {
        switch(menu)
        {
        case INICIO:
            Inicio();
            break;

        case GAMEPLAY:
            if(chooselevel == EASY)
            {
                Arqdifficulty("dificuldade0.txt");
            }

            if(chooselevel == NORMAL)
            {
                Arqdifficulty("dificuldade1.txt");
            }

            if(chooselevel == HARD)
            {
                Arqdifficulty("dificuldade2.txt");
            }

            Gameplay(); // chamando a funcao gameplay

            break;

        case DIFFICULTY:
            Menudifficulty();
            break;

        case GAMEOVER:
            Gameover();
            break;

        case RANKING:
            DrawTexture(background, 0, 0, WHITE);

            DrawText("RANKING", 20, 20, 50, WHITE);

            DrawRectangle(200, 300, 350, 50, WHITE);
            DrawText(TextFormat("%s: %i mortes",squares[0].name, squares[0].points), 250, 310, 35, GREEN);

            DrawRectangle(200, 360, 350, 50, WHITE);
            DrawText(TextFormat("%s: %i mortes",squares[1].name, squares[1].points), 250, 370, 35, GREEN);

            DrawRectangle(200, 420, 350, 50, WHITE);
            DrawText(TextFormat("%s: %i mortes",squares[2].name, squares[2].points), 250, 430, 35, GREEN);

            DrawRectangle(200, 480, 350, 50, WHITE);
            DrawText(TextFormat("%s: %i mortes",squares[3].name, squares[3].points), 250, 490, 35, GREEN);

            DrawRectangle(200, 540, 350, 50, WHITE);
            DrawText(TextFormat("%s: %i mortes",squares[4].name, squares[4].points), 250, 550, 35, GREEN);

            DrawRanking();
            break;

        default:
            break;
        }
    }

    UnloadAssets();
    CloseWindow();
    return 0;
}

//======================================================================================
//====== GAMEPLAY
void Gameplay()
{
    InitVariables();

    //Arqdifficulty("dificuldade0.txt");

    while(!WindowShouldClose())
    {
        posmapa -= 4;

        if(IsKeyDown(KEY_SPACE) && square.onGround && !square.isJumping)
        {
            square.velocity = -12;
            square.isJumping = true;
            square.onGround = false;
            PlaySound(som_jump);
        }else
        if(square.onGround && square.velocity > 0)
        {
            square.velocity = 0;
        }else if(!square.onGround)
            {
         square.velocity += GRAVITY;


        } else if(square.onGround)
        {
            square.isJumping = false;
        }

        square.position.y += square.velocity;
        square.onGround = false;

        if(square.position.y >= HEIGHT - square.size)
        {
            square.position.y = HEIGHT - square.size;
            square.isJumping = false;
        }

       // DrawRectangle(450, 480, 350, 50, WHITE);

        HandleCollisions();

        if(square.timepower && (time(NULL)-square.timepower) >= 10)
            square.timepower = 0;

        DrawAll();


        if(menu==GAMEOVER)
            break;
    }
    menu = GAMEOVER;
}

void LoadAssets(void)
{
    // Load textures
    background = LoadTexture("background.png");
    backgroundEasy = LoadTexture("fundo3.png");
    backgroundNormal = LoadTexture("fundo4.png");
    backgroundHard = LoadTexture("fundo6.png");
    icon = LoadTexture("player.png");
    logo = LoadTexture("logo.png");

    // Load sounds
    som_gameover = LoadSound("gameover.mp3");
    som_jump = LoadSound("jump.mp3");
    som_power = LoadSound("power.mp3");

    // Load ranking
    LoadRanking(squares);
}

void SomJump(void)
{
    if(IsSoundPlaying(som_jump))
    {
        StopSound(som_jump);
    }
    PlaySound(som_jump);
}

void SomEnd(void)
{
    if(IsSoundPlaying(som_gameover))
    {
        StopSound(som_gameover);
    }
    PlaySound(som_gameover);
}

void SomPower(void)
{
    if(IsSoundPlaying(som_power))
    {
        StopSound(som_power);
    }
    PlaySound(som_power);
}



void Inicio(void)
{
    BeginDrawing();
    //ClearBackground(RAYWHITE)

    DrawTexture(background,0,0,WHITE);
    DrawTextureEx(logo,(Vector2){120,250},0,0.7,WHITE);

    DrawRectangle(WIDTH / 2 - 210, (HEIGHT / 2)-10, 420,55,WHITE);
    DrawText("JOGO (enter)", WIDTH / 2 - 200, HEIGHT / 2, 40, GREEN);

    DrawRectangle(WIDTH / 2 - 210, (HEIGHT / 2)+65, 420,55,WHITE);
    DrawText("DIFICULDADE (d)", WIDTH / 2 - 200, HEIGHT / 2 + 75, 40, GREEN);

    DrawRectangle(WIDTH / 2 - 210, (HEIGHT / 2)+140, 420,55,WHITE);
    DrawText("LEADERBOARD (r)", WIDTH / 2 - 200, HEIGHT / 2 + 150, 40, GREEN);

    EndDrawing();

    if(IsKeyPressed(KEY_D))
    {
        menu = DIFFICULTY;
    }
    if(IsKeyPressed(KEY_ENTER))
    {
        menu = GAMEPLAY;
    }
    if(IsKeyPressed(KEY_R))
    {
        menu = RANKING;
    }
}

void Gameover(void)
{
    BeginDrawing();
   // ClearBackground(RAYWHITE);
   DrawTexture(background,0,0,WHITE);

    if(new_square.points < squares[4].points && posmapa <= -18000)
    {
        printf("dromedario fabuloso");

        //DrawTexture(background, 0, 0, WHITE);

        // DrawRectangle(450, 480, 350, 50, WHITE);


        UpdateRanking();
        new_square.points=0;

    }

        // DrawTexture(background, 0, 0, WHITE);

        DrawRectangle((WIDTH / 2)-210, (HEIGHT / 2)-5, 415,35,WHITE);
        DrawText("Pressione M para voltar ao menu", WIDTH / 2 - 200, HEIGHT / 2, 22, GREEN);

        DrawRectangle((WIDTH / 2)-210, (HEIGHT / 2)+45, 460,35,WHITE);
        DrawText("Pressione ENTER para jogar novamente", WIDTH / 2 - 200, HEIGHT / 2 + 50, 22, GREEN);

        DrawRectangle((WIDTH / 2)-210, (HEIGHT / 2)+90, 415,35,WHITE);
        DrawText("Pressione R para ver o ranking", WIDTH / 2 - 200, HEIGHT / 2 + 95, 22, GREEN);


    if(IsKeyPressed(KEY_M))
    {
        menu = INICIO;
    }
    if(IsKeyPressed(KEY_ENTER))
    {
        menu = GAMEPLAY;
    }
    if(IsKeyPressed(KEY_R))
    {
        menu = RANKING;
    }


    EndDrawing();

}

void Menudifficulty(void)
{
    BeginDrawing();
    //ClearBackground(RAYWHITE);

    DrawTexture(background,0,0,WHITE);

    DrawRectangle((WIDTH / 2)-220, (HEIGHT / 2)+2, 500,35,WHITE);
    DrawText("Selecione a dificuldade:", WIDTH / 2 - 210, HEIGHT / 2, 40, GREEN);

    DrawRectangle((WIDTH / 2)-210, (HEIGHT / 2)+40, 100,30,WHITE);
    DrawText("1 - Facil", WIDTH / 2 - 200, HEIGHT / 2 + 50, 20, GREEN);

    DrawRectangle((WIDTH / 2)-210, (HEIGHT / 2)+70, 130,30,WHITE);
    DrawText("2 - Normal", WIDTH / 2 - 200, HEIGHT / 2 + 80, 20, GREEN);

    DrawRectangle((WIDTH / 2)-210, (HEIGHT / 2)+100, 130,35,WHITE);
    DrawText("3 - Dificil", WIDTH / 2 - 200, HEIGHT / 2 + 110, 20, GREEN);

    EndDrawing();

    if(IsKeyPressed(KEY_ONE))
    {
        chooselevel = EASY;
        menu = INICIO;
    }
    if(IsKeyPressed(KEY_TWO))
    {
        chooselevel = NORMAL;
        menu = INICIO;
    }
    if(IsKeyPressed(KEY_THREE))
    {
        chooselevel = HARD;
        menu = INICIO;
    }
}

void Arqdifficulty(const char* Arquivodifficulty)
{
    FILE* file = fopen(Arquivodifficulty, "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo de nível!\n");
    }
    else
    {

        for (int i = 0; i < MAX_ROWS; i++)
        {
           for(int j=0; j < MAX_COLS; j++)
           {
               levelMatrix[i][j]=fgetc(file);
               //printf(" %c", levelMatrix[i][j]);

               if(levelMatrix[i][j]=='P'){
               square.position = (Vector2) {j*TILE_SIZE, i*TILE_SIZE};
               }

           }

        }
    }

    fclose(file);

}

void UpdateRanking(void)
{
    int letterCount = 0;
    int j, k;

        int key = GetCharPressed();

        while(!WindowShouldClose() && !IsKeyPressed(KEY_ENTER)) // desenhar tela que pede nome do jogador
        {
            BeginDrawing();
            //ClearBackground(WHITE);
            DrawTexture(background,0,0,WHITE);
            DrawText("voce entrou no ranking", 200, 250, 40, GREEN);
            DrawText("nome em minusculas: ", 200, 350, 40, GREEN);

            DrawRectangle(200, 450, 350, 50, WHITE);
            DrawText(new_square.name, 210, 460, 40, ORANGE);

            key = GetCharPressed();

            if ((key >= 97) && (key <= 122) && (letterCount < SIZENAME)) // letras aparecem na tela e string eh lida
            {
                new_square.name[letterCount] = (char)key;
                new_square.name[letterCount+1] = '\0';
                letterCount++;
            }

            if (IsKeyPressed(KEY_BACKSPACE)) // caso usuario apague
            {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                new_square.name[letterCount] = '\0';
            }

            EndDrawing();

        }

        if(IsKeyPressed(KEY_ENTER))
                menu = INICIO;


        for(j = 0; j < 5; j++) // enquanto nao passar por todos os jogadores
        {
            if(new_square.points < squares[j].points) // se a pontuacao do jogador for maior do cont do ranking
            {

                for(k = MAXPLAYER - 1; k > j-1; k--) // colocar o restante dos jogadores uma posicao abaixo no ranking
                {
                    strcpy(squares[k].name, squares[k-1].name);
                    squares[k].points = squares[k-1].points;

                }

                strcpy(squares[j].name, new_square.name); // colocar o nome do jogador no nome do ranking
                squares[j].points = new_square.points; // colocar a pontuacao do jogador na pontuacao do ranking

                j = MAXPLAYER;
            }
        }

        FILE *ranking = fopen("ranking.bin", "wb");

        if(ranking != NULL)
        {
            fwrite(squares, sizeof(Player), 5, ranking); // salva no arquivo
            fclose(ranking);
        }



    }


void LoadRanking(Player squares[5])
{

    FILE *ranking;
    ranking = fopen("ranking.bin", "rb");

    if(ranking == NULL) // se arquivo nao existe
    {
        MakesNewRank();
    }

    else
    {
        fread(squares, sizeof(Player), 5, ranking);
        fclose(ranking);
    }
}



void MakesNewRank(void)
{
    Player squaresf[MAXPLAYER];
    FILE *arq;

    for(int i = 0; i < MAXPLAYER; i++) // escreve 5 jogadores com 0 npontos
    {
        strcpy(squaresf[i].name, TextFormat("%d", i));
        squaresf[i].points = 999;
    }

    arq = fopen("ranking.bin", "wb");

    if (arq != NULL) // salva no arquivo
    {
        fwrite(squaresf, sizeof(Player), MAXPLAYER, arq);
        fclose(arq);
    }
}

void ShowRanking(Player squares[5])
{
    DrawTexture(background,0,0,WHITE);
    char buffer[50];
    for(int i = 0; i < 5; i++)
    {
        sprintf(buffer, "%s - %d", squares[i].name, squares[i].points);
        DrawText(buffer, WIDTH / 2 - MeasureText(buffer, 20) / 2, HEIGHT / 2 - 100 + (i * 30), 20, BLACK);
    }
}

void DrawRanking(void)
{
    BeginDrawing();
    //ClearBackground(RAYWHITE);
   // DrawAll();
    LoadRanking(squares);

    if(IsKeyPressed(KEY_M))
    {
        menu = INICIO;
    }
    EndDrawing();
}

//======================================================================================
//====== DRAW FUNCTIONS
void DrawAll(void)
{
    BeginDrawing();
    //ClearBackground(RAYWHITE);

    if(chooselevel == EASY)
    {
        DrawTexture(backgroundEasy, 0, 0, WHITE);
    }
     if(chooselevel == NORMAL)
    {
        DrawTexture(backgroundNormal, 0, 0, WHITE);
    }
     if(chooselevel == HARD)
    {
        DrawTexture(backgroundHard, 0, 0, WHITE);
    }

    //DrawTexture(background, 0, 0, WHITE);
    DrawLevel();


    DrawText(TextFormat("tentativas: %d", new_square.points),20,80, 40, BLACK);

    if(square.timepower){

        DrawText(TextFormat("tempo restante de invencibilidade: %d", (10-(time(NULL)-square.timepower))), 20,120,30, BLACK);
    }


    EndDrawing();
}

void DrawLevel()
{

    DrawTexture(icon,square.position.x, (square.position.y-13), WHITE);

   // DrawRectangle(square.position.x, square.position.y, square.size, square.size, BLUE);

    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = 0; j < MAX_COLS; j++)
        {
                if (levelMatrix[i][j] == 'G')
                {
                    DrawRectangle(j * TILE_SIZE+posmapa, i * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
                }
                else if (levelMatrix[i][j] == 'S')
                {
                    DrawTriangle((Vector2)
                    {
                        ((((j + 0.5)*TILE_SIZE)+posmapa), (i-1) * TILE_SIZE)
                    },
                    (Vector2)
                    {
                        ((j * TILE_SIZE+posmapa), (i * TILE_SIZE))
                    },
                    (Vector2)
                    {
                        (((j + 1) * TILE_SIZE)+posmapa, (i * TILE_SIZE))
                    }, BLACK);



                } else if(levelMatrix[i][j] == 'M')
                    {
                        DrawCircle(((j*TILE_SIZE) + (TILE_SIZE/2) + posmapa), (i*TILE_SIZE+(TILE_SIZE/2)), TILE_SIZE/2, GREEN);
                    }

            }
        }
    }

//======================================================================================
//====== COLLISION HANDLING
void HandleCollisions(void)
    {
        if(posmapa <= -18000)
         menu = GAMEOVER;

        Rectangle squareRect = {square.position.x, square.position.y, square.size, square.size};


        // Verificar colisões com os obstáculos da matriz
        for (int i = 0; i < MAX_ROWS; i++)
        {
            for (int j = 0; j < MAX_COLS; j++)
            {
                Rectangle blockRect = {j * TILE_SIZE+posmapa, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                if (CheckCollisionRecs(squareRect, blockRect))
                    {
                       // printf("rogerinho dj");
                        if (levelMatrix[i][j] == 'G')
                {
                     square.onGround = true;

                        if(blockRect.x <= squareRect.x + square.size && blockRect.y < squareRect.y + square.size - (square.size / 3) && !square.timepower)
                        {
                        new_square.points++;
                        PlaySound(som_gameover);
                        menu = GAMEOVER;
                        return;
                        }


                } else if(levelMatrix[i][j] == 'M')
                {
                    levelMatrix[i][j] = ' ';
                    square.timepower = time(NULL);
                    PlaySound(som_power);
                }


                    }


            }


        }


    }

    void InitVariables(void)
    {
        square.size = 50;
        square.velocity = 0;
        square.isJumping = false;
        square.onGround = false;
        posmapa= 0;
        square.timepower=0;
    }


    void UnloadAssets(void)
    {
        // Unload textures


        // Unload sounds
        UnloadSound(som_gameover);
        UnloadSound(som_jump);
        UnloadSound(som_power);
    }
