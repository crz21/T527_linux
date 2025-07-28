#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void write_csv_header(FILE *file) { fprintf(file, "num,ax,ay,az,gx,gy,gz,pitch_f32,roll_f32\n"); }

void write_data_to_csv(FILE *file, int16_t num, float a_x, float a_y, float a_z, float g_x, float g_y, float g_z,
                       float f_pitch, float f_roll)
{
    // 使用 fprintf 写入逗号分隔的值
    fprintf(file, "%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", num, a_x, a_y, a_z, g_x, g_y, g_z, f_pitch, f_roll);
}

int main(int argc, char **argv)
{
    FILE *csv_file = NULL;
    int error_close = 0;
    char buf[100];
    static int16_t imu_cnt = 0;

    memset(buf, 0, sizeof(buf));

    csv_file = fopen("test.csv", "w");
    if (csv_file == NULL) {
        perror("无法打开文件");
        exit(EXIT_FAILURE);
    }
    write_csv_header(csv_file);
    // if (NULL != fgets(buf, sizeof(buf), csv_file)) {
    //     printf("%s\n", buf);
    // }
    for (imu_cnt = 0; imu_cnt < 10; imu_cnt++) {
        write_data_to_csv(csv_file, imu_cnt, 1, 1, 2, 2, 3, 3, 4, 4);
    }

    fclose(csv_file);
    // printf("CSV文件创建成功: students.csv\n");

    // error_close = fclose(csv_file);
    // if (error_close != 0) {
    //     printf("关闭文件不成功!\n");
    //     return error_close;
    // }
    return 0;
}