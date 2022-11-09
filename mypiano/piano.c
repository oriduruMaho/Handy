#include <handy.h>
#include <stdio.h>

#define WINDOWSIZEW 600
#define WINDOWSIZEH 400
#define SIZE 50

int main() {
    hgevent *event;
    double x, y;
    int i, j;
    int layer1, layer2, layer3;
    int song;
    int sound1[10];
    int sound2[7];

    HgOpen(WINDOWSIZEW, WINDOWSIZEH);

    layer1 = HgWAddLayer(0);
    layer2 = HgWAddLayer(0);
    layer3 = HgWAddLayer(0);

    song = HgSoundLoad("noraneko.mp3");

    sound1[0] = HgSoundLoad("w0.mp3");
    sound1[1] = HgSoundLoad("w1.mp3");
    sound1[2] = HgSoundLoad("w2.mp3");
    sound1[3] = HgSoundLoad("w3.mp3");
    sound1[4] = HgSoundLoad("w4.mp3");
    sound1[5] = HgSoundLoad("w5.mp3");
    sound1[6] = HgSoundLoad("w6.mp3");
    sound1[7] = HgSoundLoad("w7.mp3");
    sound1[8] = HgSoundLoad("w8.mp3");
    sound1[9] = HgSoundLoad("w9.mp3");

    sound2[0] = HgSoundLoad("b0.mp3");
    sound2[1] = HgSoundLoad("b1.mp3");
    sound2[2] = HgSoundLoad("b2.mp3");
    sound2[3] = HgSoundLoad("b3.mp3");
    sound2[4] = HgSoundLoad("b4.mp3");
    sound2[5] = HgSoundLoad("b5.mp3");
    sound2[6] = HgSoundLoad("b6.mp3");


    HgLMove(layer1, 1);
    HgWSetFillColor(layer1, HG_BLACK);
    HgWSetFillColor(layer2, HG_GRAY);
    HgWSetFillColor(layer3, HG_GRAY);

    for (i = 0; i < 10; i++) {
        HgBoxFill(SIZE * (i + 1), 100, 50, 200, 1);
        if (i != 0 && i != 3 && i != 7) {
            HgWBoxFill(layer1, SIZE * i + 35, 200, 30, 100, 0);
        }
    }

    HgBox(0, WINDOWSIZEH - SIZE, SIZE, SIZE);

    HgSetEventMask(HG_MOUSE_DOWN);
    for (;;) {
        event = HgEvent();
        if (event->type == HG_MOUSE_DOWN) {
            x = event->x;
            y = event->y;

            // printf("x=%5.2f, y=%5.2f\n", x, y);

            for (i = 0; i < 10; i++) {
                if (((50 * (i + 1) < x && 50 * (i + 2) > x) &&
                        (y <= 200 && y > 100)) ||
                    ((i == 0 && i == 3 && i == 7) &&
                         (50 * (i + 1) < x && 50 * (i + 2) - 15 > x) ||
                     (i == 2 && i == 6 && i == 9) &&
                         (50 * (i + 2) > x && 50 * (i + 1) + 15 < x) ||
                     (50 * (i + 1) + 15 < x && 50 * (i + 2) - 15 > x)) &&
                        y > 200 && y < 300) {
                    HgLClear(layer2);
                    HgLClear(layer3);
                    HgWBoxFill(layer2, SIZE * (i + 1), 100, 50, 200, 1);
                    HgSoundPlay(sound1[i]);
                }

                if ((i != 0 && i != 3 && i != 7) &&
                    (50 * i + 35 < x && 50 * i + 65 > x) &&
                    (y > 200 && y < 300)) {
                    HgLClear(layer2);
                    HgLClear(layer3);
                    HgWBoxFill(layer3, SIZE * i + 35, 200, 30, 100, 0);
                    if (i < 3) {
                        j = i - 1;
                    } else if (i < 7) {
                        j = i - 2;
                    } else {
                        j = i - 3;
                    }
                    HgSoundPlay(sound2[j]);
                }

                if (x < 50 && y < 50) {
                    HgSoundPlay(song);
                }
                if (x > 550 && y < 50) {
                    HgSoundStop(song);
                }
            }

            if (x < 50 && y > 350) {
                break;
            }
        }
    }

    HgClose();
    return 0;
}
