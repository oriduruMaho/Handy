#include <handy.h>
#include <stdio.h>

#define WINDOWSIZEW 600
#define WINDOWSIZEH 400
#define WHITE_SIZE 50
#define BLACK_SIZE 30

#define KEY_TYPE_BLACK 0
#define KEY_TYPE_WHITE 1

typedef struct key {
    char* sound_name;
} Key;

int layer1, layer2, layer3;
int sound[17];
int wx, bx;

int piano(int i) {
    int layer, x, y, w, h;
    if (i < 10) {
        layer = layer2;
        x = wx;
        y = 100;
        w = WHITE_SIZE;
        h = 200;
    } else {
        layer = layer3;
        x = bx;
        y = 200;
        w = BLACK_SIZE;
        h = 100;
    }
    HgLClear(layer2);
    HgLClear(layer3);
    HgWBoxFill(layer, x, y, w, h, 1);
    HgSoundPlay(sound[i]);
    return i;
}

int main() {
    hgevent* event;
    double x, y;
    int i, j, k, flag;
    int rec[100];

    HgOpen(WINDOWSIZEW, WINDOWSIZEH);

    layer1 = HgWAddLayer(0);
    layer2 = HgWAddLayer(0);
    layer3 = HgWAddLayer(0);

    char buf[17] = "";
    for (i = 0; i < 17; i++) {
        sprintf(buf, "%d.mp3", i);
        sound[i] = HgSoundLoad(buf);
    }

    HgLMove(layer1, 1);
    HgWSetFillColor(layer1, HG_BLACK);
    HgWSetFillColor(layer2, HG_GRAY);
    HgWSetFillColor(layer3, HG_GRAY);

    for (i = 0; i < 10; i++) {
        HgBoxFill(WHITE_SIZE * (i + 1), 100, WHITE_SIZE, 200, 1);
        if (i != 0 && i != 3 && i != 7) {
            HgWBoxFill(layer1, WHITE_SIZE * i + 35, 200, BLACK_SIZE, 100, 0);
        }
    }

    k = 0;

    HgBox(0, WINDOWSIZEH - WHITE_SIZE, 50, 50);
    HgBox(325, 325, 50, 50);
    HgBox(400, 325, 50, 50);

    HgSetEventMask(HG_MOUSE_DOWN);
    for (;;) {
        event = HgEvent();
        if (event->type != HG_MOUSE_DOWN) {
            continue;
        }
        x = event->x;
        y = event->y;

        // printf("x=%5.2f, y=%5.2f\n", x, y);

        flag = 0;

        for (i = 0; i < 10; i++) {
            bx = WHITE_SIZE * (i + 1) - BLACK_SIZE / 2;
            if ((i != 0 && i != 3 && i != 7) &&
                (50 * i + 35 < x && 50 * i + 65 > x) && (y > 200 && y < 300)) {
                if (i < 3) {
                    j = i - 1 + 10;
                } else if (i < 7) {
                    j = i - 2 + 10;
                } else {
                    j = i - 3 + 10;
                }
                rec[k] = piano(j);
                flag = 1;
                k++;
            }
        }
        for (i = 0; i < 10; i++) {
            wx = WHITE_SIZE * (i + 1);
            if (flag != 1 && (50 * (i + 1) < x && 50 * (i + 2) > x) &&
                y > 100 && y < 300) {
                rec[k] = piano(i);
                k++;
            }
        }

        if (325 < x && x < 375 && 325 < y) {
            for (i = 0; i < k; i++) {
                piano(rec[i]);
                HgSleep(0.5);
            }
        }

        if (400 < x && x < 450 && 325 < y) {
            k = 0;
        }

        if (x < 50 && y > 350) {
            break;
        }
    }

    HgClose();
    return 0;
}
