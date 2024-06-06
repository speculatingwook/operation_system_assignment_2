#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 65

int main() {
    FILE *fp;
    char line[MAX_LINE_LENGTH];

    // 파일 열기
    fp = fopen("reader_log.txt", "r");
    if (fp == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // 파일 끝까지 한 줄씩 읽기
    while (fgets(line, sizeof(line), fp) != NULL) {
        // 문자열과 숫자를 저장할 변수 선언
        int number;
        char string[MAX_LINE_LENGTH];

        // 줄에서 숫자와 문자열 읽기
        if (sscanf(line, "%d %s", &number, string)) {
            // 문자열이 한 문자로만 구성되었는지 확인
            char c = string[0];
            for(int i = 1; i < 64; i++) {
                if(string[i] == '\n' || string[i] == '\0') {
                    break;
                }
                if(string[i] != c) {
                    printf("[ERR] : Race!! {expected :%c  actual: %c }\n", c, string[i]);
                    return 0;
                }
            }
        }
    }

    // 파일 닫기
    fclose(fp);
    printf("SUCCESS\n");
    return 0;
}
