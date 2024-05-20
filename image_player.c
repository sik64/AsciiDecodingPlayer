#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#define NUM_CHARS 13
const char CHARS[] = " .,-~:;=!*#$@";
#define BUFFER_SIZE (720 * 720 * 1) // 고정 버퍼 (NW * NW * 2 ) 권장

int img_resolution = 220;
int invert_colors_set_img = 0;
int img_delay = 10; // 이미지 출력 딜레이
char* img_addr = "imgs/peter.jpg";

// 이미지 전용 출력 함수
void print_ascii_image(unsigned char* image_data, int new_width, int new_height) {
    for (int i = 0; i < new_height; i++) {
        for (int j = 0; j < new_width; j++) {
            int index = (int)((float)image_data[i * new_width + j] / 255 * NUM_CHARS);
            printf("%c", CHARS[index]);
        }
        Sleep(img_delay);
        printf("\n");
    }
}
// 버퍼 복붙
void print_ascii_image2(unsigned char* image_data, int width, int height) {
    char* buffer = malloc(BUFFER_SIZE);
    char* ptr = buffer;
    int chars_written;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int index = (int)((float)image_data[i * width + j] / 255 * NUM_CHARS);
            chars_written = sprintf(ptr, "%c", CHARS[index]); // 범위 내에 맞는 아스키 코드 선택
            if (chars_written < 0 || ptr - buffer + chars_written >= BUFFER_SIZE) {
                fprintf(stderr, "Buffer overflow occurred!\n");
                exit(1);
            }
            ptr += chars_written;
        }
        if (ptr - buffer + 1 >= BUFFER_SIZE) {
            fprintf(stderr, "Buffer overflow occurred!\n");
            exit(1);
        }
        *ptr++ = '\n';
    }

    fwrite(buffer, sizeof(char), ptr - buffer, stdout);
}
// 뉴 버퍼
//void print_ascii_image(unsigned char* image_data, int width, int height) {
//    int chars_per_row = width; // 이미지의 가로 길이를 그대로 사용
//    int buffer_size = (chars_per_row + 1) * height; // 각 행마다 개행 문자를 추가하기 위해 +1
//    char* buffer = malloc(buffer_size);
//
//    char* ptr = buffer;
//    int chars_written;
//
//    for (int i = 0; i < height; i++) {
//        for (int j = 0; j < chars_per_row; j++) {
//            if (j % 2 == 0) { // 홀수 번째 열의 픽셀은 무시
//                int index = (int)((float)image_data[i * width + j] / 255 * NUM_CHARS); // 이미지의 모든 픽셀을 고려
//                chars_written = sprintf(ptr, "%c", CHARS[index]); // 범위 내에 맞는 아스키 코드 선택
//                //Sleep(1);
//                if (chars_written < 0 || ptr - buffer + chars_written >= buffer_size) {
//                    fprintf(stderr, "Buffer overflow occurred!\n");
//                    exit(1);
//                }
//                ptr += chars_written;
//            }
//        }
//        if (ptr - buffer + 1 >= buffer_size) {
//            fprintf(stderr, "Buffer overflow occurred!\n");
//            exit(1);
//        }
//        *ptr++ = '\n';
//    }
//
//    fwrite(buffer, sizeof(char), ptr - buffer, stdout);
//    free(buffer);
//}

// 이미지 크기 재조정 / proto에서 복붙 / 가로 *2 하여 폰트 사이즈에 맞춤 ( 중복 2) ( 4 는 아님 )
void resize_image(unsigned char* input_data, int input_width, int input_height, unsigned char** output_data, int* output_width, int* output_height) {
    float aspect_ratio = (float)input_width * 2 / input_height;
    *output_width = img_resolution;
    *output_height = img_resolution / aspect_ratio;
    *output_data = malloc(img_resolution * (*output_height));
    stbir_resize_uint8(input_data, input_width, input_height, 0, *output_data, *output_width, *output_height, 0, 1);
}

// 복붙 중복
void normalize_image(unsigned char* input_data, int width, int height) {
    // 이미지의 최소 및 최대 값을 찾습니다.
    int min_value = 255;
    int max_value = 0;
    for (int i = 0; i < width * height; i++) {
        if (input_data[i] < min_value) {
            min_value = input_data[i];
        }
        if (input_data[i] > max_value) {
            max_value = input_data[i];
        }
    }
    // 이미지를 정규화합니다.
    for (int i = 0; i < width * height; i++) {
        input_data[i] = (unsigned char)((input_data[i] - min_value) / (float)(max_value - min_value) * 255);
    }
}
char* get_rightmost_substring(const char* str) {
    char* rightmost = strrchr(str, '/');
    if (rightmost == NULL) {
        return _strdup(str); // '/'가 없으면 전체 문자열을 복제하여 반환
    }
    else {
        return _strdup(rightmost + 1); // '/' 이후의 문자열을 복제하여 반환
    }
}
void print_eq(int num) {
    for (int i = 0; i < num; i++) {
        printf("━");
    }
}
int length_of_string(const char* str) {
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}
void topbar_img(int screen_width, char *filename) {
    int file_addr_leng = length_of_string(filename);
    int n = screen_width / 2 - file_addr_leng / 2;
    print_eq(n);
    printf("[ ");
    printf("%s", get_rightmost_substring(filename));
    printf(" ]");
    print_eq(n);
    int sum = n + 2 + file_addr_leng + 2 + n;
    if (sum % 2 == 0) print_eq(1);
    
    //printf("ASCII DECODING PLAYER");
    printf("\n");
}

int main() {
    // Load image using stb_image
    int width, height, channels;
    unsigned char* img = stbi_load(img_addr, &width, &height, &channels, 0); // 이미지 주소에서 가져오기
    if (!img) {
        printf("Failed to load image.\n");
        return -1;
    }

    // Calculate new width and height while maintaining aspect ratio
    int new_width, new_height;
    if (width > height) {
        new_width = img_resolution;
        new_height = (int)((float)height / width * img_resolution);
    }
    else {
        new_height = img_resolution;
        new_width = (int)((float)width / height * img_resolution);
    }

    // Convert image to grayscale
    unsigned char* gray_img = malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        int r = img[3 * i];
        int g = img[3 * i + 1];
        int b = img[3 * i + 2];
        gray_img[i] = (unsigned char)(0.2989 * r + 0.5870 * g + 0.1140 * b);
    }

    // Resize image using stb_image_resize
    unsigned char* resized_img = malloc(new_width * new_height);
    //stbir_resize_uint8(gray_img, width, height, 0, resized_img, new_width, new_height, 0, 1);
    resize_image(gray_img, width, height, &resized_img, &new_width, &new_height);
    // 이미지 정규화
    normalize_image(resized_img, new_width, new_height);

    topbar_img(img_resolution, img_addr);
    // Print ASCII image
    print_ascii_image(resized_img, new_width, new_height);

    // Free memory
    free(img);
    free(gray_img);
    free(resized_img);
    _getch(); // 리눅스에서 getch() 함수는 일반적으로 ncurses 라이브러리와 함께 사용됩니다.
    return 0;
}
