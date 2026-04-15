#include <iostream>
#include <windows.h>
#include <vector>
#include <ctime>

using namespace std;

const int WIDTH = 70;
const int HEIGHT = 35;

// 双缓冲
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);// 获取控制台句柄
CHAR_INFO buffer[HEIGHT][WIDTH];// 缓冲区
COORD bufferSize = { WIDTH, HEIGHT };// 缓冲区大小
COORD bufferCoord = { 0, 0 };// 缓冲区坐标
SMALL_RECT writeArea = { 0, 0, WIDTH - 1, HEIGHT - 1 };// 写入区域

// 游戏对象结构体
struct Player {
    int x, y;
    char c[3][5];
    int score;
};

struct Bullet {
    int x, y;
    bool active;
    bool isEnemy;
};

struct Enemy {
    int x, y;
    bool active;
    float speed;
    int points;
};

// 全局变量
Player player;
vector<Bullet> bullets;
vector<Enemy> enemies;
bool gameover = false;
int enemySpawnTimer = 0;
int gameTimer = 0;
int level = 1;

// 函数声明
void Setup();
void HideCursor();// 隐藏光标
void Draw();
void Input();
void Logic();
Enemy CreateEnemy();

// 初始化游戏
void Setup() {
    // 设置我方飞机形状
    player = { WIDTH / 2 - 2, HEIGHT - 10, {
        {' ', '/', '=', '\\', ' '},
        {'<', '<', '*', '>', '>'},
        {' ', '*', '=', '*', ' '}
    }, 10 };

    bullets.clear();
    enemies.clear();
    gameover = false;
    gameTimer = 0;
    level = 1;
    enemySpawnTimer = 0;

    // 初始化缓冲区
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            buffer[y][x].Char.AsciiChar = ' ';
            buffer[y][x].Attributes = 7;
        }
    }
}

// 隐藏光标
void HideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// 绘制游戏画面
void Draw() {
    // 清空缓冲区
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            buffer[y][x].Char.AsciiChar = ' ';
            buffer[y][x].Attributes = 7;
        }
    }

    // 绘制边框
    for (int x = 0; x < WIDTH; x++) {
        buffer[0][x].Char.AsciiChar = '#';
        buffer[HEIGHT - 1][x].Char.AsciiChar = '#';
    }
    for (int y = 0; y < HEIGHT; y++) {
        buffer[y][0].Char.AsciiChar = '#';
        buffer[y][WIDTH - 1].Char.AsciiChar = '#';
    }

    // 绘制玩家
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            int posY = player.y + i;
            int posX = player.x + j;
            if (player.c[i][j] != ' ' && posY < HEIGHT && posX < WIDTH) {
                buffer[posY][posX].Char.AsciiChar = player.c[i][j];
            }
        }
    }

    // 绘制子弹
    for (auto& b : bullets) {
        if (b.active && b.y < HEIGHT && b.x < WIDTH) {
            char bulletChar = b.isEnemy ? 'v' : '|';
            buffer[b.y][b.x].Char.AsciiChar = bulletChar;
        }
    }

    // 绘制敌机
    for (auto& e : enemies) {
        if (e.active) {
            char enemyShape[3][5] = {
                {' ', '/', '=', '\\', ' '},
                {'<', '<', '+', '>', '>'},
                {' ', '\\', '|', '/', ' '}
            };

            WORD enemyColor = 0x0C;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 5; j++) {
                    int posY = e.y + i;
                    int posX = e.x + j;
                    if (enemyShape[i][j] != ' ' && posY < HEIGHT && posX < WIDTH) {
                        buffer[posY][posX].Char.AsciiChar = enemyShape[i][j];
                        buffer[posY][posX].Attributes = enemyColor;
                    }
                }
            }
        }
    }

    // 输出缓冲区到控制台
    WriteConsoleOutput(hConsole, (CHAR_INFO*)buffer, bufferSize, bufferCoord, &writeArea);
}

// 输入处理
void Input() {
    // 游戏中输入处理
	if (GetAsyncKeyState('W') & 0x8000 && player.y > 1) player.y--;//&0x8000 检测按键是否被按下（提取最高位）
    if (GetAsyncKeyState('S') & 0x8000 && player.y < HEIGHT - 6) player.y++;
    if (GetAsyncKeyState('A') & 0x8000 && player.x > 1) player.x--;
    if (GetAsyncKeyState('D') & 0x8000 && player.x < WIDTH - 6) player.x++;

    // 连续发射子弹
    static int shootCooldown = 0;
    if (shootCooldown > 0) shootCooldown--;

    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (shootCooldown == 0) {
            bullets.push_back({ player.x + 2, player.y - 1, true, false});
        }
    }

    // 退出检测
    if (GetAsyncKeyState('Q') & 0x8000) {
        gameover = true;
    }
}

// 创建敌机
Enemy CreateEnemy() {
    Enemy enemy;
    enemy.speed = 0.5f + (level - 1) * 0.1f;
    enemy.points = 10;

    // 设置初始位置
    enemy.x = rand() % (WIDTH - 6) + 1;
    enemy.y = 1;
    enemy.active = true;

    return enemy;
}

// 游戏逻辑
void Logic() {
    gameTimer++;

    // 玩家子弹移动
    for (auto& b : bullets) {
        if (b.active) {
            b.y += b.isEnemy ? 1 : -1;
            if (b.y <= 0 || b.y >= HEIGHT - 1 || b.x <= 0 || b.x >= WIDTH - 1)
                b.active = false;
        }
    }

    // 敌机生成
    if (++enemySpawnTimer >= 40) {
        enemies.push_back(CreateEnemy());
        enemySpawnTimer = 0;
    }

    // 敌机移动与射击
    static float enemyMoveAccumulator = 0.0f;
    enemyMoveAccumulator += 0.2f;

    if (enemyMoveAccumulator >= 1.0f) {
        for (auto& e : enemies) {
            if (e.active) {
                // 随机左右移动
                if (rand() % 10 == 0)
                    e.x += (rand() % 3) - 1;

                // 根据速度移动
                static float enemySpeedAccumulator[1000] = { 0 };
                enemySpeedAccumulator[e.x + e.y * WIDTH] += e.speed;

                if (enemySpeedAccumulator[e.x + e.y * WIDTH] >= 1.0f) {
                    e.y += static_cast<int>(enemySpeedAccumulator[e.x + e.y * WIDTH]);
                    enemySpeedAccumulator[e.x + e.y * WIDTH] -= static_cast<int>(enemySpeedAccumulator[e.x + e.y * WIDTH]);
                }

                e.x = max(1, min(WIDTH - 6, e.x));

                // 射击冷却
                if (rand() % 100 < 2) {
                    bullets.push_back({ e.x + 2, e.y + 3, true, true});
                }

                // 敌机飞出屏幕
                if (e.y > HEIGHT - 4) {
                    e.active = false;
                    player.score--;
                    if (player.score <= 0) {
                        gameover = true;
                    }
                }
            }
        }

        enemyMoveAccumulator -= static_cast<int>(enemyMoveAccumulator);
    }

    // 碰撞检测：玩家子弹 vs 敌机
    for (auto& b : bullets) {
        if (!b.active || b.isEnemy) continue;

        for (auto& e : enemies) {
            if (e.active) {
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 5; j++) {
                        char enemyShape[3][5] = {
                            {' ', '/', '=', '\\', ' '},
                            {'<', '<', '+', '>', '>'},
                            {' ', '\\', '|', '/', ' '}
                        };

                        if (b.x == e.x + j && b.y == e.y + i && enemyShape[i][j] != ' ') {
                            b.active = false;
                            e.active = false;
                            player.score += 1;
                            break;
                        }
                    }
                    if (!b.active) break;
                }
            }
        }
    }

    // 碰撞检测：敌机子弹 vs 玩家
    for (auto& b : bullets) {
        if (b.active && b.isEnemy) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 5; j++) {
                    if (b.x == player.x + j && b.y == player.y + i) {
                        b.active = false;
                        player.score--;

                        if (player.score <= 0) {
                            gameover = true;
                        }
                        break;
                    }
                }
                if (player.score <= 0) break;
            }
        }
    }

    // 碰撞检测：敌机 vs 玩家
    for (auto& e : enemies) {
        if (e.active) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 5; j++) {
                    for (int pi = 0; pi < 3; pi++) {
                        for (int pj = 0; pj < 5; pj++) {
                            char enemyShape[3][5] = {
                                {' ', '/', '=', '\\', ' '},
                                {'<', '<', '+', '>', '>'},
                                {' ', '\\', '|', '/', ' '}
                            };

                            if (enemyShape[i][j] != ' ' && player.c[pi][pj] != ' ' &&
                                e.x + j == player.x + pj && e.y + i == player.y + pi) {

                                e.active = false;
                                player.score--;

                                if (player.score <= 0) {
                                    gameover = true;
                                }
                                return;
                            }
                        }
                    }
                }
            }
        }
    }

    // 清理无效对象
    bullets.erase(remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return !b.active; }), bullets.end());
    enemies.erase(remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return !e.active; }), enemies.end());
}

int main() {
    srand(time(NULL));
    HideCursor();

    // 设置窗口大小
    SMALL_RECT windowSize = { 0, 0, WIDTH + 5, HEIGHT + 20 };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

    Setup();

    while (!gameover) {
        Draw();
        Logic();
        Input();
        Sleep(30);
    }

    system("cls");
    cout << "游戏结束！最终得分: " << player.score << endl;
    cout << "按任意键退出..." << endl;


    return 0;
}