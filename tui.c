#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // Windows������ �۵��մϴ�.
#include "image_player.h"
//#include "video_player.h"
//lib �����ȿ� ffmpeg���̺귯�� include,lib,bin ������ ��ŷ ���־�� �մϴ�

// ȭ���� ����� ���õ� �ɼ� ���� '>' ǥ�ø� �߰��Ͽ� �޴��� ǥ���ϴ� �Լ�
void display_menu(int selected_option) {
    system("cls"); // ȭ���� ����ϴ�.
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
            printf("%15s ��\n", menu[i-1]);
        }
        else {
            printf("%15s\n", menu[i-1]);
        }
    }
}

// ���õ� �ɼǿ� �ش��ϴ� �Լ��� �����ϴ� �Լ�
void execute_option(int selected_option) {
    }
void print_makers() {
    printf("��������\n");
    printf("2020037108 ������\n");
    printf("2020037108 ��ȭ��\n");
    printf("2020037108 �Ž¿�\n");
    printf("2020037108 ��¹�\n");
}
int main() {
    int selected_option = 1; // �ʱ� ���õ� �ɼ�
    
    while (1) {
        display_menu(selected_option); // �޴� ǥ��

        // ����Ű �Է� �ޱ�
        int key = _getch();
        if (key == 0xE0 || key == 0) { // ����Ű �Է��� �޾��� ���� �߰� ó��
            key = _getch(); // ���� Ű �� �б�
            switch (key) {
            case 72: // ���� ȭ��ǥ Ű
                selected_option--;
                if (selected_option < 1) selected_option = 6;
                break;
            case 80: // �Ʒ��� ȭ��ǥ Ű
                selected_option++;
                if (selected_option > 6) selected_option = 1;
                break;
            }
        }
        else if (key == 13) { // Enter Ű �Է��� �޾��� ��
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
