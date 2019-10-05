# StarlightMusic

  一款小而精致的音乐播放器，使用Qt / Qml + FFmpeg开发
 
  应当是能够跨平台的

---

### 安装、配置 

 - 下载 Qt [Qt官网](https://www.qt.io/)

 - 下载 FFmpeg [FFmpeg官网](https://www.ffmpeg.org/)

---

### 效果预览(GIF 12MB)

![Image text](preview/preview.gif) 

---

### 如何使用

```
   StarlightMusic是完整的音乐播放器
   
   因此，直接编译、运行即可

   如果需要，可以自由扩展功能
```

---

### 快捷键说明

 - 按键 `Tab` 打开/关闭 播放列表

 - 按键 `~` 打开/关闭 歌词页

 - 按键 `⌴` 播放/暂停 

 - 按键 `←` 后退 5%

 - 按键 `→` 快进 5%

 - 按键 `Ctrl + ←` 上一首

 - 按键 `Ctrl + →` 下一首 

---

### 改进建议

 - 在StarlightMusic中使用了两个粒子(系统)，更加炫酷但不是必要的，并且会占用3%左右的CPU资源(在我的机器上)，因此可以考虑去掉它们。

 - 在SkinManager中使用XML来存储，如果数据过多性能会下降，可以考虑使用JSON替代。

---

### 许可证

   使用 `MIT LICENSE`

---

### 开发环境

  Windows 10，Qt 5.13.0, FFmpeg 4.2
