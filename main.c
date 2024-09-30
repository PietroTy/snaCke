#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>

typedef struct Snake {
    float x;
    float y;
} Snake;

typedef struct Apple {
    int x;
    int y;
} Apple;

typedef struct Grape {
    int x;
    int y;
    bool isVisible;
    float spawnTime;
} Grape;

typedef struct Orange {
    int x;
    int y;
    bool isVisible;
    float spawnTime;
} Orange;

// Função para verificar se uma fruta está sobre o corpo da cobra
bool IsPositionOccupiedBySnake(int x, int y, Snake head, Snake *body, int bodyLength) {
    // Verifica se está na posição da cabeça
    if (x == head.x && y == head.y) {
        return true;
    }
    // Verifica se está em alguma parte do corpo
    for (int i = 0; i < bodyLength; i++) {
        if (x == body[i].x && y == body[i].y) {
            return true;
        }
    }
    return false;
}

// Função para verificar se uma posição está ocupada por alguma fruta
bool IsPositionOccupiedByFruit(int x, int y, Apple apple, Grape grape, Orange orange) {
    // Verifica a maçã
    if (x == apple.x && y == apple.y) return true;
    // Verifica a uva
    if (grape.isVisible && x == grape.x && y == grape.y) return true;
    // Verifica a laranja
    if (orange.isVisible && x == orange.x && y == orange.y) return true;
    
    return false;
}


// Função para gerar uma nova posição válida para a maçã ou uva
void GenerateValidPositionForFruit(Apple *apple, Grape *grape, Orange *orange, Snake head, Snake *body, int bodyLength, bool isApple, bool isGrape) {
    int fruitX, fruitY;
    do {
        fruitX = GetRandomValue(1, 11) * 50;
        fruitY = GetRandomValue(1, 11) * 50;
    } while (IsPositionOccupiedBySnake(fruitX, fruitY, head, body, bodyLength));

    if (isApple && apple != NULL) {
        apple->x = fruitX;
        apple->y = fruitY;
    } else if (isGrape && grape != NULL) {
        grape->x = fruitX;
        grape->y = fruitY;
    } else if (orange != NULL) {
        orange->x = fruitX;
        orange->y = fruitY;
    }
}


void ResetGame(Snake *head, Snake *body, int *bodyLength, Apple *apple, Grape *grape, Orange *orange, int *score, float *moveInterval, bool *gameOver, bool *moved, float *lastGrapeAppearanceTime, float *lastOrangeAppearanceTime, float currentTime, int *direction, bool *victory) {
    // Reiniciar cobra
    head->x = GetRandomValue(1, 11) * 50;  // Garante que a cobra começa longe das bordas
    head->y = GetRandomValue(1, 11) * 50;
    *bodyLength = 2;

    // Reiniciar corpo (coloca o corpo em uma posição válida atrás da cabeça)
    for (int i = 0; i < *bodyLength; i++) {
        body[i].x = head->x - ((i + 1) * 50);
        body[i].y = head->y;
    }

    // Reiniciar maçã e uva
    GenerateValidPositionForFruit(apple, NULL, NULL, *head, body, *bodyLength, true, false);
    grape->isVisible = false;
    orange->isVisible = false;
    *lastGrapeAppearanceTime = currentTime;
    *lastOrangeAppearanceTime = currentTime;

    // Resetar outras variáveis
    *score = 0;
    *gameOver = false;
    *victory = false;
    *moved = false;
    *moveInterval = 0.2f;
    *direction = 0;
    
}

int main(void) {
    Snake head;
    Apple apple;
    Grape grape;
    Orange orange;
    Snake body[144]; 
    int bodyLength = 2;
    int direction = 0;
    int score = 0;
    bool gameOver = false, victory = false, moved = false, open = false;
    float moveInterval = 0.2f, lastMoveTime = 0.0f, lastGrapeAppearanceTime = 0.0f, lastOrangeAppearanceTime = 0.0f;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(600, 600, "snaCke");
    SetWindowIcon(LoadImage("resources/iconeSnaCke.png"));
    SetTargetFPS(60);

    ResetGame(&head, body, &bodyLength, &apple, &grape, &orange, &score, &moveInterval, &gameOver, &moved, &lastGrapeAppearanceTime, &lastOrangeAppearanceTime, (float) GetTime(), &direction, &victory);

    while (!WindowShouldClose()) {
        float currentTime = GetTime(); // Obter o tempo atual
        float deltaTime = currentTime - lastMoveTime;

        // Aumentar velocidade da cobra a cada 20 pontos
        if (score % 20 == 0 && score != 0) {
            moveInterval = 0.2f - (0.02f * (score / 20));
        }

        if (gameOver) {
            BeginDrawing();
            ClearBackground(BLACK);
            const char *lostMessage = "You Lost!";
            const char *scoreMessage = TextFormat("Score: %d", score);
            const char *resetMessage = "press R to reset";
            DrawText(lostMessage, (GetScreenWidth() - MeasureText(lostMessage, 100)) / 2, (GetScreenHeight() - 200) / 2, 100, WHITE);
            DrawText(scoreMessage, (GetScreenWidth() - MeasureText(scoreMessage, 60)) / 2, (GetScreenHeight() + 100) / 2, 60, WHITE);
            DrawText(resetMessage, (GetScreenWidth() - MeasureText(resetMessage, 30)) / 2, (GetScreenHeight() + 300) / 2, 30, WHITE);

            if (IsKeyPressed(KEY_R)) {
                ResetGame(&head, body, &bodyLength, &apple, &grape, &orange, &score, &moveInterval, &gameOver, &moved, &lastGrapeAppearanceTime, &lastOrangeAppearanceTime, (float) GetTime(), &direction, &victory);
            }


            EndDrawing();
            continue;
        }

        if (victory) {
            BeginDrawing();
            ClearBackground(WHITE);
            const char *winMessage = "You Won!";
            const char *scoreMessage = TextFormat("Score: %d", score);
            const char *resetMessage = "press R to reset";
            DrawText(winMessage, (GetScreenWidth() - MeasureText(winMessage, 100)) / 2, (GetScreenHeight() - 200) / 2, 100, BLACK);
            DrawText(scoreMessage, (GetScreenWidth() - MeasureText(scoreMessage, 60)) / 2, (GetScreenHeight() + 100) / 2, 60, BLACK);
            DrawText(resetMessage, (GetScreenWidth() - MeasureText(resetMessage, 30)) / 2, (GetScreenHeight() + 300) / 2, 30, BLACK);

            if (IsKeyPressed(KEY_R)) {
                ResetGame(&head, body, &bodyLength, &apple, &grape, &orange, &score, &moveInterval, &gameOver, &moved, &lastGrapeAppearanceTime, &lastOrangeAppearanceTime, (float) GetTime(), &direction, &victory);
            }


            EndDrawing();
            continue;
        }
        

        // Controle de movimento
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_W) ||
            IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP)) {

            if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && direction != 2) direction = 1; // Direita
            else if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && direction != 1) direction = 2; // Esquerda
            else if ((IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) && direction != 4) direction = 3; // Baixo
            else if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && direction != 3) direction = 4; // Cima
            moved = true;
        }

        // Movimento da cobra
        if (deltaTime >= moveInterval) {
            lastMoveTime = currentTime;
            if(open) open = false;
            else open = true;
            for (int i = bodyLength - 1; i > 0; i--) body[i] = body[i - 1];
            if (bodyLength > 0) {
                body[0].x = head.x;
                body[0].y = head.y;
            }
            if (direction == 1) head.x += 50;
            else if (direction == 2) head.x -= 50;
            else if (direction == 3) head.y += 50;
            else if (direction == 4) head.y -= 50;
        }

        // Colisão com o corpo
        if (moved && bodyLength > 2) {
            for (int i = 0; i < bodyLength; i++) {
                if (head.x == body[i].x && head.y == body[i].y) gameOver = true;
            }
        }

        // Lógica de spawn da uva
        if (grape.isVisible && currentTime - grape.spawnTime >= 3.0f) {
            grape.isVisible = false;
            lastGrapeAppearanceTime = currentTime;
        } else if (!grape.isVisible && currentTime - lastGrapeAppearanceTime >= 12.0f) {
            GenerateValidPositionForFruit(NULL, &grape, NULL, head, body, bodyLength, false, true);
            grape.isVisible = true;
            grape.spawnTime = currentTime;
        }

        // Lógica de spawn da laranja
        if (orange.isVisible && currentTime - orange.spawnTime >= 15.0f) {
            orange.isVisible = false;
            lastOrangeAppearanceTime = currentTime;
        } else if (!orange.isVisible && currentTime - lastOrangeAppearanceTime >= 30.0f) {
            GenerateValidPositionForFruit(NULL, NULL, &orange, head, body, bodyLength, false, false);
            orange.isVisible = true;
            orange.spawnTime = currentTime;
        }


        // Verificar se comeu a maçã
        if (head.x == apple.x && head.y == apple.y) {
            score++;
            body[bodyLength] = body[bodyLength - 1]; // Cresce o corpo
            bodyLength++;
            GenerateValidPositionForFruit(&apple, NULL, NULL, head, body, bodyLength, true, false);
        }

        // Verificar se comeu a uva
        if (grape.isVisible && head.x == grape.x && head.y == grape.y) {
            score += 3;
            for (int i = 0; i < 3; i++) body[bodyLength++] = body[bodyLength - 1];
            grape.isVisible = false;
            lastGrapeAppearanceTime = currentTime;
        }

        // Verificar se comeu a laranja
        if (orange.isVisible && head.x == orange.x && head.y == orange.y) {
            score += 10;
            for (int i = 0; i < 10; i++) body[bodyLength++] = body[bodyLength - 1];
            orange.isVisible = false;
            lastOrangeAppearanceTime = currentTime;
        }



        // Verificar limites da tela
        if (head.x < 0 || head.x >= GetScreenWidth() || head.y < 0 || head.y >= GetScreenHeight()) gameOver = true;

        // Desenhar
        BeginDrawing();
        ClearBackground(BLACK);

        // Desenhar grid
        for (int i = 0; i <= GetScreenWidth(); i += 100) {
            for (int j = 0; j <= GetScreenHeight(); j += 100) DrawRectangle(i, j, 50, 50, DARKGRAY);
            for (int j = 0; j <= GetScreenHeight(); j += 100) DrawRectangle(i + 50, j + 50, 50, 50, DARKGRAY);
        }

        // Desenhar cabeça e corpo
        DrawRectangle(head.x, head.y, 50, 50, GREEN);
        if (open) {
            DrawRectangle(head.x + 10, head.y + 10, 30, 30, BLACK);
        } else {
            DrawRectangle(head.x + 5, head.y + 20, 40, 10, BLACK);
        }
        for (int i = 0; i < bodyLength; i++) {
            if (i % 2) {
                DrawRectangle(body[i].x, body[i].y, 50, 50, GREEN);
            } else {
                DrawRectangle(body[i].x, body[i].y, 50, 50, DARKGREEN);
            }
        }

        // Desenhar maçã
        DrawRectangle(apple.x + 5, apple.y + 5, 40, 40, RED);
        DrawRectangle(apple.x + 20, apple.y, 20, 10, LIME);

        // Desenhar uva
        if (grape.isVisible) {
            DrawRectangle(grape.x + 5, grape.y + 5, 18, 18, PURPLE);
            DrawRectangle(grape.x + 27, grape.y + 5, 18, 18, PURPLE);
            DrawRectangle(grape.x + 16, grape.y + 28, 18, 18, PURPLE);
            DrawRectangle(grape.x + 20, grape.y, 20, 10, LIME);
        }

        if (orange.isVisible) {
            DrawRectangle(orange.x + 5, orange.y + 5, 40, 40, ORANGE);
            DrawRectangle(orange.x + 20, orange.y, 20, 10, LIME);
        }

        // Desenhar HUD do placar
        DrawText(TextFormat("Score: %d", score), 10, 10, 30, WHITE);

        // Verificar condição de vitória
        if (bodyLength >= 144) {
            victory = true;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
