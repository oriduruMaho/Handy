#include <stdio.h>

int main(){

    double num1, num2;

    printf("パスワード(数字)を入力してください。\n");
    scanf("%lf", &num1);

    if (num1 == 1.34){
        printf("パスワード(数字)を入力してください。\n");
        scanf("%lf", &num2);

        if (num2 == 5){
            printf("welcome to my world.\n");
        }

        else{
            printf("you can't join.\n");
        }
    }

    else{
        printf("you can't join.\n");
    }

    return 0;

}
