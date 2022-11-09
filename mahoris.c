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

#define WINDOWSIZE_W 400  //ウィンドウサイズ横
#define WINDOWSIZE_H 500  //ウィンドウサイズ縦
#define SIZE 50           //正方形の辺の長さ

int layer1, layer2;  //レイヤーを分けるための変数

int box(int x, int y) { 
    HgWBoxFill(layer1, x, y, SIZE, SIZE, 0); 
    return 0;
}

int main() {
    hgevent *event;  //イベントのためのもの
    //変数の宣言
    int key;      //入力したキーを覚えておくための変数
    int x, y;     //図形の座標
    int v;        //落ちる速度
    int i, j, k;  //カウンタ変数
    int random;   //乱数を覚えておくための変数
    int beside, height;  //ブロックがどこにあるか確認するために配列で使う変数
    int tate;   // y座標を変えるための変数
    int count;  //横と縦を切り替えるためのカウント変数
    int num;    //消した列の数を数えるための変数
    int layer1, layer2;  //レイヤーを分けるための変数
    int array[6][9] = {};  //どこにブロックが落ちたか覚えておくための配列

    tate = 0;  //初期化
    num = 0;

    HgOpen(WINDOWSIZE_W, WINDOWSIZE_H);  //ウィンドウを表示

    // 2つレイヤーを追加する
    layer1 = HgWAddLayer(0);  //アニメーション用レイヤー
    layer2 = HgWAddLayer(0);  //記録用レイヤー

    HgLMove(layer1, 1);  //アニメーション用レイヤーを最前面に移動させておく

    //黒い壁と底を表示（ベースレイヤー）
    HgSetFillColor(HG_BLACK);
    //壁と底の厚さはそれぞれ20
    HgBoxFill(0, 0, 20, WINDOWSIZE_H, 0);
    HgBoxFill(WINDOWSIZE_W - 20, 0, 20, WINDOWSIZE_H, 0);
    HgBoxFill(20, 0, WINDOWSIZE_W - 40, 20, 0);

    //操作方法などを表示する待機画面
    HgWSetFont(layer1, HG_MB, 40);  //明朝体太字でテトリスと表示
    HgWText(layer1, WINDOWSIZE_W / 2 - 80, 300, "テトリス\n");
    HgWSetFont(layer1, HG_G, 20);  //ゴシック体で操作説明を表示
    CenteredText(layer1, WINDOWSIZE_W / 2, 250, "操作方法\n");
    CenteredText(layer1, WINDOWSIZE_W / 2, 220, "a:右移動\n");
    CenteredText(layer1, WINDOWSIZE_W / 2, 200, "d:左移動\n");
    CenteredText(layer1, WINDOWSIZE_W / 2, 180, "s:縦横切り替え\n");
    CenteredText(layer1, WINDOWSIZE_W / 2, 100, "キー入力でゲームスタート\n");
    CenteredText(layer1, WINDOWSIZE_W / 2, 80, "スペースキー入力でゲーム終了\n");
    CenteredText(layer1, WINDOWSIZE_W / 2, 60, "e:イージー n:ノーマル h:ハード\n");

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

    HgSetEventMask(HG_KEY_DOWN);  //キー入力のためのイベントセット

    //レイヤー1と2で図形の色をかえる
    HgWSetFillColor(layer1, HG_BLUE);   //青
    HgWSetFillColor(layer2, HG_GREEN);  //緑

    for (;;) {  //以下ゲーム部分
        //乱数を一つ生成
        //乱数でブロッック数を1〜3にする
        srand(time(NULL));
        random = rand() % 3;
        // printf("%d\n", random); //確認用

        //ブロックは端から3つ目から横で登場
        x = 145;
        count = 0;

        for (y = WINDOWSIZE_H; y >= 25 + 60 * tate;
             y += v) {  //上から下に落ちるループ

            HgLClear(layer1);  //レイヤー1を消してアニメーションを作る

            //初期化
            beside = (x - 25) / 60;
            height = (y - 25) / 60;
            tate = 0;

            event = HgEventNonBlocking();  //キー入力のイベント

            // key入力
            if (event != NULL) {  //キー入力がされなくても動かし続ける
                key = event->ch;  //入力されたキーの情報をkeyに入れる

                // dの入力で右に一つ分動かす＋右に行きすぎないようにするための条件
                //一つにまとめると長すぎてわかりにくいので縦と横、ブロックの数でそれぞれ分けて書いた
                if (key == 'd' && count % 2 == 0 &&
                    ((x < 160 && random == 2) || (x < 300 && random == 0) ||
                     (x < 240 && random == 1))) {  //動ブロックが横の場合
                    //動ブロック右端の右横にブロックがない場合は動ブロック全体をを右に動かせる
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
                           (x < 300)) {  //動ブロックが縦の場合
                    //全ての動ブロックの右横にブロックがない場合は動ブロックを右に動かせる
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
                    //横の場合
                    if (count % 2 == 0 &&
                        (beside >= 1 && array[beside - 1][height] == 0)) {
                        x -= 60;
                        // printf("%c\n", key); //確認用
                    }
                    //縦の場合
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

                //スペースキー入力でプログラム終了
                if (key == ' ') {
                    HgLClear(layer1);
                    CenteredText(layer1, WINDOWSIZE_W / 2 , WINDOWSIZE_H / 2,
                            "終了します\n");
                    HgSleep(3);  // 3秒後に終了
                    HgClose();
                    return 0;
                }
            }

            //下にブロックがあったらその上に積む
            for (i = 0; i <= height; i++) {
                if (count % 2 == 0) {   //横の場合
                    if (random == 0) {  //ブロックが1個の場合
                        if (array[beside][height] == 1) {
                            tate++;
                        }
                    } else if (random == 1) {  //ブロックが2個の場合
                        if (array[beside][height] == 1 ||
                            array[beside + 1][height] == 1) {
                            tate++;
                        }
                    } else if (random == 2) {  //ブロックが3個の場合
                        if (array[beside][height] == 1 ||
                            array[beside + 1][height] == 1 ||
                            array[beside + 2][height]) {
                            tate++;
                        }
                    }
                } else {  //縦の場合
                    if (array[beside][height] == 1) {
                        tate++;
                    }
                }
            }

            //ブロックを描く
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

            HgSleep(0.05);  //描画をゆっくりにする
        }

        // 8ブロック以上積み上がったら終了
        if (tate == 8) {
            break;
        }

        //配列をijの繰り返しと乱数で出た数を使ってx、yを計算してブロックの配置を記録させる
        //どこにブロックが落ちたか覚えておくための処理
        if (count % 2 == 0) {  //横の場合
            for (i = 0; i <= random; i++) {
                array[beside + i][tate] = 1;
                // printf("(%d,%d,%d)\n", beside + i, tate,
                //        array[beside + i][tate]); //確認用
            }
        } else {  //縦の場合
            for (i = 0; i <= random; i++) {
                array[beside][tate + i] = 1;
                // printf("(%d,%d,%d)\n", beside, tate + i,
                //        array[beside][tate + i]); //確認用
            }
        }

        //一列揃ったら消す
        for (i = 0; i < 8; i++) {
            if (array[0][i] + array[1][i] + array[2][i] + array[3][i] +
                    array[4][i] + array[5][i] ==
                6) {
                for (j = i; j < 8; j++) {
                    for (k = 0; k < 6; k++) {
                        array[k][j] = array[k][j + 1];
                    }
                }
                num++;
                i--;
            }
        }

        //落下したブロック（配列の中身が1になっている場所にあるブロック）をレイヤー2で表示
        HgLClear(layer2);
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 6; j++) {
                if (array[j][i] == 1) {
                    HgWBoxFill(layer2, 25 + 60 * j, 25 + 60 * i, SIZE, SIZE, 0);
                }
            }
        }

        HgWSetFont(layer2, HG_M, 15);
        HgWText(layer2, 30, 460, "消した行の数%d\n", num);
    }

    //ゲームオーバーになった時
    HgWSetFont(layer1, HG_MB, 20);              //明朝体太字で表示
    for (y = WINDOWSIZE_H; y >= 250; y += v) {  //上から文字が降りてくる
        HgLClear(layer1);
        CenteredText(layer1, WINDOWSIZE_W / 2 , y, "ゲームオーバー\n");
        HgSleep(0.01);
    }
    HgSleep(0.5);
    CenteredText(layer1, WINDOWSIZE_W / 2, 100, "キー入力でウィンドウが閉じます\n");

    //キー入力でウィンドウを閉じてプログラム終了
    HgGetChar();
    HgClose();
    return 0;
}
