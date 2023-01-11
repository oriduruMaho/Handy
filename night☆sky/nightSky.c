/*
    254261 Nakamura Maho
    nightSky.c

    春夏秋冬それぞれの夜空を描く
    東西南北をぐるっと回るように星（丸）と星座線（線）を表示

*/

#include <handy.h>
#include <math.h>
#include <stdio.h>

#include "stars.h"  //マクロ定義と構造体

double theta;        // 回転運動の角度の変数
int updown;          // 上下移動のための変数
int layer1, layer2;  // 追加レイヤー
doubleLayer layers;  // ダブルバッファリングのためのレイヤー

// 星のx,y座標と半径の大きさをファイルから読み込んで構造体に入れる
void starsData(FILE *fp, Season *pC, int num) {
    int n, j, i;  // カウンタ変数

    for (j = 0; j < num; j++) {
        fscanf(fp, "%d,%s", &n, pC[j].name);  // 星の数と名前を読み込む
        for (i = 0; i < n; i++) {
            fscanf(fp, "%lf,%lf,%d", &pC[j].constellation[i].x,
                   &pC[j].constellation[i].y,
                   &pC[j].constellation[i].r);  // 座標と半径を読み込む
            if (i + 1 != n) {
                pC[j].constellation[i].next = &pC[j].constellation[i + 1];
            } else {
                pC[j].constellation[i].next = NULL;
            }
        }
    }
}

// 星座線の始点のx,y座標と終点のx,y座標をファイルから読み込んで構造体に入れる
void lineData(FILE *fp, Season *pC, int num) {
    int i, j, n;  // カウンタ変数
    char name[20];

    for (i = 0; i < num; i++) {
        fscanf(fp, "%d,%s", &n, name);  // 線の数と名前を読み込む
        for (j = 0; j < n; j++) {
            fscanf(fp, "%lf,%lf,%lf,%lf", &pC[i].starlines[j].x1,
                   &pC[i].starlines[j].y1, &pC[i].starlines[j].x2,
                   &pC[i].starlines[j].y2);  // 座標を読み込む
            if (j + 1 != n) {
                pC[i].starlines[j].next = &pC[i].starlines[j + 1];
            } else {
                pC[i].starlines[j].next = NULL;
            }
        }
    }
}

/*
    座標の求め方
    元の座標を(x,y)、中心の点を(a,b)とする
    x方向の移動はthetaを変更して、中心の点を中心にtheta分、円運動させる
    x=(x-a)cosθ-(y-b)sinθ+a
    y=(x-a)sinθ+(y-b)cosθ+b
*/

// 回転させたx座標を求める計算
double xcalc(double x1, double y1, double x2, double y2) {
    double x0;
    x0 = (x1 - x2) * cos(theta) - (y1 - y2) * sin(theta) + x2;
    return x0;
}

// 回転させたy座標を求める計算
double ycalc(double x1, double y1, double x2, double y2) {
    double y0;
    y0 = (x1 - x2) * sin(theta) + (y1 - y2) * cos(theta) + y2;
    return y0;
}

// 星（円）を描く
void starDraw(Season *pC, int num) {
    Stars *pstars;
    int i;                          // カウンタ変数
    double star_x, star_y, mx, my;  // 座標のための変数

    for (i = 0; i < num; i++) {
        for (pstars = pC[i].constellation; pstars != NULL;
             pstars = pstars->next) {
            // 座標の計算
            star_x =
                xcalc(pstars->x, pstars->y, WINDOWSIZE_W / 2, WINDOWSIZE_H / 2);
            star_y =
                ycalc(pstars->x, pstars->y, WINDOWSIZE_W / 2, WINDOWSIZE_H / 2);

            // アプリケーション座標をウィンドウ座標に変換
            HgTransAtoW(star_x, star_y, &mx, &my);

            // 星（円）を描く
            if (0 < mx && mx < WINDOWSIZE_W) {
                HgWCircleFill(layer1, star_x, star_y + updown, pstars->r, 0);
            }
        }
    }
}

// 星座線（線）を描く
void lineDraw(Season *pC, int num) {
    Sline *psline;
    int i;                                              // カウンタ変数
    double line_x1, line_y1, line_x2, line_y2, mx, my;  // 座標のための変数

    for (i = 0; i < num; i++) {
        for (psline = pC[i].starlines; psline != NULL; psline = psline->next) {
            // 座標の計算
            line_x1 = xcalc(psline->x1, psline->y1, WINDOWSIZE_W / 2,
                            WINDOWSIZE_H / 2);
            line_x2 = xcalc(psline->x2, psline->y2, WINDOWSIZE_W / 2,
                            WINDOWSIZE_H / 2);
            line_y1 = ycalc(psline->x1, psline->y1, WINDOWSIZE_W / 2,
                            WINDOWSIZE_H / 2);
            line_y2 = ycalc(psline->x2, psline->y2, WINDOWSIZE_W / 2,
                            WINDOWSIZE_H / 2);
            // アプリケーション座標をウィンドウ座標に変換
            HgTransAtoW(line_x1, line_y2, &mx, &my);
            if (0 < mx && mx < WINDOWSIZE_W) {
                // 線を描く
                HgWLine(layer1, line_x1, line_y1 + updown, line_x2,
                        line_y2 + updown);
            }
        }
    }
}

// 星座線のボタンと方角
void textDraw(int num) {
    double sx, sy, nx, ny, ex, ey, wx, wy;  // x,y座標
    HgCoordinateEnable(0);
    HgWSetFont(layer1, HG_M, 20);

    // 星座線を表示する文字の切り替え
    if (num % 2 == 0) {
        HgWText(layer1, 840, 650, "星座線\nを表示");
    } else {
        HgWText(layer1, 840, 650, "星座線\nを消す");
    }
    HgWBox(layer2, 840, 650, 60, 50);

    // 座標の計算
    sx = xcalc(825, 25, 825, 75);
    sy = ycalc(825, 25, 825, 75);
    nx = xcalc(825, 125, 825, 75);
    ny = ycalc(825, 125, 825, 75);
    wx = xcalc(775, 75, 825, 75);
    wy = ycalc(775, 75, 825, 75);
    ex = xcalc(875, 75, 825, 75);
    ey = ycalc(875, 75, 825, 75);

    // 方角の表示
    HgWText(layer1, sx, sy, "南");
    HgWText(layer1, nx, ny, "北");
    HgWText(layer1, ex, ey, "東");
    HgWText(layer1, wx, wy, "西");

    // 表示している画面の大体の場所を示す四角
    HgWBox(layer1, 805, 30 - updown / 5, 60, 40);
    HgWCircle(layer1, 835, 90, 80);
    HgCoordinateEnable(1);
}

// 夜空を描く
void nightDraw(Season *pC, int num) {
    hgevent *event;  // イベントを使うためのポインタ
    int key;         // キー入力の変数
    int flag = 0;    // 切り替えのための変数

    // 値の初期化
    updown = 0;
    theta = 0;

    // ホーム画面戻るようボタンの表示
    HgCoordinateEnable(0);
    HgWSetFontByName(layer2, "游教科書体 横用", 20);
    HgWText(layer2, 5, WINDOWSIZE_H - 20, "ホームに戻る");
    HgWBox(layer2, 0, 675, 125, 25);
    HgCoordinateEnable(1);

    for (;;) {
        // 入力処理
        event = HgEventNonBlocking();
        if (event != NULL) {
            if (event->type == HG_KEY_DOWN) {  // キー入力の場合
                key = event->ch;
                // 十字キーで移動、スペースでホーム画面へ、アンダーバーで星座線の表示切り替え
                if (key == HG_L_ARROW) theta -= (0.01 * M_PI);
                if (key == HG_R_ARROW) theta += (0.01 * M_PI);
                if (key == HG_U_ARROW && -450 < updown) updown -= 5;
                if (key == HG_D_ARROW && updown < 200) updown += 5;
                if (key == ' ') break;
                if (key == '_') flag++;
            } else if (event->type == HG_MOUSE_DOWN) {  // マウス入力
                // 星座線の表示切り替え
                if (840 < (event->x) && 650 < (event->y)) flag++;
                // ホーム画面へ
                if (event->x < 125 && 670 < event->y) break;
            }
        }

        layer1 = HgLSwitch(&layers);  // レイヤーの入れ替え
        HgLClear(layer1);             // レイヤ1の描画消去

        // 色の指定
        HgWSetFillColor(layer1, HG_YELLOW);
        HgWSetColor(layer1, HG_WHITE);

        if (flag % 2 == 1) lineDraw(pC, num);  // 星座線を描く
        starDraw(pC, num);                     // 星を描く
        textDraw(flag);                        // 方角などの文字を描く
        HgSleep(0.02);
    }
}

// ホーム画面の入力処理
int homeImput(void) {
    hgevent *event;      // イベントを使うためのポインタ
    double x, y;         // 座標
    int i;               // カウンタ変数
    int season_num = 0;  // 切り替えのための戻り値の変数
    int key;             // キー入力のための変数

    // 入力処理
    event = HgEvent();
    if (event->type == HG_MOUSE_DOWN) {  // マウス入力の場合
        x = event->x;
        y = event->y;
        for (i = 1; i < 5; i++) {
            if (100 + 200 * (i - 1) < x && x < i * 200 && 200 < y &&
                y < 300) {  // 季節の切り替え
                season_num = i;
                break;
            } else if (x < 100 && 660 < y) {  // 終了
                season_num = 7;
                break;
            } else if (WINDOWSIZE_W / 8 < x && x < WINDOWSIZE_W / 8 + 250 &&
                       80 < y && y < 130) {  // 概要
                season_num = 6;
            } else if (WINDOWSIZE_W / 2 < x && x < WINDOWSIZE_W / 2 + 350 &&
                       80 < y && y < 130) {  // 操作方法
                season_num = 5;
            }
        }
    } else if (event->type == HG_KEY_DOWN) {  // キー入力の場合
        key = event->ch;
        if (key == '1') season_num = SPRING_NUM;  // 春
        if (key == '2') season_num = SUMMER_NUM;  // 夏
        if (key == '3') season_num = AUTUMN_NUM;  // 秋
        if (key == '4') season_num = WINTER_NUM;  // 冬
        if (key == '5') season_num = 5;           // 操作方法
        if (key == '6') season_num = 6;           // 概要
        if (key == ' ') season_num = 7;           // 終了
    }

    HgLClear(layer2);  // layer2の描画を消去
    return season_num;
}

// ホーム画面の表示
void homeDraw(void) {
    // 全てlayer2に描く
    HgCoordinateEnable(0);
    HgWSetFontByName(layer2, "Snell Roundhand", 150);
    HgWSetColor(layer2, HG_WHITE);
    HgWText(layer2, WINDOWSIZE_W / 10, 380, "Night☆Sky");

    HgWSetFontByName(layer2, "ヒラギノ明朝 ProN", 100);
    HgWText(layer2, WINDOWSIZE_W / 5 - 50, 200, "春");
    HgWText(layer2, WINDOWSIZE_W / 5 * 2 - 50, 200, "夏");
    HgWText(layer2, WINDOWSIZE_W / 5 * 3 - 50, 200, "秋");
    HgWText(layer2, WINDOWSIZE_W / 5 * 4 - 50, 200, "冬");

    HgWSetFontByName(layer2, "游教科書体 横用", 30);
    HgWText(layer2, 20, WINDOWSIZE_H - 35, "終了");
    HgWBox(layer2, 0, 660, 100, 40);

    HgWSetFontByName(layer2, "ヒラギノ角ゴシック", 50);
    HgWText(layer2, WINDOWSIZE_W / 8, 75, "概要を表示");
    HgWText(layer2, WINDOWSIZE_W / 2, 75, "操作方法を表示");
    HgWBox(layer2, WINDOWSIZE_W / 8, 80, 250, 50);
    HgWBox(layer2, WINDOWSIZE_W / 2, 80, 350, 50);
    HgCoordinateEnable(1);
}

// 操作方法の表示の関数
void operationDraw(void) {
    // 全てlayer2に描く
    HgCoordinateEnable(0);
    HgWSetFontByName(layer2, "游教科書体", 25);
    HgWText(layer2, WINDOWSIZE_W / 8, 400,
            "マウス入力\n  ホーム画面\n・春夏秋冬：それぞれの季節の星座を表示\n"
            "・概要を表示：このプログラムの概要を表示\n"
            "・操作方法を表示：操作説明（現在の画面）を表示\n"
            "・終了：プログラム終了\n  星座画面\n"
            "・星座線を表示：星座線を表示する\n・星座線を消す：星座線を消す\n・"
            "ホームに戻る：ホーム画面を表示する");
    HgWText(layer2, WINDOWSIZE_W / 8, 60,
            "キー入力\n  ホーム画面\n・1：春の星座を表示    "
            "・2：夏の星座を表示\n・3：秋の星座を表示    "
            "・4：冬の星座を表示\n・5：操作説明（現在の画面）を表示\n・6：概要"
            "を表示\n・スペース：プログラム終了\n  "
            "星座画面\n・_(アンダーバー)：星座線の表示切り替え\n・→：左回転   "
            "・←：右回転\n・↑↓：上下移動\n・スペース：ホーム画面を表示\n");
    HgWText(layer2, WINDOWSIZE_W / 2, 25, "キー入力でホーム画面に戻る");
    HgCoordinateEnable(1);
    HgGetChar();  // キー入力があるまで待機
}

// 概要の表示の関数
void overviewDraw(void) {
    // 全てlayer2に描く
    HgCoordinateEnable(0);
    HgWSetFontByName(layer2, "游教科書体", 30);
    HgWText(
        layer2, WINDOWSIZE_W / 12, 200,
        "春夏秋冬それぞれの季節の日の午前0時に\n日本で見えるであろう星座を描"
        "いています。\n春：春分の日（3月20日頃）  夏：夏至の日（6月20日頃）"
        "\n秋：秋分の日（9月20日頃）  冬：冬至の日（12月20日頃）\nBGM,"
        "背景共に自作です。\nスマホのアプリで星座を見ていた時に\n「これHandy "
        "Graphicでも作れるんじゃないかな」\nと思って作ってみました。\n制作期間"
        "は一ヶ月ほど。\n夜空の旅を楽しんでいただけたら幸いです。");
    HgWText(layer2, WINDOWSIZE_W / 2, 25, "キー入力でホーム画面に戻る");
    HgWText(layer2, WINDOWSIZE_W / 8, 50, "製作者：中村真穂");
    HgCoordinateEnable(1);
    HgGetChar();  // キー入力があるまで待機
}

int main(void) {
    FILE *fp1, *fp2;  // ファイル読み込みのためのポインタ
    int i;            // カウンタ変数
    int pic;          // イラストを読み込むための変数
    char bufs[25], bufl[25];  // ファイルの名前を入れる配列
    int flag;                 // 切り替えのための変数
    int sound;                // 音楽を流すための変数

    // それぞれの季節の構造体を宣言
    Season spring[SPRING_STAR];
    Season summer[SUMMER_STAR];
    Season autumn[AUTUMN_STAR];
    Season winter[WINTER_STAR];

    // テキストファイルから座標を読み込む
    for (i = 1; i < 5; i++) {
        // ファイルの名前を入れる
        sprintf(bufs, "constellations_%d.txt", i);
        sprintf(bufl, "constellations_1%d.txt", i);

        // ファイルを開く
        fp1 = fopen(bufs, "r");
        fp2 = fopen(bufl, "r");

        if (fp1 == NULL || fp2 == NULL) {
            printf("ファイルオープンエラー\n");
            return 1;
        }

        // データを読み込む
        switch (i) {
            case SPRING_NUM:  // 春のデータ
                starsData(fp1, spring, SPRING_STAR);
                lineData(fp2, spring, SPRING_STAR);
                break;
            case SUMMER_NUM:  // 夏のデータ
                starsData(fp1, summer, SUMMER_STAR);
                lineData(fp2, summer, SUMMER_STAR);
                break;
            case AUTUMN_NUM:  // 秋のデータ
                starsData(fp1, autumn, AUTUMN_STAR);
                lineData(fp2, autumn, AUTUMN_STAR);
                break;
            case WINTER_NUM:  // 冬のデータ
                starsData(fp1, winter, WINTER_STAR);
                lineData(fp2, winter, WINTER_STAR);
                break;
        }

        // ファイルを閉じる
        fclose(fp1);
        fclose(fp2);
    }

    HgOpen(WINDOWSIZE_W, WINDOWSIZE_H);

    layer1 = HgWAddLayer(0);
    layer2 = HgWAddLayer(0);
    layers = HgWAddDoubleLayer(0);

    // ウィンドウ座標とアプリケーション座標の変換をしてスケールを大きくする（3倍）
    HgCoordinate(-1000, -400, SCALE);
    // イベントマスクの設定
    HgSetEventMask(HG_KEY_DOWN | HG_MOUSE_DOWN);

    // 背景のイラストを読み込む
    pic = HgImageLoad("night.PNG");

    // BGMを流す（繰り返す）
    sound = HgSoundLoad("constellation.mp3");
    HgSoundVolume(sound, 1, 1);
    HgSoundPlay(sound);

    do {
        flag = 0;   // 初期化
        HgClear();  // 全画面消去

        // 背景を描く
        HgCoordinateEnable(0);
        HgImagePut(WINDOWSIZE_W / 2, WINDOWSIZE_H / 2, pic, 1, 0);
        HgCoordinateEnable(1);

        // ホーム画面を描く
        HgSetTitle("night☆sky");
        homeDraw();
        flag = homeImput();

        // それぞれの画面を表示する
        switch (flag) {
            case SPRING_NUM:  // 春
                HgSetTitle("春");
                nightDraw(spring, SPRING_STAR);
                break;
            case SUMMER_NUM:  // 夏
                HgSetTitle("夏");
                nightDraw(summer, SUMMER_STAR);
                break;
            case AUTUMN_NUM:  // 秋
                HgSetTitle("秋");
                nightDraw(autumn, AUTUMN_STAR);
                break;
            case WINTER_NUM:  // 冬
                HgSetTitle("冬");
                nightDraw(winter, WINTER_STAR);
                break;
            case 5:  // 操作方法を表示
                HgSetTitle("操作方法");
                operationDraw();
                break;
            case 6:  // 概要を表示
                HgSetTitle("概要");
                overviewDraw();
                break;
        }
    } while (flag != 7);  // 7だったら終了

    HgSoundStop(sound);
    HgImageUnload(pic);
    HgSoundUnload(sound);
    HgClose();

    return 0;
}
