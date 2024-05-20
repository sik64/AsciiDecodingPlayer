#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // Windows에서만 작동합니다.
#include "image_player_test.h"

// 화면을 지우고 선택된 옵션 옆에 '>' 표시를 추가하여 메뉴를 표시하는 함수
void display_menu(int selected_option) {
    system("cls"); // 화면을 지웁니다.
    printf("Select an option:\n");
    for (int i = 1; i <= 5; i++) {
        if (i == selected_option) {
            printf(" %d. Option %d <\n", i, i);
        }
        else {
            printf(" %d. Option %d\n", i, i);
        }
    }
}

// 선택된 옵션에 해당하는 함수를 실행하는 함수
void execute_option(int selected_option) {
    system("cls");
    image_player_test();
}

int main() {
    int selected_option = 1; // 초기 선택된 옵션

    while (1) {
        display_menu(selected_option); // 메뉴 표시

        // 방향키 입력 받기
        int key = getch();
        if (key == 0xE0 || key == 0) { // 방향키 입력을 받았을 때의 추가 처리
            key = getch(); // 실제 키 값 읽기
            switch (key) {
            case 72: // 위쪽 화살표 키
                selected_option--;
                if (selected_option < 1) selected_option = 5;
                break;
            case 80: // 아래쪽 화살표 키
                selected_option++;
                if (selected_option > 5) selected_option = 1;
                break;
            }
        }
        else if (key == 13) { // Enter 키 입력을 받았을 때
            execute_option(selected_option); // 선택된 옵션 실행
            break;
        }
    }

    return 0;
}
