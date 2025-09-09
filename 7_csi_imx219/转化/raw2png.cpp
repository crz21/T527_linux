#include <libraw/libraw.h>
#include <png.h>

#include <fstream>
#include <iostream>
#include <vector>

// 保存RGB数据为PNG文件
void save_png(const char* filename, int width, int height, const uint8_t* rgb_data)
{
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        std::cerr << "无法打开输出文件: " << filename << std::endl;
        return;
    }

    // 初始化PNG结构体
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
        std::cerr << "PNG结构体创建失败" << std::endl;
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(fp);
        std::cerr << "PNG信息结构体创建失败" << std::endl;
        return;
    }

    // 设置错误处理
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        std::cerr << "PNG编码错误" << std::endl;
        return;
    }

    // 初始化IO
    png_init_io(png_ptr, fp);

    // 设置PNG信息（RGB格式，8位深度）
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);

    // 准备扫描线数据（libpng要求每行数据指针的数组）
    std::vector<png_bytep> row_pointers(height);
    for (int i = 0; i < height; ++i) {
        row_pointers[i] = (png_bytep)(rgb_data + i * width * 3);
    }

    // 写入图像数据
    png_write_image(png_ptr, row_pointers.data());
    png_write_end(png_ptr, info_ptr);

    // 清理资源
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "用法: " << argv[0] << " <输入RAW文件> <输出PNG文件> <分辨率(WxH)>" << std::endl;
        std::cerr << "示例: " << argv[0] << " imx219_raw.rg10 output.png 3280x2464" << std::endl;
        return 1;
    }

    // 解析参数
    const char* input_path = argv[1];
    const char* output_path = argv[2];
    std::string resolution = argv[3];
    int width, height;
    if (sscanf(resolution.c_str(), "%dx%d", &width, &height) != 2) {
        std::cerr << "分辨率格式错误（应为WxH，如3280x2464）" << std::endl;
        return 1;
    }

    // 1. 读取RGGB 10位RAW文件
    std::ifstream raw_file(input_path, std::ios::binary);
    if (!raw_file) {
        std::cerr << "无法打开RAW文件: " << input_path << std::endl;
        return 1;
    }

    // 计算RAW文件大小（10位/像素，总字节数=宽×高×10/8）
    size_t expected_size = (width * height * 10) / 8;
    raw_file.seekg(0, std::ios::end);
    size_t actual_size = raw_file.tellg();
    raw_file.seekg(0, std::ios::beg);

    if (actual_size != expected_size) {
        std::cerr << "RAW文件大小不匹配！期望: " << expected_size << " 字节，实际: " << actual_size << " 字节"
                  << std::endl;
        return 1;
    }

    // 读取RAW数据到缓冲区
    std::vector<uint8_t> raw_data(actual_size);
    raw_file.read(reinterpret_cast<char*>(raw_data.data()), actual_size);
    raw_file.close();

    // 2. 使用libraw处理RGGB 10位RAW数据（去马赛克）
    LibRaw lr;
    lr.imgdata.params.output_bps = 8;     // 输出8位像素（PNG常用）
    // lr.imgdata.params.gammacor = 0;       // 禁用gamma校正（保持原始亮度）
    lr.imgdata.params.use_camera_wb = 1;  // 使用相机白平衡（IMX219默认值）
    lr.imgdata.params.output_color = 1;   // 输出RGB格式

    // 将RAW数据导入libraw
    int ret = lr.open_buffer(raw_data.data(), actual_size);
    if (ret != LIBRAW_SUCCESS) {
        std::cerr << "libraw打开RAW数据失败: " << libraw_strerror(ret) << std::endl;
        return 1;
    }

    // 处理RAW数据（包括去马赛克）
    ret = lr.unpack();
    if (ret != LIBRAW_SUCCESS) {
        std::cerr << "libraw解析RAW数据失败: " << libraw_strerror(ret) << std::endl;
        return 1;
    }

    // ret = lr.process();
    // if (ret != LIBRAW_SUCCESS) {
    //     std::cerr << "libraw处理RAW数据失败: " << libraw_strerror(ret) << std::endl;
    //     return 1;
    // }

    // 3. 提取处理后的RGB数据
    // LibRaw::imgdata_t& img = lr.imgdata;
    // if (img.sizes.width != width || img.sizes.height != height) {
    //     std::cerr << "处理后图像分辨率不匹配！期望: " << width << "x" << height << "，实际: " << img.sizes.width << "x"
    //               << img.sizes.height << std::endl;
    //     return 1;
    // }

    // libraw输出的RGB数据是交错存储（R、G、B依次排列）
    const uint8_t* rgb_data = (const uint8_t*)img.image.data();

    // 4. 保存为PNG
    save_png(output_path, width, height, rgb_data);
    std::cout << "转换完成，输出文件: " << output_path << std::endl;

    return 0;
}
