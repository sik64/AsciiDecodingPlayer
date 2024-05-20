// ���� �ϼ� ����
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

#define NUM_CHARS 13 //ASCII ��Ʈ�� ����� ������ ����
const char CHARS[] = " .,-~:;=!*#$@"; // ASCII ��Ʈ�� ����� ���� �迭

//#define NW 120 // ASCII ��Ʈ�� ���� ���� // Resolution : 72/98/128/144/240/360/480/720 
#define BUFFER_SIZE (720 * 720 * 1) // ���� ���� (NW * NW * 2 ) ����

//=====================================================
// �ʱ� ���� ���� ��
int img_resolution;
int img_delay; // �̹��� ��� ������
int invert_colors_set_img = 0;
char* img_addr;

int video_resolution = 110; // Resolution : 72/98/128/144/240/360/480/720 
int frame_delay = 50; // �����Ӱ� ��� ������
int output_interval = 2; // ��� ����
int invert_colors_set_vid = 0; // 0 or 1(Ȱ��ȭ); ����
char* video_addr = "imgs/adidas3.mp4";

char* txt_save_addr;
//=====================================================

#include <conio.h> // �񵿱� �Է�
#define UP 72
#define DOWN 80
#define ESC 27
#define SPACEBAR ' '
#define ENTER '\r'

// �̹��� ������ �����ϸ� �̹����� �������ϴ� �Լ�
void resize_image(unsigned char* input_data, int input_width, int input_height, unsigned char** output_data, int* output_width, int* output_height) {
    float aspect_ratio = (float)input_width *2/ input_height;
    *output_width = video_resolution;
    *output_height = video_resolution / aspect_ratio;
    *output_data = malloc(video_resolution * (*output_height));
    stbir_resize_uint8(input_data, input_width, input_height, 0, *output_data, *output_width, *output_height, 0, 1);
}
// �̹����� ����ȭ�ϴ� �Լ�
void normalize_image(unsigned char* input_data, int width, int height) {
    // �̹����� �ּ� �� �ִ� ���� ã���ϴ�.
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
    // �̹����� ����ȭ�մϴ�.
    for (int i = 0; i < width * height; i++) {
        input_data[i] = (unsigned char)((input_data[i] - min_value) / (float)(max_value - min_value) * 255);
    }
}

//ASCII ��Ʈ ����Ʈ �Լ� // ���� ��� X
void print_ascii_image_old(unsigned char* image_data, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int index = (int)((float)image_data[i * width + j] / 255 * NUM_CHARS);
            printf("%c", CHARS[index]);
        }
        printf("\n");
    }
}

// ���۸� ����� ����Ʈ �Լ�
void print_ascii_image(unsigned char* image_data, int width, int height) {
    char buffer[BUFFER_SIZE];
    char* ptr = buffer;
    int chars_written;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int index = (int)((float)image_data[i * width + j] / 255 * NUM_CHARS);
            chars_written = sprintf(ptr, "%c", CHARS[index]); // ���� ���� �´� �ƽ�Ű �ڵ� ����
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
        printf("[ �� ]"); // 5 byte
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
    int totalBarLeng = screenWidth - 6; // �ε����� ���� 
    int barLeng = (int)round(totalBarLeng * (float)(frame) / totalFrame); // ���� �ε����� ����
    int percentage;

    printf("[");
    for (int j = 0; j < barLeng; j++) {
        printf("��");
    }
    printf("��");
    for (int j = 0; j < totalBarLeng - barLeng - 1; j++) {
        printf("-");
    }
    percentage = (int)((100 * ((float)(frame) / totalFrame)) + 0.5);
    printf("]%3d%%\n",percentage);
    fflush(stdout);
}

// ������ ���� ������ġ ����Ʈ �����
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
    // �̹����� �ȼ��� �����մϴ�.
    for (int i = 0; i < width * height; i++) {
        input_data[i] = 255 - input_data[i]; // 255���� �� �ȼ� ���� ���ϴ�.
    }
}
//===========================================================================================================
int main(int argc, char* argv[]) {
    int key; // �񵿱� �Է� Ű

    // �Է� ������ ���� ���
    const char* input_file = video_addr;
    //const char* input_file = "imgs/adidas.mp4";

    // FFmpeg ��Ʈ��ũ �ʱ�ȭ
    avformat_network_init();

    // �Է� ������ ���� ���� ���ؽ�Ʈ�� ����
    AVFormatContext* format_context = NULL;
    if (avformat_open_input(&format_context, input_file, NULL, NULL) != 0) {
        printf("Error: Couldn't open input file.\n");
        return 1;
    }

    // ���Ͽ��� ��Ʈ�� ���� ã��
    if (avformat_find_stream_info(format_context, NULL) < 0) {
        printf("Error: Couldn't find stream information.\n");
        return 1;
    }

    // ���� ��Ʈ�� �ε��� ã��
    int video_stream_index = -1;
    for (int i = 0; i < format_context->nb_streams; i++) {
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    // ���� ��Ʈ���� ���� ��� ���� ���
    if (video_stream_index == -1) {
        printf("Error: Couldn't find a video stream.\n");
        return 1;
    }

    // �ڵ� �Ķ���� ��������
    AVCodecParameters* codec_params = format_context->streams[video_stream_index]->codecpar;

    // �ڵ� ã��
    AVCodec* codec = avcodec_find_decoder(codec_params->codec_id);
    if (!codec) {
        printf("Error: Unsupported codec.\n");
        return 1;
    }

    // �ڵ� ���ؽ�Ʈ �Ҵ� �� �ʱ�ȭ
    AVCodecContext* codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        printf("Error: Failed to allocate codec context.\n");
        return 1;
    }

    // �ڵ� �Ķ���͸� �ڵ� ���ؽ�Ʈ�� ����
    if (avcodec_parameters_to_context(codec_context, codec_params) < 0) {
        printf("Error: Failed to initialize codec context.\n");
        return 1;
    }

    // �ڵ� ����
    if (avcodec_open2(codec_context, codec, NULL) < 0) {
        printf("Error: Failed to open codec.\n");
        return 1;
    }

    // ������ �� ��Ŷ �Ҵ�
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
    // ������ ��� ������ ���� ����
    int totalFrame = format_context->streams[video_stream_index]->nb_frames;
    // ������ �б� �ݺ�
    while (av_read_frame(format_context, packet) >= 0) {
        if (packet->stream_index == video_stream_index) {
            // ��Ŷ�� ���ڵ�
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
                // n��° �����Ӹ� ó�� // ��Ʈ�� �������� ����
                if (frame_number % output_interval == 0){
                //if (frame_number == 356) { // Ư�� ������ ����
                    system("cls"); // ȭ���� ����� ��� (Windows � ü�������� ����)
                    // ������ ũ�� ����
                    unsigned char* resized_img;
                    int resized_width, resized_height;
                    resize_image(frame->data[0], frame->width, frame->height, &resized_img, &resized_width, &resized_height);
                    // �̹��� ����
                    if (invert_colors_set_vid != 1 && invert_colors_set_vid != 0) invert_colors_set_vid = 0;
                    if (invert_colors_set_vid == 1) invert_colors(resized_img, video_resolution, resized_height);
                    // ������ �̹��� ����ȭ
                    normalize_image(resized_img, video_resolution, resized_height);
                    // ž �� ���
                    topBar(frame_number, totalFrame,0);
                    // �ε� �� ���
                    loadingBar(video_resolution, totalFrame, frame_number);
                    // ASCII ��Ʈ ���
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
                    // �޸� ����
                    free(resized_img);
                }

                frame_number++;
            }
        }
        av_packet_unref(packet);
        // �񵿱� ��ư �Է� // ������ ����
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
    // ���� ���ؽ�Ʈ �� �޸� ����
    avformat_close_input(&format_context);
    avcodec_free_context(&codec_context);
    av_frame_free(&frame);
    av_packet_free(&packet);

    // �ε���/�޴�/������
    // ++ �̹��� ����ȭ/ ������ȭ
    // ++ ���۸� �� / ��
    // ++ �ִ� ȭ��
    // �񵿱� �Է� ://  ESC:Quit SpaceBar:��/|| ������ok, �Ͻ�����ok, ��� , �������� �������� ���۹� ����
    // // �Ͻ����� ������ ���� // ������ ���� ����
    // ������ ���� / ������ ����
    // ��Ʈ�ѷ� ���� ������

    return 0;
}
