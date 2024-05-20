#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // Windows에서만 작동합니다.
#include "image_player.h"
//#include "video_player.h"
//lib 폴더안에 ffmpeg라이브러리 include,lib,bin 폴더를 링킹 해주어야 합니다

// 화면을 지우고 선택된 옵션 옆에 '>' 표시를 추가하여 메뉴를 표시하는 함수
void display_menu(int selected_option) {
    system("cls"); // 화면을 지웁니다.
    printf("< ASCII DECODING PLAYER >\n\n");
    char* op1 = "IMAGE";
    char* op2 = "VIDEO";
    char* op3 = "IMG_TXT";
    char* op4 = "CONFIGURE";
    char* op5 = "MAKERS";
    char* op6 = "EXIT";
    char* menu[6] = { op1,op2,op3,op4,op5,op6 };
    for (int i = 1; i <= 6; i++) {
        if (i == selected_option) {
            printf("%15s ◀\n", menu[i-1]);
        }
        else {
            printf("%15s\n", menu[i-1]);
        }
    }
}

// 선택된 옵션에 해당하는 함수를 실행하는 함수
void execute_option(int selected_option) {
    }
void print_makers() {
    printf("만든사람들\n");
    printf("2020037108 문은식\n");
    printf("2020037108 이화진\n");
    printf("2020037108 신승우\n");
    printf("2020037108 양승민\n");
}
int main() {
    int selected_option = 1; // 초기 선택된 옵션
    
    while (1) {
        display_menu(selected_option); // 메뉴 표시

        // 방향키 입력 받기
        int key = _getch();
        if (key == 0xE0 || key == 0) { // 방향키 입력을 받았을 때의 추가 처리
            key = _getch(); // 실제 키 값 읽기
            switch (key) {
            case 72: // 위쪽 화살표 키
                selected_option--;
                if (selected_option < 1) selected_option = 6;
                break;
            case 80: // 아래쪽 화살표 키
                selected_option++;
                if (selected_option > 6) selected_option = 1;
                break;
            }
        }
        else if (key == 13) { // Enter 키 입력을 받았을 때
            switch (selected_option) {
            case 1:
                system("cls");
                image_player();
                break;
            case 2:
                system("cls");
                //video_player();
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                system("cls");
                print_makers();
                _getch();
                break;
            case 6:
                goto final;
                break;
            default:
                break;
            }
            continue;
        }
    }
final :
    return 0;
}
