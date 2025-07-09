#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *error_code = NULL;
    int error_close = 0;
    char buf[100];

    memset(buf, 0, sizeof(buf));

    error_code = fopen(argv[1], "r");
    if (error_code == NULL) {
        perror("无法打开文件");  
        exit(EXIT_FAILURE);      
    }

    if (NULL != fgets(buf, sizeof(buf), error_code)) {
        printf("%s\n", buf);
    }

    error_close = fclose(error_code);
    if (error_close != 0) {
        printf("关闭文件不成功!\n");
        return error_close;
    }
    return 0;
}