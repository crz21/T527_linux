import numpy as np
import cv2
import argparse
import os

def parse_args():
    """解析命令行参数"""
    parser = argparse.ArgumentParser(description="将10-bit RAW格式(Y10/RG10/BA10/GB10/BG10)转换为PNG/JPG等图片格式")
    parser.add_argument("--input", required=True, help="原始RAW文件路径（如input.y10、input.rg10）")
    parser.add_argument("--format", required=True, choices=["Y10", "RG10", "BA10", "GB10", "BG10"], 
                      help="原始格式类型（必须指定，如Y10、RG10）")
    parser.add_argument("--width", type=int, required=True, help="图像宽度（必须是8的倍数，64~1920）")
    parser.add_argument("--height", type=int, required=True, help="图像高度（必须是8的倍数，64~1080）")
    parser.add_argument("--output", required=True, help="输出图片路径（如output.png）")
    parser.add_argument("--endian", choices=["little", "big"], default="little", 
                      help="字节序（默认小端little，大端请指定big）")
    return parser.parse_args()

def validate_params(args):
    """验证参数合法性"""
    # 检查宽高是否为8的倍数且在范围内
    if (args.width % 8 != 0) or (args.height % 8 != 0):
        raise ValueError(f"宽高必须是8的倍数，当前width={args.width}, height={args.height}")
    if not (64 <= args.width <= 1920) or not (64 <= args.height <= 1080):
        raise ValueError(f"宽高需在64~1920（宽）和64~1080（高）范围内")
    # 检查输入文件是否存在
    if not os.path.exists(args.input):
        raise FileNotFoundError(f"输入文件不存在：{args.input}")

def convert_y10(args):
    """转换Y10格式为图片"""
    # 读取原始数据
    with open(args.input, 'rb') as f:
        data = f.read()
    
    # 根据字节序选择数据类型
    dtype = np.uint32 if args.endian == "little" else np.uint32BE
    uint32_array = np.frombuffer(data, dtype=dtype)
    
    # 提取10-bit有效数据（假设存储在高10位，低6位为填充）
    y10_values = uint32_array & 0x3FF  # 0x3FF = 1023，确保取10位
    expected_pixels = args.width * args.height
    if len(y10_values) != expected_pixels:
        raise ValueError(f"像素数不匹配：实际{len(y10_values)}个，预期{expected_pixels}个")
    # 重塑为图像尺寸并转换为8-bit
    y10_image = y10_values.reshape((args.height, args.width))
    y8_image = (y10_image * 255 // 1023).astype(np.uint8)
    
    # 保存输出
    cv2.imwrite(args.output, y8_image)
    print(f"Y10转换完成：{args.output}")

def convert_bayer(args):
    """转换Bayer格式（RG10/BA10/GB10/BG10）为图片"""
    # 读取原始数据
    with open(args.input, 'rb') as f:
        data = f.read()
    
    # 根据字节序选择数据类型
    dtype = np.uint16 if args.endian == "little" else np.uint16BE
    uint16_array = np.frombuffer(data, dtype=dtype)
    
    # 提取10-bit有效数据
    bayer10_values = (uint16_array >> 6) & 0x3FF
    bayer10_image = bayer10_values.reshape((args.height, args.width))
    
    # 转换为16-bit（OpenCV去马赛克函数要求）
    bayer16_image = (bayer10_image * 65535 // 1023).astype(np.uint16)
    
    # 映射Bayer模式到OpenCV参数
    pattern_map = {
        "RG10": cv2.COLOR_BAYER_RG2RGB,
        "BA10": cv2.COLOR_BAYER_GR2RGB,
        "GB10": cv2.COLOR_BAYER_GB2RGB,
        "BG10": cv2.COLOR_BAYER_BG2RGB
    }
    rgb16_image = cv2.cvtColor(bayer16_image, pattern_map[args.format])
    
    # 转换为8-bit并保存
    rgb8_image = (rgb16_image * 255 // 65535).astype(np.uint8)
    cv2.imwrite(args.output, rgb8_image)
    print(f"{args.format}转换完成：{args.output}")

def main():
    args = parse_args()
    try:
        validate_params(args)
        if args.format == "Y10":
            convert_y10(args)
        else:
            convert_bayer(args)
    except Exception as e:
        print(f"转换失败：{str(e)}")
        exit(1)

if __name__ == "__main__":
    main()
