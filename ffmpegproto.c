// 거의 완성 비디오
#include <stdio.h>
#include <stdlib.h>
#include <libavutil/imgutils.h>       // ffmpeg
#include <libavutil/mem.h>            // ffmpeg
#include <libavutil/opt.h>            // ffmpeg
#include <libavformat/avformat.h>     // ffmpeg
#include <libswscale/swscale.h>       // ffmpeg
#include <libswresample/swresample.h> // ffmpeg
#include <libavcodec/avcodec.h>       // ffmpeg
#include <windows.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // stb lib
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h" // stb lib

#define NUM_CHARS 13 //ASCII 아트에 사용할 문자의 개수
const char CHARS[] = " .,-~:;=!*#$@"; // ASCII 아트에 사용할 문자 배열

//#define NW 120 // ASCII 아트의 가로 길이 // Resolution : 72/98/128/144/240/360/480/720 
#define BUFFER_SIZE (720 * 720 * 1) // 고정 버퍼 (NW * NW * 2 ) 권장

//=====================================================
// 초기 설정 세팅 값
int img_resolution;
int img_delay; // 이미지 출력 딜레이
int invert_colors_set_img = 0;
char* img_addr;

int video_resolution = 110; // Resolution : 72/98/128/144/240/360/480/720 
int frame_delay = 50; // 프레임간 출력 딜레이
int output_interval = 2; // 출력 간격
int invert_colors_set_vid = 0; // 0 or 1(활성화); 반전
char* video_addr = "imgs/adidas3.mp4";

char* txt_save_addr;
//=====================================================

#include <conio.h> // 비동기 입력
#define UP 72
#define DOWN 80
#define ESC 27
#define SPACEBAR ' '
#define ENTER '\r'

// 이미지 비율을 유지하며 이미지를 재조정하는 함수
void resize_image(unsigned char* input_data, int input_width, int input_height, unsigned char** output_data, int* output_width, int* output_height) {
    float aspect_ratio = (float)input_width *2/ input_height;
    *output_width = video_resolution;
    *output_height = video_resolution / aspect_ratio;
    *output_data = malloc(video_resolution * (*output_height));
    stbir_resize_uint8(input_data, input_width, input_height, 0, *output_data, *output_width, *output_height, 0, 1);
}
// 이미지를 정규화하는 함수
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

//ASCII 아트 프린트 함수 // 현재 사용 X
void print_ascii_image_old(unsigned char* image_data, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int index = (int)((float)image_data[i * width + j] / 255 * NUM_CHARS);
            printf("%c", CHARS[index]);
        }
        printf("\n");
    }
}

// 버퍼를 사용한 프린트 함수
void print_ascii_image(unsigned char* image_data, int width, int height) {
    char buffer[BUFFER_SIZE];
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
void printSpace(int num) {
    for (int i = 0; i < num; i++) {
        printf(" ");
    }
}
void topBar(int frame_number,int totalFrame,int pause_state) {
    printf("Frame:%4d/%4d", frame_number, totalFrame);
    printSpace((int)video_resolution / 2 - 16 - 3); // 16 == length of "Frame:%4d/%4d"
    if (pause_state != 0 && pause_state != 1) pause_state = 0;
    if (pause_state == 0) {
        printf("[ ▶ ]"); // 5 byte
        printSpace((int)video_resolution / 2 - 21 - 1); // 21 == length of "ASCII DECODING PLAYER"
    }
    else if (pause_state == 1) {
        printf("[ || ]"); // 6 byte
        printSpace((int)video_resolution / 2 - 21 - 2); // 21 == length of "ASCII DECODING PLAYER"
    }
    printf("ASCII DECODING PLAYER");
    printf("\n");
}
void loadingBar(int screenWidth, int totalFrame, int frame) {
    int totalBarLeng = screenWidth - 6; // 로딩바의 길이 
    int barLeng = (int)round(totalBarLeng * (float)(frame) / totalFrame); // 현재 로딩바의 길이
    int percentage;

    printf("[");
    for (int j = 0; j < barLeng; j++) {
        printf("━");
    }
    printf("■");
    for (int j = 0; j < totalBarLeng - barLeng - 1; j++) {
        printf("-");
    }
    percentage = (int)((100 * ((float)(frame) / totalFrame)) + 0.5);
    printf("]%3d%%\n",percentage);
    fflush(stdout);
}

// 윈도우 전용 고정위치 프린트 덮어쓰기
//void printStringAtPosition(int y, int x, const char* str) {
//    COORD pos = { x, y };
//    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
//    SetConsoleCursorPosition(output, pos);
//    printf("%s", str);
//}

//int getFullFrame() {
//    return 0;
//}

void invert_colors(unsigned char* input_data, int width, int height) {
    // 이미지의 픽셀을 반전합니다.
    for (int i = 0; i < width * height; i++) {
        input_data[i] = 255 - input_data[i]; // 255에서 각 픽셀 값을 뺍니다.
    }
}
//===========================================================================================================
int main(int argc, char* argv[]) {
    int key; // 비동기 입력 키

    // 입력 동영상 파일 경로
    const char* input_file = video_addr;
    //const char* input_file = "imgs/adidas.mp4";

    // FFmpeg 네트워크 초기화
    avformat_network_init();

    // 입력 파일을 열고 포맷 컨텍스트를 생성
    AVFormatContext* format_context = NULL;
    if (avformat_open_input(&format_context, input_file, NULL, NULL) != 0) {
        printf("Error: Couldn't open input file.\n");
        return 1;
    }

    // 파일에서 스트림 정보 찾기
    if (avformat_find_stream_info(format_context, NULL) < 0) {
        printf("Error: Couldn't find stream information.\n");
        return 1;
    }

    // 비디오 스트림 인덱스 찾기
    int video_stream_index = -1;
    for (int i = 0; i < format_context->nb_streams; i++) {
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    // 비디오 스트림이 없는 경우 오류 출력
    if (video_stream_index == -1) {
        printf("Error: Couldn't find a video stream.\n");
        return 1;
    }

    // 코덱 파라미터 가져오기
    AVCodecParameters* codec_params = format_context->streams[video_stream_index]->codecpar;

    // 코덱 찾기
    AVCodec* codec = avcodec_find_decoder(codec_params->codec_id);
    if (!codec) {
        printf("Error: Unsupported codec.\n");
        return 1;
    }

    // 코덱 컨텍스트 할당 및 초기화
    AVCodecContext* codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        printf("Error: Failed to allocate codec context.\n");
        return 1;
    }

    // 코덱 파라미터를 코덱 컨텍스트로 설정
    if (avcodec_parameters_to_context(codec_context, codec_params) < 0) {
        printf("Error: Failed to initialize codec context.\n");
        return 1;
    }

    // 코덱 열기
    if (avcodec_open2(codec_context, codec, NULL) < 0) {
        printf("Error: Failed to open codec.\n");
        return 1;
    }

    // 프레임 및 패킷 할당
    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        printf("Error: Failed to allocate frame.\n");
        return 1;
    }

    AVPacket* packet = av_packet_alloc();
    if (!packet) {
        printf("Error: Failed to allocate packet.\n");
        return 1;
    }

    int frame_number = 1;
    // 영상의 모든 프레임 갯수 저장
    int totalFrame = format_context->streams[video_stream_index]->nb_frames;
    // 프레임 읽기 반복
    while (av_read_frame(format_context, packet) >= 0) {
        if (packet->stream_index == video_stream_index) {
            // 패킷을 디코딩
            int response = avcodec_send_packet(codec_context, packet);
            if (response < 0) {
                printf("Error decoding packet: %s\n", av_err2str(response));
                return 1;
            }
            while (response >= 0) {
                response = avcodec_receive_frame(codec_context, frame);
                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                    break;
                }
                else if (response < 0) {
                    printf("Error decoding frame: %s\n", av_err2str(response));
                    return 1;
                }

                //frame_number++;
                // n번째 프레임만 처리 // 컨트롤 변수에서 설정
                if (frame_number % output_interval == 0){
                //if (frame_number == 356) { // 특정 프레임 선택
                    system("cls"); // 화면을 지우는 명령 (Windows 운영 체제에서만 동작)
                    // 프레임 크기 조정
                    unsigned char* resized_img;
                    int resized_width, resized_height;
                    resize_image(frame->data[0], frame->width, frame->height, &resized_img, &resized_width, &resized_height);
                    // 이미지 반전
                    if (invert_colors_set_vid != 1 && invert_colors_set_vid != 0) invert_colors_set_vid = 0;
                    if (invert_colors_set_vid == 1) invert_colors(resized_img, video_resolution, resized_height);
                    // 프레임 이미지 정규화
                    normalize_image(resized_img, video_resolution, resized_height);
                    // 탑 바 출력
                    topBar(frame_number, totalFrame,0);
                    // 로딩 바 출력
                    loadingBar(video_resolution, totalFrame, frame_number);
                    // ASCII 아트 출력
                    print_ascii_image(resized_img, video_resolution, resized_height);
                    
                    //printf("\n");
                    Sleep(frame_delay);
                    
                    //frame_number++;
                    
                    if (_kbhit()) {
                        key = _getch();
                        switch (key) {
                        case ESC:
                            system("cls");
                            goto end;

                        case SPACEBAR:
                            //printStringAtPosition(0, (int)video_resolution / 2 - 4, "[ || ]");
                            system("cls");
                            topBar(frame_number, totalFrame, 1);
                            loadingBar(video_resolution, totalFrame, frame_number);
                            print_ascii_image(resized_img, video_resolution, resized_height);
                            if (_getch() == ESC) {
                                system("cls");
                                goto end;
                            }
                            else continue;
                        default:
                            continue;
                        }
                        break;
                    }
                    // 메모리 해제
                    free(resized_img);
                }

                frame_number++;
            }
        }
        av_packet_unref(packet);
        // 비동기 버튼 입력 // 윈도우 전용
        /*if (_kbhit()) {
            key = _getch();
            switch (key) {
            case ESC:
                system("cls");
                break;
            case SPACEBAR:
                printStringAtPosition(0, (int)video_resolution / 2 - 4, "[ || ]");
                if (_getch() == ESC) {
                    system("cls");
                    break;
                }
                else continue;
            default:
                continue;
            }
            break;
        }*/
        
    } // end of while(1)
end:
    //printf("End\n");
    // break;
    _getch();
    // 포맷 컨텍스트 및 메모리 해제
    avformat_close_input(&format_context);
    avcodec_free_context(&codec_context);
    av_frame_free(&frame);
    av_packet_free(&packet);

    // 로딩바/메뉴/프레임
    // ++ 이미지 정규화/ 안정규화
    // ++ 버퍼링 전 / 후
    // ++ 최대 화질
    // 비동기 입력 ://  ESC:Quit SpaceBar:▶/|| 나가기ok, 일시정시ok, 재생 , 빨리가기 설정에서 조작법 설명
    // // 일시정지 아이콘 오류 // 프레임 갯수 오류
    // 윈도우 버전 / 리눅스 버전
    // 컨트롤러 변수 뺴내기

    return 0;
}
