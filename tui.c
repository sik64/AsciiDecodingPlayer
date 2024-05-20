#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // Windows������ �۵��մϴ�.
#include "image_player_test.h"

// ȭ���� ����� ���õ� �ɼ� ���� '>' ǥ�ø� �߰��Ͽ� �޴��� ǥ���ϴ� �Լ�
void display_menu(int selected_option) {
    system("cls"); // ȭ���� ����ϴ�.
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

// ���õ� �ɼǿ� �ش��ϴ� �Լ��� �����ϴ� �Լ�
void execute_option(int selected_option) {
    system("cls");
    image_player_test();
}

int main() {
    int selected_option = 1; // �ʱ� ���õ� �ɼ�

    while (1) {
        display_menu(selected_option); // �޴� ǥ��

        // ����Ű �Է� �ޱ�
        int key = getch();
        if (key == 0xE0 || key == 0) { // ����Ű �Է��� �޾��� ���� �߰� ó��
            key = getch(); // ���� Ű �� �б�
            switch (key) {
            case 72: // ���� ȭ��ǥ Ű
                selected_option--;
                if (selected_option < 1) selected_option = 5;
                break;
            case 80: // �Ʒ��� ȭ��ǥ Ű
                selected_option++;
                if (selected_option > 5) selected_option = 1;
                break;
            }
        }
        else if (key == 13) { // Enter Ű �Է��� �޾��� ��
            execute_option(selected_option); // ���õ� �ɼ� ����
            break;
        }
    }

    return 0;
}
