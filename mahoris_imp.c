/*
    1〜3のランダムな数の連なったブロックが上から落ちてきて、一列並べると消えるプログラム(簡易的なテトリス)

    操作方法
    最初の画面でeかnかhを押すとゲームスタート
    eはイージーモード（ゆっくり）
    nはノーマル（中くらい）
    hはハード（高速）
    aを1回押すと左に1ブロック分動く
    dを1回押すと右に1ブロック分動く
    sを1回押すと縦と横が切り替わる
    スペースキーで途中終了

    ブロックの落ちる速さは変わらない
    8ブロック縦に積み上がるとゲームオーバー
    その場合ゲームオーバーの文字とキー入力を促す文字が現れ、キー入力の後プログラムが終了する

    不具合
    右端や他のブロックに囲まれているところで縦から横に切り替えると、ブロックが埋まってしまったり挙動がおかしくなったりする
    表示される上限以上のブロックを積んだ時、ブロックを揃えても上のブロックが表示されたままになる
    ハードモードの時に、記録されるブロックの位置がずれることがある
*/

#include <handy.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// テキストを真ん中に表示させる
#define CenteredText(layer, x, y, ...)                                     \
    {                                                                      \
        double _dp1, _dp2;                                                 \
        HgWTextSize(layer, &_dp1, &_dp2, __VA_ARGS__);                     \
        HgWText((layer), (x - _dp1 / 2.0), (y - _dp2 / 2.0), __VA_ARGS__); \
    }

#define WINDOWSIZE_W 300  // ウィンドウサイズ横
#define WINDOWSIZE_H 550  // ウィンドウサイズ縦
#define SIZE 30           // 正方形の辺の長さ
#define BROCK_NUM 4       // ブロック数
#define HEIGHT_NUM 16     // 天井までのブロック数

typedef struct Brock {
    int x[BROCK_NUM];
    int y[BROCK_NUM];
    int color;
} Brock;

typedef struct Coordinate {
    int x[BROCK_NUM];
    int y[BROCK_NUM];
} Coordinate;

int box(Coordinate pC, int layer) {
    int i;
    for (i = 0; i < BROCK_NUM; i++) {
        HgWBoxFill(layer, pC.x[i], pC.y[i], SIZE, SIZE, 0);
    }
    return 0;
}

void starttxt(int layer) {
    // 操作方法などを表示する待機画面
    HgWSetFont(layer, HG_MB, 40);  // 明朝体太字でテトリスと表示
    HgWText(layer, WINDOWSIZE_W / 2 - 80, 300, "テトリス\n");
    HgWSetFont(layer, HG_G, 20);  // ゴシック体で操作説明を表示
    CenteredText(layer, WINDOWSIZE_W / 2, 250, "操作方法\n");
    CenteredText(layer, WINDOWSIZE_W / 2, 220, "a:右移動\n");
    CenteredText(layer, WINDOWSIZE_W / 2, 200, "d:左移動\n");
    CenteredText(layer, WINDOWSIZE_W / 2, 180, "s:縦横切り替え\n");
    CenteredText(layer, WINDOWSIZE_W / 2, 100, "キー入力でゲームスタート\n");
    CenteredText(layer, WINDOWSIZE_W / 2, 80, "スペースキー入力でゲーム終了\n");
}

void gameover(int layer) {
    int i;
    // ゲームオーバーになった時
    HgWSetFont(layer, HG_MB, 15);               // 明朝体太字で表示
    for (i = WINDOWSIZE_H; i >= 250; i -= 5) {  // 上から文字が降りてくる
        HgLClear(layer);
        CenteredText(layer, WINDOWSIZE_W / 2, i, "ゲームオーバー\n");
        HgSleep(0.01);
    }
    HgSleep(0.5);
    CenteredText(layer, WINDOWSIZE_W / 2, 100,
                 "キー入力でウィンドウが閉じます\n");
}

Coordinate move_Calc(Brock *pB, int x_move, int y_move) {
    int i;
    Coordinate coordinate;
    for (i = 0; i < 4; i++) {
        coordinate.x[i] = (pB->x[i] + x_move) * (SIZE + 5);
        coordinate.y[i] = (pB->y[i] + y_move) * (SIZE + 5);
    }
    return coordinate;
}

Brock rotate_Calc(Brock pB, int brock_num, int num1) {
    int i;
    Brock brock;
    int num2 = num1 % 2;
    for (i = 0; i < BROCK_NUM; i++) {
        if (brock_num == 0 || brock_num == 4 && num2 == 1) {
            brock.x[i] = pB.y[i];
            brock.y[i] = pB.x[i];
        } else if (brock_num == 4 || brock_num == 5 && num2 == 0) {
            brock.x[i] = pB.y[i];
            brock.y[i] = pB.x[i] ^ 1;
        } else if (brock_num == 6 && num2 == 1) {
            brock.x[i] = pB.y[i] ^ 1;
            brock.y[i] = pB.x[i];
        } else if ((brock_num == 1 && num2 == 1) ||
                   (brock_num == 2 && num2 == 0)) {
            brock.x[i] = pB.y[i];
            brock.y[i] = pB.x[BROCK_NUM - i - 1];
        } else if ((brock_num == 2 && num2 == 1) ||
                   (brock_num == 1 && num2 == 0)) {
            brock.x[i] = pB.y[BROCK_NUM - i - 1];
            brock.y[i] = pB.x[i];
        } else if (brock_num == 5 && num2 == 1 || brock_num == 6 && num2 == 0) {
            brock.x[i] = pB.y[i];
            brock.y[i] = pB.x[i];
            if (i == 3) {
                switch (num1 % 4) {
                    case 0:
                        brock.x[i] = 2;
                        break;
                    case 1:
                        brock.y[i] = 2;
                        break;
                    case 2:
                        brock.x[i] = 0;
                        break;
                    case 3:
                        brock.y[i] = 0;
                        break;
                }
            }
        } else {
            brock = pB;
            break;
        }
    }
    return brock;
}

int wall(Coordinate *pC, int array[9][HEIGHT_NUM], int num) {
    int i;
    for (i = 0; i < BROCK_NUM; i++) {
        if (num < HEIGHT_NUM - 3 &&
            (pC->x[i] + SIZE > WINDOWSIZE_W || pC->x[i] < 10 ||
             pC->x[i] / (SIZE + 5) != 0 &&
                 array[pC->x[i] / (SIZE + 5) - 1][pC->y[i] / (SIZE + 5)] == 1 ||
             pC->x[i] / (SIZE + 5) != 9 &&
                 array[pC->x[i] / (SIZE + 5) + 1][pC->y[i] / (SIZE + 5)]))
            return 1;
    }
    return 0;
}

int height(Coordinate *pC, int array[9][HEIGHT_NUM]) {
    int j;
    for (j = 0; j < BROCK_NUM; j++) {
        if (pC->y[j] / (SIZE + 5) != 0 &&
            array[pC->x[j] / (SIZE + 5)][pC->y[j] / (SIZE + 5) - 1] == 1) {
            return pC->y[j] / (SIZE + 5);
        }
    }
    return 0;
}

int main() {
    hgevent *event;  // イベントのためのもの
    doubleLayer layers;
    Brock brocks[7] = {{{0, 1, 2, 3}, {0, 0, 0, 0}, HG_SKYBLUE},
                       {{0, 1, 1, 2}, {0, 0, 1, 1}, HG_RED},
                       {{0, 1, 1, 2}, {1, 1, 0, 0}, HG_GREEN},
                       {{0, 1, 0, 1}, {1, 1, 0, 0}, HG_YELLOW},
                       {{1, 0, 1, 2}, {1, 0, 0, 0}, HG_PURPLE},
                       {{0, 1, 2, 0}, {0, 0, 0, 1}, HG_BLUE},
                       {{0, 1, 2, 2}, {0, 0, 0, 1}, HG_ORANGE}};
    Brock game_brock;
    Coordinate coordinate;

    int layer1, layer2;
    int i, j, k;
    int key;
    int random_num;
    int x_move, y_move;
    int sum;
    int num;
    int array[9][HEIGHT_NUM] = {};

    y_move = 0;

    HgOpen(WINDOWSIZE_W, WINDOWSIZE_H);

    HgSetEventMask(HG_KEY_DOWN);

    layer1 = HgWAddLayer(0);
    layer2 = HgWAddLayer(0);
    layers = HgWAddDoubleLayer(0);

    HgSetTitle("Tetoris");
    HgWSetFillColor(layer1, HG_LGRAY);

    starttxt(layer1);
    HgGetChar();
    HgClear();

    srandom(time(NULL));
    while (y_move < HEIGHT_NUM) {
        random_num = random() % 7;
        game_brock = brocks[random_num];
        x_move = 3;
        num = 0;

        for (i = HEIGHT_NUM - 1; i >= 0 + y_move; i--) {
            y_move = 0;
            
            event = HgEventNonBlocking();
            if (event != NULL) {
                key = event->ch;
                if (key == 'd' && wall(&coordinate, array, i) == 0) x_move += 1;
                if (key == 'a' && wall(&coordinate, array, i) == 0) x_move -= 1;
                if (key == 's') {
                    num++;
                    game_brock = rotate_Calc(game_brock, random_num, num);
                }
                if (key == ' ') {
                    HgClear();
                    CenteredText(layer1, WINDOWSIZE_W / 2, WINDOWSIZE_H / 2,
                                 "終了します\n");
                    HgSleep(3);  // 3秒後に終了
                    HgClose();
                    return 0;
                }
            }
            
            coordinate = move_Calc(&game_brock, x_move, i);
            y_move = height(&coordinate, array);

            layer2 = HgLSwitch(&layers);
            HgLClear(layer2);

            HgWSetFillColor(layer2, brocks[random_num].color);

            box(coordinate, layer2);
            HgSleep(0.1);
        }

        for (i = 0; i < BROCK_NUM; i++) {
            array[coordinate.x[i] / (SIZE + 5)][coordinate.y[i] / (SIZE + 5)] =
                1;
        }

        for (i = 0; i < HEIGHT_NUM; i++) {
            sum = 0;
            if (array[0][i] == 0) continue;
            for (k = 0; k < 9; k++) {
                sum += array[k][i];
            }
            if (sum == 9) {
                for (j = i; j < HEIGHT_NUM - 1; j++) {
                    for (k = 0; k < 9; k++) {
                        array[k][j] = array[k][j + 1];
                    }
                }
                i--;
            }
        }

        HgLClear(layer1);
        for (i = 0; i < HEIGHT_NUM; i++) {
            for (k = 0; k < 9; k++) {
                if (array[k][i] == 1) {
                    HgWBoxFill(layer1, k * (SIZE + 5), i * (SIZE + 5), SIZE,
                               SIZE, 1);
                }
            }
        }
    }
    gameover(layer1);

    HgGetChar();
    HgClose();

    return 0;
}
