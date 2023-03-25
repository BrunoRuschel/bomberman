#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define RAIO 15
#define LADO 35
#define LADO_BLOCOS 40
#define LARGURA_TELA 520
#define ALTURA_TELA 440
#define NUM_INIMIGO 3
#define TAMSTR 20
#define Y_INICIAL 200
#define X_INICAL 240
#define NUM_BLOCOS 30
#define NUM_MUROS 20
#define VEL_INIMIGO 5
#define VEL_BOMBERMAN 2
#define NUMEXP 4
#define RAIOEXP 30


typedef enum
{
        esq,
        dir,
        cima,
        baixo,
}DIRECAO;

typedef struct {
    char nomejog[TAMSTR];
    short vidas;
    int pontuacao;
    Vector2 posicao;
    Vector2 posicao_bombas;
    DIRECAO dir;
    int bombas;
    Rectangle bomberman;
} JOGADOR;

typedef struct
{
    Vector2 posicao;
    DIRECAO dir;
    short status;
} INIMIGO;

typedef struct
{
    int r, g, b, a;
}COR;

typedef struct
{
    Rectangle muros[NUM_MUROS];
    short status;
}MURO;

void  inicializaJog (JOGADOR *jog){
    strcpy(jog->nomejog, "\0");
    jog->vidas = 3;
    jog->pontuacao = 0;
    jog->posicao.x = X_INICAL;
    jog->posicao.y = Y_INICIAL;
    //jog->dir = rand() % 4;
    jog->bombas = 3;
    jog->bomberman.x = jog->posicao.x - LADO;
    jog->bomberman.y = jog->posicao.y - LADO;
    jog->bomberman.width = LADO;
    jog->bomberman.height = LADO;
}

void inicializa_status_inimigo (INIMIGO ListaInimigo[])
{
    int i;
    for(i = 0; i < NUM_INIMIGO; i++)
    {
        ListaInimigo[i].status = 0;
    }
}

void bomberman_n_atravessa_janela (JOGADOR *jog)
{
    if (jog->bomberman.x < 0) jog->bomberman.x = 0;
    if (jog->bomberman.x + jog->bomberman.width > LARGURA_TELA) jog->bomberman.x = LARGURA_TELA - jog->bomberman.width;
    if (jog->bomberman.y < 0) jog->bomberman.y = 0;
    if (jog->bomberman.y + jog->bomberman.height > ALTURA_TELA) jog->bomberman.y = ALTURA_TELA - jog->bomberman.height;
}

void move_jogador(JOGADOR *jog)
{
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) jog->bomberman.x += VEL_BOMBERMAN;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) jog->bomberman.x -= VEL_BOMBERMAN;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) jog->bomberman.y -= VEL_BOMBERMAN;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) jog->bomberman.y += VEL_BOMBERMAN;

    bomberman_n_atravessa_janela(jog);
}

void inimigo_fora_bloco (INIMIGO listainimigo[], Rectangle blocos[], int i)
{
    int j;
    for (j = 0; j < NUM_BLOCOS; j++)
        {
            if (CheckCollisionCircleRec(listainimigo[i].posicao, RAIO, blocos[j]))
            {
                listainimigo[i].posicao.x = rand() % LARGURA_TELA;
                listainimigo[i].posicao.y = rand() % ALTURA_TELA;
                j = -1;
            }
        }
}

void inimigo_fora_muro (INIMIGO listainimigo[], Rectangle muros[], int i)
{
    int j;
    for (j = 0; j < NUM_MUROS; j++)
        {
            if (CheckCollisionCircleRec(listainimigo[i].posicao, RAIO, muros[j]))
            {
                listainimigo[i].posicao.x = rand() % LARGURA_TELA;
                listainimigo[i].posicao.y = rand() % ALTURA_TELA;
                j = -1;
            }
        }
}

void gera_inimigo(INIMIGO listainimigo[], Rectangle blocos[], Rectangle muros[])
{
    int i;
    for(i=0; i<NUM_INIMIGO; i++)
    {
        listainimigo[i].posicao.x=rand()%(LARGURA_TELA);
        listainimigo[i].posicao.y=rand()%(ALTURA_TELA);
        listainimigo[i].dir = rand() % 4;

        inimigo_fora_bloco(listainimigo, blocos, i);
        inimigo_fora_muro(listainimigo, muros, i);
    }
}


void move_inimigo (INIMIGO *ini, DIRECAO direcao, Rectangle blocos[], Rectangle muros[]){
    switch(direcao){
        case esq:
            ini->posicao.x = ini->posicao.x - VEL_INIMIGO;
            break;
        case dir:
            ini->posicao.x = ini->posicao.x + VEL_INIMIGO;
            break;
        case cima:
            ini->posicao.y = ini->posicao.y - VEL_INIMIGO;
            break;
        case baixo:
            ini->posicao.y = ini->posicao.y + VEL_INIMIGO;
            break;
    }
}

void inverte_movimento_inim (INIMIGO *ini, Rectangle blocos[], Rectangle muros[], DIRECAO direcao)
{
    int i, j;
    for(i = 0; i < NUM_BLOCOS; i++){
        for(j =0; j < NUM_MUROS; j++)
        {
            if(CheckCollisionCircleRec(ini->posicao, RAIO, blocos[i]) || CheckCollisionCircleRec(ini->posicao, RAIO, muros[j]))
            {
                switch(direcao)
                {
                    case esq:
                        ini->posicao.x = ini->posicao.x + VEL_INIMIGO;
                        break;
                    case dir:
                        ini->posicao.x = ini->posicao.x - VEL_INIMIGO;
                        break;
                    case cima:
                        ini->posicao.y = ini->posicao.y + VEL_INIMIGO;
                        break;
                    case baixo:
                        ini->posicao.y = ini->posicao.y - VEL_INIMIGO;
                        break;
                }
            }
        }
    }
}

void desenha_inimigo (INIMIGO ListaInimigo[])
{
    int i;
    for(i = 0; i < NUM_INIMIGO; i++)
    {
        if(!ListaInimigo[i].status)
        {
            DrawCircleV(ListaInimigo[i].posicao, RAIO, MAROON);
        }
        else
        {
            ListaInimigo[i].posicao.x=-30;
            ListaInimigo[i].posicao.y=0;
        }
    }
}

void inverte_mov_bomberman (JOGADOR *jog, Rectangle muros[], Rectangle blocos[])
{
    int i, j;
    for(i = 0; i < NUM_BLOCOS; i++){
        for(j = 0; j < NUM_MUROS; j++){
            if (CheckCollisionRecs(jog->bomberman, blocos[i]) || CheckCollisionRecs(jog->bomberman, muros[j])){
                if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) jog->bomberman.y += VEL_BOMBERMAN;
                if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) jog->bomberman.y -= VEL_BOMBERMAN;
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) jog->bomberman.x += VEL_BOMBERMAN;
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) jog->bomberman.x -= VEL_BOMBERMAN;
            }
        }
    }
}

void desenha_blocos(JOGADOR *jog, Rectangle blocos[])
{
    int i;
    for(i = 0; i < NUM_BLOCOS; i++){
        DrawRectangle(blocos[i].x, blocos[i].y, LADO_BLOCOS, LADO_BLOCOS, GRAY);
        DrawRectangleLines(blocos[i].x, blocos[i].y, LADO_BLOCOS, LADO_BLOCOS, BLACK);
    }
}

void gera_blocos(Rectangle blocos[])
{
    int i,j, c = 0;
    for(i = 0; i < 5; i++){
        for(j = 0; j < 6; j++){
            int x, y;
            //gera posição do bloco
            x = j * (LADO_BLOCOS + LADO_BLOCOS) + LADO_BLOCOS;
            y = i * (LADO_BLOCOS + LADO_BLOCOS) + LADO_BLOCOS;
            blocos[c] = (Rectangle){x, y, LADO_BLOCOS, LADO_BLOCOS};
            c++;
        }
    }
}

void gera_muros(Rectangle muros[], Rectangle blocos[])
{
    int i, j;
    int x, y;

    for (i = 0; i < NUM_MUROS; i++)
    {
        //gera nova posição para o muro
        x = (rand() % (LARGURA_TELA / LADO_BLOCOS)) * LADO_BLOCOS;
        y = (rand() % (ALTURA_TELA / LADO_BLOCOS)) * LADO_BLOCOS;

        //checa se sobrepõe a muros existentes
        for (j = 0; j < i; j++)
        {
            if (CheckCollisionRecs(muros[j], (Rectangle){x, y, LADO_BLOCOS, LADO_BLOCOS}))
            {
                x = (rand() % (LARGURA_TELA / LADO_BLOCOS)) * LADO_BLOCOS;
                y = (rand() % (ALTURA_TELA / LADO_BLOCOS)) * LADO_BLOCOS;

                j = -1;
            }
        }

        //checa se sobrepõe a blocos existentes
        for (j = 0; j < NUM_BLOCOS; j++)
        {
            if (CheckCollisionRecs((Rectangle){x, y, LADO_BLOCOS, LADO_BLOCOS}, blocos[j]))
            {
                //ajusta a posição do muro para evitar a colisão
                x = (rand() % (LARGURA_TELA / LADO_BLOCOS)) * LADO_BLOCOS;
                y = (rand() % (ALTURA_TELA / LADO_BLOCOS)) * LADO_BLOCOS;

                //verifica novamente se colide com outro bloco
                j = -1;
            }
        }
        muros[i] = (Rectangle){x, y, LADO_BLOCOS, LADO_BLOCOS};
    }
}

void desenha_muros(JOGADOR *jog, Rectangle muros[])
{
    int i;
    for(i = 0; i < NUM_MUROS; i++){
        DrawRectangle(muros[i].x, muros[i].y, LADO_BLOCOS, LADO_BLOCOS, (Color){172, 73, 35, 255});
        DrawRectangleLines(muros[i].x, muros[i].y, LADO_BLOCOS, LADO_BLOCOS, BLACK);
    }
}

void inimigo_n_atravessa(INIMIGO *inim)
{
    if (inim->posicao.x + RAIO > LARGURA_TELA) inim->posicao.x = LARGURA_TELA - RAIO;
    if (inim->posicao.x - RAIO < 0) inim->posicao.x = RAIO;
    if (inim->posicao.y + RAIO > ALTURA_TELA) inim->posicao.y = ALTURA_TELA - RAIO;
    if (inim->posicao.y - RAIO < 0) inim->posicao.y = RAIO;
}

void move_todos_inimigos (INIMIGO ListaInimigo[], Rectangle blocos[], Rectangle muros[])
{
    int i, r;
    for(i = 0; i < NUM_INIMIGO; i++)
        {
            r = rand() % 4;
            move_inimigo(&ListaInimigo[i], r, blocos, muros);
            inimigo_n_atravessa(&ListaInimigo[i]);
            inverte_movimento_inim(&ListaInimigo[i], blocos, muros, r);
        }
}

void seta_cor(COR *cor)
{
    cor->r=rand()%256;
    cor->g=rand()%256;
    cor->b=rand()%256;
    cor->a=rand()%256;
}

void colisao_bomberman_inimigo (JOGADOR *jog, INIMIGO listainimigo[])
{
    int i;
    for(i = 0; i < NUM_INIMIGO; i++)
    {
        if(CheckCollisionCircleRec(listainimigo[i].posicao, RAIO, jog->bomberman))
        {
            jog->bomberman.x = X_INICAL;
            jog->bomberman.y = Y_INICIAL; //bomberman volta para posicao inicial
            jog->vidas--; //perde uma vida
        }
    }
}

void colisao_explosao(JOGADOR *jog, INIMIGO listainimigo[])
{
    Rectangle explosao[4]={{ 0, 0, LADO, LADO}, { 0, 0, LADO, LADO}, { 0, 0, LADO, LADO}, { 0, 0, LADO, LADO}};
    int col_x=0, col_y=0, col_x2=0, col_y2=0, i;

    explosao[0].x= jog->posicao_bombas.x-RAIOEXP/2-LADO;
    explosao[0].y= jog->posicao_bombas.y;

    explosao[1].x= jog->posicao_bombas.x;
    explosao[1].y= jog->posicao_bombas.y-RAIOEXP/2-LADO;

    explosao[2].x= jog->posicao_bombas.x-RAIOEXP/2+LADO;
    explosao[2].y= jog->posicao_bombas.y;

    explosao[3].x= jog->posicao_bombas.x;
    explosao[3].y= jog->posicao_bombas.y-RAIOEXP/2+LADO;

    while(explosao[0].x > jog->posicao_bombas.x-RAIOEXP/2-3*LADO && !col_x)
    {


        for(i=0; i<NUM_INIMIGO; i++)
            if(CheckCollisionCircleRec(listainimigo[i].posicao, RAIO, explosao[0]))
            {
                listainimigo[i].status=1;
                //jog->cont_inimigos++;
            }

        explosao[0].x-=LADO;
    }

    while(explosao[1].y > jog->posicao_bombas.y-RAIOEXP/2-3*LADO && !col_y)
    {
        for(i=0; i<NUM_INIMIGO; i++)
            if(CheckCollisionCircleRec(listainimigo[i].posicao, RAIO, explosao[1]))
            {
                listainimigo[i].status=1;
                //jog->cont_inimigos++;
            }

        explosao[1].y-=LADO;
    }

    while(explosao[2].x < jog->posicao_bombas.x-RAIOEXP/2+3*LADO && !col_x2)
    {
        for(i=0; i<NUM_INIMIGO; i++)
            if(CheckCollisionCircleRec(listainimigo[i].posicao, RAIO, explosao[2]))
            {
                listainimigo[i].status=1;
                //jog->cont_inimigos++;
            }

        explosao[2].x+=LADO;
    }

    while(explosao[3].y < jog->posicao_bombas.y-RAIOEXP/2+3*LADO && !col_y2)
    {
        for(i=0; i<NUM_INIMIGO; i++)
            if(CheckCollisionCircleRec(listainimigo[i].posicao, RAIO, explosao[3]))
            {
                listainimigo[i].status=1;
                //jog->cont_inimigos++;
            }

        explosao[3].y+=LADO;
    }

    BeginDrawing();

    for(i = 0; i < NUMEXP; i++)
    {
        DrawRectangleRec(explosao[i], ORANGE);
    }

    EndDrawing();

}

void bota_bomba(JOGADOR *jog, INIMIGO listainimigo[], bool *colocada, bool *destruida)
{
    if(IsKeyPressed(KEY_SPACE) && (*colocada == false) && (jog->bombas > 0))
    {
        (jog->bombas)--;
        if(jog->bombas <= 0)
            jog->bombas = 0;
        jog->posicao_bombas.x = jog->bomberman.x;
        jog->posicao_bombas.y = jog->bomberman.y;
        *colocada = true;
        *destruida = false;
    }

    if(IsKeyPressed(KEY_ENTER) && *colocada)
    {
        *destruida=true;
        colisao_explosao(jog, listainimigo);
        *colocada=false;
    }
}


int main()
{
    InitWindow(LARGURA_TELA, ALTURA_TELA, "jogo");

    JOGADOR jog;
    INIMIGO inimigos[NUM_INIMIGO];
    Rectangle blocos[NUM_BLOCOS] = {}, muros[NUM_MUROS] = {};
    COR cor;
    bool pause = false;
    bool colocada = false, destruida = false;

    inicializaJog(&jog);
    gera_blocos(blocos);
    gera_muros(muros, blocos);
    gera_inimigo(inimigos, blocos, muros);
    inicializa_status_inimigo(inimigos);
    seta_cor(&cor);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {

        if(IsKeyPressed(KEY_P))
            pause= !pause;

        if(!pause)
        {
            move_todos_inimigos(inimigos, blocos, muros);
            move_jogador(&jog);

            DrawText(TextFormat("Vidas: %i", jog.vidas), 100, 10, 20, LIGHTGRAY);
            DrawText(TextFormat("Bombas: %i", jog.bombas), 250, 10, 20, LIGHTGRAY);
            ClearBackground((Color){cor.r, cor.g, cor.b, cor.a});
            desenha_inimigo(inimigos);
            colisao_bomberman_inimigo(&jog, inimigos);
            if(jog.vidas == 0 || IsKeyPressed(KEY_ESCAPE))
                return 0;
        }
        //desenha o bomberman
        DrawRectangleRec(jog.bomberman, DARKBLUE);
        desenha_blocos(&jog, blocos);
        desenha_muros(&jog, muros);
        inverte_mov_bomberman(&jog, muros, blocos);
        bota_bomba(&jog, inimigos, &colocada, &destruida);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
