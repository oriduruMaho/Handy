#include <handy.h>

#define CenteredText(layer, x, y, ...)                                 \
  {                                                                    \
    double _dp1, _dp2;                                                 \
    HgWTextSize(layer, &_dp1, &_dp2, __VA_ARGS__);                     \
    HgWText((layer), (x - _dp1 / 2.0), (y - _dp2 / 2.0), __VA_ARGS__); \
  }

int main(void) {
  // ウィンドウを作成
  int window = HgOpen(600, 400);

  // (300, 100)に「Hello World」を真ん中寄せで描画
  CenteredText(window, 300, 100, "Hello World");

  // 書式演算子も使える
  CenteredText(window, 300, 200, "%d", 123456);

  // 何かのキーが押されるまで待機
  HgGetChar();
  HgClose();
  return 0;
}
