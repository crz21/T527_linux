## lcd相关启动知识
### 设备树
1. fragment@0
```
panel_dsi: panel_dsi {
    power0-supply = <&reg_display>; // 电源由reg_display提供
    status = "okay"; // 启用此设备
    enable-delay-ms = <10>; // 电源启用后的延迟时间
    reset-on-sequence = <1 10>, <0 20>, <1 100>; // 复位序列：高10ms -> 低20ms -> 高100ms
    reset-off-sequence = <0 100>; // 关闭复位序列
    backlight = <&reg_display>; // 背光控制
    dsi,flags = <1>; // 视频模式
    dsi,lanes = <1>; // 使用1条数据通道
    dsi,format = <0>; // 像素格式
    display-timings {
        native-mode = <&timing0>; // 指定默认时序
        timing0: timing0 {
            clock-frequency = <27777000>; // 像素时钟频率，约27.8MHz
            hback-porch = <45>; // 水平后沿
            hactive = <800>; // 水平有效像素
            hfront-porch = <59>; // 水平前沿
            hsync-len = <10>; // 水平同步脉冲宽度
            vback-porch = <50>; // 垂直后沿
            vactive = <480>; // 垂直有效像素
            vfront-porch = <20>; // 垂直前沿
            vsync-len = <2>; // 垂直同步脉冲宽度
        };
    };
};
```

2. fragment@1
```
target = <&dsi0>; // 只需打开不需要重新配置
__overlay__ {
    #address-cells = <1>;
    #size-cells = <0>;
    status = "okay";
};
```

3. fragment@2
```
ft5406: ts@38 {
    compatible = "edt,rpi-ft5506"; // 兼容FT5506触摸屏驱动
    reg = <0x38>; // I2C地址为0x38
    touchscreen-size-x = < 800 >; // X轴分辨率
    touchscreen-size-y = < 480 >; // Y轴分辨率
};

reg_display: reg_display@45 {
    compatible = "raspberrypi,7inch-touchscreen-panel-regulator"; // 兼容7英寸触摸屏电源调节器
    reg = <0x45>; // I2C地址为0x45
    gpio-controller; // 作为GPIO控制器
    #gpio-cells = <2>; // GPIO单元数
};
```

### kernel object
### makefile
   
## camera相关启动知识
### 设备树
### kernel object
### makefile

## v4l2-core相关知识
### 设备树
### kernel object
### makefile
