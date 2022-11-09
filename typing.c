/*
    Maho
    typing
*/

#include <handy.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// テキストを真ん中に表示させる
#define CenteredText(layer, x, y, ...)                                     \
    {                                                                      \
        double _dp1, _dp2;                                                 \
        HgWTextSize(layer, &_dp1, &_dp2, __VA_ARGS__);                     \
        HgWText((layer), (x - _dp1 / 2.0), (y - _dp2 / 2.0), __VA_ARGS__); \
    }

int window, layer1;  //レイヤー

#define MAX_LEN 80

void trim_ln(char *str) {
    char *p;
    p = strchr(str, '\n');
    if (p != NULL) {
        *p = '\0';
    }
}

//タイピング処理のテンプレート
void typing(char string[100]) {
    int i;    //カウンタ変数
    int key;  //入力されたキーを判定するための変数
    char word[100] = {""};  //入力されたキーを覚えておくための配列

    //入力する文字を表示
    CenteredText(window, 300, 200, "%s", string);

    //入力と判定と表示
    for (i = 0; i < strlen(string);) {
        key = HgGetChar();
        if (string[i] == key) {
            HgLClear(layer1);
            word[i] = key;
            CenteredText(layer1, 300, 100, "%s", word);
            i++;
        }
    }

    // 0.5秒後に表示されているものを全て消す
    HgSleep(0.5);
    HgClear();
}

#define DEBUG
#ifdef DEBUG

int main() {
    FILE *fp;
    char s[MAX_LEN + 1], *result;
    char **subject_names, **subject_chars;
    int n;
    int i;

    fp = fopen("data.txt", "r");

    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%d", &n);
    subject_names = malloc(sizeof(char *) * n);
    subject_chars = malloc(sizeof(char *) * n);

    for (i = 0; i < n; i++) {
        result = fgets(s, MAX_LEN, fp);
        if (result == NULL) {
            fclose(fp);
            break;
        }

        trim_ln(result);

        printf("%s\n", result);
        subject_names[i] = result;

        result = fgets(s, MAX_LEN, fp);
        if (result == NULL) {
            fclose(fp);
            break;
        }

        subject_chars[i] = result;
    }   
}

#else

int main() {
    FILE *fp;
    char s[MAX_LEN + 1], *result;
    char **subject_names, **subject_chars;
    int n;
    int i;

    fp = fopen("data.txt", "r");

    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%d", &n);
    subject_names = malloc(sizeof(char *) * n);
    subject_chars = malloc(sizeof(char *) * n);

    for (i = 0; i < n; i++) {
        result = fgets(s, MAX_LEN, fp);
        if (result == NULL) {
            fclose(fp);
            break;
        }
        printf("%s\n", result);
        subject_names[i] = result;

        result = fgets(s, MAX_LEN, fp);
        if (result == NULL) {
            fclose(fp);
            break;
        }

        subject_chars[i] = result;
    }

    // for(i = 0; i < n; i++) {
    //     printf("%s: %s\n", subject_names[i], subject_chars[i]);
    // }


    window = HgOpen(600, 400);  //ウィンドウを開く
    layer1 = HgWAddLayer(0);    //レイヤーの追加

    //文字のフォントと大きさを設定
    HgWSetFont(window, HG_GB, 50);
    HgWSetFont(layer1, HG_GB, 50);

    CenteredText(window, 300, 300, "麻婆豆腐!");
    typing("ma-bo-douhu!");

    CenteredText(window, 300, 300, "まぜそば");
    typing("mazesoba");

    CenteredText(window, 300, 300, "スイートポテト");
    typing("sui-topoteto");

    CenteredText(window, 300, 300, "杏仁豆腐");
    typing("annnindouhu");

    CenteredText(window, 300, 300, "わらび餅");
    typing("warabimoti");

    CenteredText(window, 300, 300, "大学芋");
    typing("daigakuimo");

    CenteredText(window, 300, 300, "ぎょうざ");
    typing("gyouza");

    CenteredText(window, 300, 300, "豚汁");
    typing("butaziru");

    CenteredText(window, 300, 300, "きなこ");
    typing("kinako");

    CenteredText(window, 300, 300, "汁なしけんちん汁");
    typing("sirunasikentinjiru");

    CenteredText(window, 300, 300, "からあげ");
    typing("karaage");

    CenteredText(window, 300, 300, "ヤンニョムチキン");
    typing("yannnyomutikinn");

    CenteredText(window, 300, 300, "タルから");
    typing("tarukara");

    CenteredText(window, 300, 300, "ゆーりんちー");
    typing("yu-rinti-");

    CenteredText(window, 300, 300, "プレーン");
    typing("pure-nn");

    CenteredText(window, 300, 300, "タイムセール");
    typing("taimuse-ru");

    CenteredText(window, 300, 300, "しょーろんぽー");
    typing("syo-ronpo-");

    CenteredText(window, 300, 250, "おしまい");
    CenteredText(window, 300, 150, "おつかれさまでした!");

    HgGetChar();
    HgClose();
    return 0;
}

#endif
