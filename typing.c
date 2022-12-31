/*
    Maho 
    typing
*/

#include <handy.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// テキストを真ん中に表示させる
#define CenteredText(layer, x, y, ...)                                 \
  {                                                                    \
    double _dp1, _dp2;                                                 \
    HgWTextSize(layer, &_dp1, &_dp2, __VA_ARGS__);                     \
    HgWText((layer), (x - _dp1 / 2.0), (y - _dp2 / 2.0), __VA_ARGS__); \
  }

int window, layer1;  //レイヤー

#define MAX_LEN 80

void trim_ln(char* str) {
  char* p;
  p = strchr(str, '\n');
  if (p != NULL) {
    *p = '\0';
  }
}

char* read_line(FILE* fp) {
  char *s, *result;
  s = malloc(sizeof(char) * (MAX_LEN + 1));
  result = fgets(s, MAX_LEN, fp);
  if (result == NULL) {
    return NULL;
  }
  trim_ln(result);
  return result;
}

//タイピング処理のテンプレート
void typing(char* string) {
  int i;                  //カウンタ変数
  int key;                //入力されたキーを判定するための変数
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

// #define DEBUG
#ifdef DEBUG

int main() {
  FILE* fp;
  char* result;
  char **subject_names, **subject_chars;
  int n;
  int i;

  fp = fopen("data.txt", "r");

  if (fp == NULL) {
    exit(EXIT_FAILURE);
  }

  fscanf(fp, "%d\n", &n);
  subject_names = malloc(sizeof(char*) * n);
  subject_chars = malloc(sizeof(char*) * n);

  for (i = 0; i < n; i++) {
    result = read_line(fp);
    printf("%s\n", result);
    subject_names[i] = result;

    result = read_line(fp);
    printf("%s\n", result);
    subject_chars[i] = result;

    read_line(fp);
  }

  fclose(fp);
}

#else

int main() {
  FILE* fp;
  char s[MAX_LEN + 1], *result;
  char **subject_names, **subject_chars;
  int n;
  int i;

  fp = fopen("data.txt", "r");

  if (fp == NULL) {
    exit(EXIT_FAILURE);
  }

  fscanf(fp, "%d\n", &n);
  subject_names = malloc(sizeof(char*) * n);
  subject_chars = malloc(sizeof(char*) * n);

  for (i = 0; i < n; i++) {
    result = read_line(fp);
    subject_names[i] = result;

    result = read_line(fp);
    subject_chars[i] = result;

    read_line(fp);
  }

  fclose(fp);

  //   for (i = 0; i < n; i++) {
  //     printf("%s: %s\n", subject_names[i], subject_chars[i]);
  //   }

  window = HgOpen(600, 400);  //ウィンドウを開く
  layer1 = HgWAddLayer(0);    //レイヤーの追加

  //文字のフォントと大きさを設定
  HgWSetFont(window, HG_GB, 50);
  HgWSetFont(layer1, HG_GB, 50);

  for (i = 0; i < n; i++) {
    CenteredText(window, 300, 300, "%s", subject_names[i]);
    typing(subject_chars[i]);
  }

  CenteredText(window, 300, 250, "おしまい");
  CenteredText(window, 300, 150, "おつかれさまでした!");

  HgGetChar();
  HgClose();
  return 0;
}

#endif
