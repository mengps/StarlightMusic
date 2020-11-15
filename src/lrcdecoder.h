#ifndef LRCDECODER_H
#define LRCDECODER_H

#include <string>
using std::string;

typedef struct LyricPacket {
    /** @brief 歌词*/
    string lyric;
    /** @brief 显示时间戳 单位: ms*/
    int64_t pts = 0;

    bool isEmpty() const { return lyric.empty() && pts == 0; }
} LyricPacket;

/**
 *  LrcDecoder LRC歌词文件解码器
 *  LRC文件非常小，因此该解码器被设计为:
 *  先进行一次完全解码，而后所有的遍历都在内部存储结构中进行
 *  存储结构可简单视为 => |LrcPacket|...|LrcPacket|
 */
class LrcDecoderPrivate;
class LrcDecoder
{
public:
    /**
     * @brief The SeekFlag enum 搜索标志
     * @note 建议根据duration来选择前向还是后向
     */
    enum SeekFlag {
        //前向查找：从前往后
        SeekForward = 1,
        //后向查找：从后往前
        SeekBackward
    };

    LrcDecoder();
    ~LrcDecoder();

    /**
     * @brief 开始解码
     * @param lrcFile LRC文件名
     * @return 成功返回true,否则返回false
     */
    bool decode(const string &lrcFile);

    /**
     * @brief 获取一个元数据
     * @param meta 元数据名称
     * @return 成功返回对应数据
     */
    string get(const string &meta);

    /**
     * @brief 读取一个LyricPacket
     * @return LyricPacket
     */
    LyricPacket readPacket();

    /**
     * @brief 利用给定的时间戳来寻求一个最接近的位置
     * @param timestamp 时间戳,单位ms
     * @param flag 查找标志
     * @warning O(n)
     * @return 成功返回true,否则返回false
     */
    bool seek(int64_t timestamp, SeekFlag flag = SeekForward);

    /**
     * @brief duration 获取歌词持续时间
     * @return 歌词持续时间
     */
    int64_t duration() const;

    /**
     * @brief 转储元数据
     * @param out 目标文件指针
     */
    void dumpMetadata(FILE *out);

    /**
     * @brief 转储歌词
     * @param out 目标文件指针
     */
    void dumpLyrics(FILE *out);

    /**
     * @brief 获取最后出现的错误
     * @return 描述错误的字符串
     */
    string lastError() const;

private:
    LrcDecoderPrivate *d;
};

#endif // LRCDECODER_H
