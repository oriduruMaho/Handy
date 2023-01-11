#ifndef STARS_H
#define STARS_H

// ウィンドウサイズ
#define WINDOWSIZE_W 900
#define WINDOWSIZE_H 700

// 拡大の倍率
#define SCALE 3

// 季節の切り替え用の数字
#define SPRING_NUM 1
#define SUMMER_NUM 2
#define AUTUMN_NUM 3
#define WINTER_NUM 4

// それぞれの季節の星座の数
#define SPRING_STAR 28
#define SUMMER_STAR 27
#define AUTUMN_STAR 29
#define WINTER_STAR 29

// 星のデータを入れるための構造体
typedef struct Stars {
    double x;
    double y;
    int r;
    struct Stars *next;
} Stars;

// 星座線のデータを入れるための構造体
typedef struct Sline {
    double x1;
    double y1;
    double x2;
    double y2;
    struct Sline *next;
} Sline;

// 星座と星座線を季節ごとに記録するための構造体
typedef struct Season {
    char name[20];
    Stars constellation[20];
    Sline starlines[20];
} Season;

#endif
