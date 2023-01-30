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
#define WINDOWSIZE_H 600  // ウィンドウサイズ縦
#define SIZE 30           // 正方形の辺の長さ
#define BROCK_NUM 4       // ブロック数

typedef struct Brock {
    int x[BROCK_NUM];
    int y[BROCK_NUM];
    int color;
} Brock;

typedef struct Coordinate {
    int x[BROCK_NUM];
    int y[BROCK_NUM];
} Coordinate;

int layer1, layer2;  // レイヤーを分けるための変数
int x, y;            // 図形の座標
int v;               // 落ちる速度

int box(Coordinate pC, int layer) {
    int i;
    for (i = 0; i < BROCK_NUM; i++) {
        HgWBoxFill(layer, pC.x[i], pC.y[i], SIZE, SIZE, 0);
    }
    return 0;
}

void starttxt(void) {
    // 黒い壁と底を表示（ベースレイヤー）
    HgSetFillColor(HG_BLACK);
    // 壁と底の厚さはそれぞれ20
    HgBoxFill(0, 0, 20, WINDOWSIZE_H, 0);
    HgBoxFill(WINDOWSIZE_W - 20, 0, 20, WINDOWSIZE_H, 0);
    HgBoxFill(20, 0, WINDOWSIZE_W - 40, 20, 0);

    // 操作方法などを表示する待機画面
    HgWSetFont(layer1, HG_MB, 40);  // 明朝体太字でテトリスと表示
    HgWText(layer1, WINDOWSIZE_W / 2 - 80, 300, "テトリス\n");
    HgWSetFont(layer1, HG_G, 20);  // ゴシック体で操作説明を表示
    /*CenteredText(layer1, WINDOWSIZE_W / 2, 250, "操作方法\n");
    CenteredText(layer1, WINDOWSIZE_W / 2, 220, "a:右移動\n");
    CenteredText(layer1, WINDOWSIZE_W / 2, 200, "d:左移動\n");
    CenteredText(layer1, WINDOWSIZE_W / 2, 180, "s:縦横切り替え\n");
    CenteredText(layer1, WINDOWSIZE_W / 2, 100, "キー入力でゲームスタート\n");
    CenteredText(layer1, WINDOWSIZE_W / 2, 80,
                 "スペースキー入力でゲーム終了\n");
    CenteredText(layer1, WINDOWSIZE_W / 2, 60,
                 "e:イージー n:ノーマル h:ハード\n");*/
}

void gameover(void) {
    // ゲームオーバーになった時
    HgWSetFont(layer1, HG_MB, 20);              // 明朝体太字で表示
    for (y = WINDOWSIZE_H; y >= 250; y += v) {  // 上から文字が降りてくる
        HgLClear(layer1);
        CenteredText(layer1, WINDOWSIZE_W / 2, y, "ゲームオーバー\n");
        HgSleep(0.01);
    }
    HgSleep(0.5);
    CenteredText(layer1, WINDOWSIZE_W / 2, 100,
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

Brock rotate_Calc(Brock pB, int brock_num, int num) {
    int i;
    Brock brock;

    for (i = 0; i < BROCK_NUM; i++) {
        if (brock_num == 0 || brock_num == 4 && (num == 1 || num == 3)) {
            brock.x[i] = pB.y[i];
            brock.y[i] = pB.x[i];
        } else if (brock_num == 4) {
            brock.x[i] = pB.y[i];
            brock.y[i] = pB.x[i] ^ 1;
        } else if ((brock_num == 1 && num == 1) ||
                   (brock_num == 2 && num == 0)) {
            brock.x[i] = pB.y[i];
            brock.y[i] = pB.x[BROCK_NUM - i - 1];
        } else if ((brock_num == 2 && num == 1) ||
                   (brock_num == 1 && num == 0)) {
            brock.x[i] = pB.y[BROCK_NUM - i - 1];
            brock.y[i] = pB.x[i];
        } else {
            brock = pB;
            break;
        }
    }
    return brock;
}

int wall(Coordinate *pC, int array[9][15]) {
    int i;
    for (i = 0; i < BROCK_NUM; i++) {
        if (pC->x[i] + SIZE > WINDOWSIZE_W || pC->x[i] < 10 ||
            pC->x[i] / (SIZE + 5) != 0 &&
                array[pC->x[i] / (SIZE + 5) - 1][pC->y[i] / (SIZE + 5)] == 1 ||
            pC->x[i] / (SIZE + 5) != 9 &&
                array[pC->x[i] / (SIZE + 5) + 1][pC->y[i] / (SIZE + 5)])
            return 1;
    }
    return 0;
}

// #define DEBUG
#ifdef DEBUG

int main() {
    hgevent *event;  // イベントのためのもの
    doubleLayer layers;
    Brock brocks[5] = {{{1, 2, 3, 4}, {1, 1, 1, 1}, HG_BLUE},
                       {{1, 2, 2, 3}, {1, 1, 2, 2}, HG_RED},
                       {{1, 2, 2, 3}, {2, 2, 1, 1}, HG_GREEN},
                       {{1, 2, 1, 2}, {2, 2, 1, 1}, HG_YELLOW},
                       {{2, 1, 2, 3}, {2, 1, 1, 1}, HG_PURPLE}};

    // 変数の宣言
    int key;      // 入力したキーを覚えておくための変数
    int i, j, k;  // カウンタ変数
    int random;   // 乱数を覚えておくための変数
    int beside, height;  // ブロックがどこにあるか確認するために配列で使う変数
    int tate;   // y座標を変えるための変数
    int count;  // 横と縦を切り替えるためのカウント変数
    int num;    // 消した列の数を数えるための変数
    int sum;
    int array[8][10] = {};  // どこにブロックが落ちたか覚えておくための配列

    tate = 0;  // 初期化
    num = 0;

    HgOpen(WINDOWSIZE_W, WINDOWSIZE_H);  // ウィンドウを表示

    // 2つレイヤーを追加する
    layer1 = HgWAddLayer(0);  // アニメーション用レイヤー
    layer2 = HgWAddLayer(0);  // 記録用レイヤー
    layers = HgWAddDoubleLayer(0);

    HgLMove(layer1, 1);  // アニメーション用レイヤーを最前面に移動させておく

    starttxt();

    // sが入力されるまで待機
    for (;;) {
        key = HgGetChar();
        if (key == 'e') {
            v = -5;
            HgText(320, 460, "イージー\n");
            break;
        } else if (key == 'n') {
            HgText(320, 460, "ノーマル\n");
            v = -10;
            break;
        } else if (key == 'h') {
            HgText(330, 460, "ハード\n");
            v = -25;
            break;
        }
    }

    HgSetEventMask(HG_KEY_DOWN);  // キー入力のためのイベントセット

    while (tate != 10) {  // 以下ゲーム部分
        // 乱数を一つ生成
        // 乱数でブロッック数を1〜3にする
        srand(time(NULL));
        random = rand() % 5;
        // printf("%d\n", random); //確認用

        // ブロックは端から3つ目から横で登場
        x = 145;
        count = 0;

        for (y = WINDOWSIZE_H; y >= 25 + 60 * tate;
             y += v) {  // 上から下に落ちるループ

            layer1 = HgLSwitch(&layers);
            HgLClear(layer1);  // レイヤー1を消してアニメーションを作る

            // 初期化
            beside = (x - 25) / 60;
            height = (y - 25) / 60;
            tate = 0;

            event = HgEventNonBlocking();  // キー入力のイベント

            // key入力
            if (event != NULL) {  // キー入力がされなくても動かし続ける
                key = event->ch;  // 入力されたキーの情報をkeyに入れる

                // dの入力で右に一つ分動かす＋右に行きすぎないようにするための条件
                // 一つにまとめると長すぎてわかりにくいので縦と横、ブロックの数でそれぞれ分けて書いた
                if (key == 'd' && count % 2 == 0 &&
                    ((x < 160 && random == 2) || (x < 300 && random == 0) ||
                     (x < 240 && random == 1))) {  // 動ブロックが横の場合
                    // 動ブロック右端の右横にブロックがない場合は動ブロック全体をを右に動かせる
                    if ((random == 0 && beside <= 4 &&
                         array[beside + 1][height] == 0) ||
                        (random == 1 && beside <= 3 &&
                         array[beside + 2][height] == 0) ||
                        (random == 2 && beside <= 2 &&
                         array[beside + 3][height] == 0)) {
                        x += 60;
                        // printf("%c\n", key); //確認用
                    }
                } else if (key == 'd' && count % 2 == 1 &&
                           (x < 300)) {  // 動ブロックが縦の場合
                    // 全ての動ブロックの右横にブロックがない場合は動ブロックを右に動かせる
                    if ((random == 0 && beside <= 4 &&
                         array[beside + 1][height] == 0) ||
                        (random == 1 && beside <= 4 &&
                         array[beside + 1][height] +
                                 array[beside + 1][height + 1] ==
                             0) ||
                        (random == 2 && beside <= 4 &&
                         array[beside + 1][height] +
                                 array[beside + 1][height + 1] +
                                 array[beside + 1][height + 2] ==
                             0)) {
                        x += 60;
                        // printf("%c\n", key); //確認用
                    }
                }

                // aの入力で左に一つ動かす＋左に行きすぎないようにするための条件
                if (key == 'a' && x > 25) {
                    // 横の場合
                    if (count % 2 == 0 &&
                        (beside >= 1 && array[beside - 1][height] == 0)) {
                        x -= 60;
                        // printf("%c\n", key); //確認用
                    }
                    // 縦の場合
                    else if (count % 2 == 1 &&
                                 (random == 0 && beside >= 1 &&
                                  array[beside - 1][height] == 0) ||
                             (random == 1 && beside >= 1 &&
                              array[beside - 1][height] +
                                      array[beside - 1][height + 1] ==
                                  0) ||
                             (random == 2 && beside >= 1 &&
                              array[beside - 1][height] +
                                      array[beside - 1][height + 1] +
                                      array[beside - 1][height + 2] ==
                                  0)) {
                        x -= 60;
                        // printf("%c\n", key); //確認用
                    }
                }
                // sの入力でcountを増やす, countは縦横の切り替えに使う
                if (key == 's') {
                    count++;
                }

                // スペースキー入力でプログラム終了
                if (key == ' ') {
                    HgLClear(layer1);
                    CenteredText(layer1, WINDOWSIZE_W / 2, WINDOWSIZE_H / 2,
                                 "終了します\n");
                    HgSleep(3);  // 3秒後に終了
                    HgClose();
                    return 0;
                }
            }

            // 下にブロックがあったらその上に積む
            for (i = 0; i <= height; i++) {
                if (count % 2 == 0) {   // 横の場合
                    if (random == 0) {  // ブロックが1個の場合
                        if (array[beside][height] == 1) {
                            tate++;
                        }
                    } else if (random == 1) {  // ブロックが2個の場合
                        if (array[beside][height] == 1 ||
                            array[beside + 1][height] == 1) {
                            tate++;
                        }
                    } else if (random == 2) {  // ブロックが3個の場合
                        if (array[beside][height] == 1 ||
                            array[beside + 1][height] == 1 ||
                            array[beside + 2][height]) {
                            tate++;
                        }
                    }
                } else {  // 縦の場合
                    if (array[beside][height] == 1) {
                        tate++;
                    }
                }
            }

            // ブロックを描く
            if (count % 2 == 0) {
                // 1〜3個（乱数で決定する）の四角を横に並べて表示
                for (i = 0; i <= random; i++) {
                    HgWBoxFill(layer1, x + 60 * i, y, SIZE, SIZE, 0);
                }
            } else {
                // 1〜3個（乱数で決定する）の四角を縦に並べて表示
                for (i = 0; i <= random; i++) {
                    HgWBoxFill(layer1, x, y + 60 * i, SIZE, SIZE, 0);
                }
            }

            HgSleep(0.05);  // 描画をゆっくりにする
        }

        // 配列をijの繰り返しと乱数で出た数を使ってx、yを計算してブロックの配置を記録させる
        // どこにブロックが落ちたか覚えておくための処理
        if (count % 2 == 0) {  // 横の場合
            for (i = 0; i <= random; i++) {
                array[beside + i][tate] = 1;
                // printf("(%d,%d,%d)\n", beside + i, tate,
                //        array[beside + i][tate]); //確認用
            }
        } else {  // 縦の場合
            for (i = 0; i <= random; i++) {
                array[beside][tate + i] = 1;
                // printf("(%d,%d,%d)\n", beside, tate + i,
                //        array[beside][tate + i]); //確認用
            }
        }

        // 一列揃ったら消す
        for (i = 0; i < 10; i++) {
            sum = 0;
            for (j = 0; j < 8; j++) {
                sum = array[j][i];
            }
            if (sum == 6) {
                for (j = i; j < 8; j++) {
                    for (k = 0; k < 6; k++) {
                        array[k][j] = array[k][j + 1];
                    }
                }
                num++;
                i--;
            }
        }

        // 落下したブロック（配列の中身が1になっている場所にあるブロック）をレイヤー2で表示
        HgLClear(layer2);
        for (i = 0; i < 10; i++) {
            for (j = 1; j < 7; j++) {
                if (array[j][i] == 1) {
                    box(brocks[random]);
                }
            }
        }

        HgWSetFont(layer2, HG_M, 15);
        HgWText(layer2, 30, 460, "消した行の数%d\n", num);
    }

    gameover();

    // キー入力でウィンドウを閉じてプログラム終了
    HgGetChar();
    HgClose();
    return 0;
}

#else

int main() {
    hgevent *event;  // イベントのためのもの
    doubleLayer layers;
    Brock brocks[5] = {{{0, 1, 2, 3}, {0, 0, 0, 0}, HG_BLUE},
                       {{0, 1, 1, 2}, {0, 0, 1, 1}, HG_RED},
                       {{0, 1, 1, 2}, {1, 1, 0, 0}, HG_GREEN},
                       {{0, 1, 0, 1}, {1, 1, 0, 0}, HG_ORANGE},
                       {{1, 0, 1, 2}, {1, 0, 0, 0}, HG_PURPLE}};

    Brock game_brock;
    Coordinate coordinate;

    int layer1, layer2;
    int i, j, k, l;
    int key;
    int random_num;
    int x_move, y_move;
    int x_max, x_min, y_max, y_min;
    int sum;
    int num;
    int array[9][15] = {};

    y_move = 0;

    HgOpen(WINDOWSIZE_W, WINDOWSIZE_H);

    HgSetEventMask(HG_KEY_DOWN);

    layer1 = HgWAddLayer(0);
    layer2 = HgWAddLayer(0);
    layers = HgWAddDoubleLayer(0);

    HgSetTitle("Tetoris");
    HgWSetFillColor(layer1, HG_LGRAY);

    // starttxt();
    // HgGetChar();

    srandom(time(NULL));
    while (y_move < 9) {
        random_num = random() % 5;
        game_brock = brocks[random_num];
        x_move = 3;
        num = 0;
        HgWSetFillColor(layer2, game_brock.color);

        for (i = 10; i >= 0 + y_move; i--) {
            event = HgEventNonBlocking();

            y_move = 0;

            if (event != NULL) {
                key = event->ch;
                if (key == HG_R_ARROW && wall(&coordinate, array) == 0)
                    x_move += 1;
                if (key == HG_L_ARROW && wall(&coordinate, array) == 0)
                    x_move -= 1;
                if (key == HG_U_ARROW) {
                    num++;
                    game_brock = rotate_Calc(game_brock, random_num, num % 2);
                }
                if (key == ' ') {
                    HgLClear(layer2);
                    CenteredText(layer2, WINDOWSIZE_W / 2, WINDOWSIZE_H / 2,
                                 "終了します\n");
                    HgSleep(3);  // 3秒後に終了
                    HgClose();
                    return 0;
                }
            }
            coordinate = move_Calc(&game_brock, x_move, i);

            for (j = 0; j < BROCK_NUM; j++) {
                if (coordinate.y[j] / (SIZE + 5) != 0 &&
                    array[coordinate.x[j] / (SIZE + 5)]
                         [coordinate.y[j] / (SIZE + 5) - 1] == 1) {
                    y_move = coordinate.y[j] / (SIZE + 5);
                    break;
                    // printf("%d\n",y_move);
                }
            }
            HgLClear(layer2);
            box(coordinate, layer2);
            HgSleep(0.5);
        }

        for (i = 0; i < BROCK_NUM; i++) {
            array[coordinate.x[i] / (SIZE + 5)][coordinate.y[i] / (SIZE + 5)] =
                1;
            printf("%d,%d\n", coordinate.x[i] / (SIZE + 5),
                   coordinate.y[i] / (SIZE + 5));
        }

        for (i = 0; i < 15; i++) {
            sum = 0;
            if (array[0][i] == 0) continue;
            for (k = 0; k < 9; k++) {
                sum += array[k][i];
            }
            if (sum == 9) {
                for (j = i; j < 14; j++) {
                    for (k = 0; k < 9; k++) {
                        array[k][j] = array[k][j + 1];
                    }
                }
                j--;
            }
        }

        HgLClear(layer1);
        for (i = 0; i < 15; i++) {
            for (k = 0; k < 9; k++) {
                if (array[k][i] == 1) {
                    HgWBoxFill(layer1, k * (SIZE + 5), i * (SIZE + 5), SIZE,
                               SIZE, 1);
                }
            }
        }
    }

    HgGetChar();
    HgClose();

    return 0;
}

#endif
