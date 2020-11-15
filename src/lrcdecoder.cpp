#include "lrcdecoder.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

static const string MetaData[7][2] = {
    {"ti", "title"},
    {"al", "album"},
    {"ar", "artist"},
    {"au", "author"},
    {"by", "creator"},
    {"re", "encoder"},
    {"ve", "encoder_version"}
};

static string findMeta(const string &tag) {
    string data;
    for (int i = 0; i < 7; ++i) {
        if (tag == MetaData[i][0]) {
            data = MetaData[i][1];
            break;
        }
    }

    return data;
}

class LrcDecoderPrivate
{
public:
    int64_t m_duration = 0;
    size_t m_currentIndex = 0;
    string m_filename;
    string m_lastError;
    string m_lrcData;
    std::map<string, string> m_metadata;
    std::map<int64_t, string> m_lyrics;
    std::map<int64_t, string>::iterator m_readIndex;

    void cleanup();

    size_t decodeHeader();
    void decodeLine(const string &line);
    std::string readLine();
};

LrcDecoder::LrcDecoder()
{
    d = new LrcDecoderPrivate;
}

LrcDecoder::~LrcDecoder()
{
    delete d;
}

bool LrcDecoder::decode(const string &lrcFile)
{
    d->cleanup();

    d->m_filename = lrcFile;

    std::ifstream fin;
    fin.open(lrcFile, std::ios_base::in);

    if (!fin.is_open()) {
        d->m_lastError = "LRC decode error: Can't open file!";
        return false;
    }

    std::stringstream buffer;
    buffer << fin.rdbuf();
    d->m_lrcData = buffer.str();
    fin.close();

    if (d->m_lrcData.empty()) {
        d->m_lastError = "LRC file is empty!";
        return false;
    }

    //先读取头部
    size_t index = d->decodeHeader();

    if (index == d->m_lrcData.length()) {
        d->m_lastError = "No lyrics text!";
        return false;
    }

    d->m_lrcData.erase(0, index);

    string line = d->readLine();
    while (!line.empty()) {
        d->decodeLine(line);
        line = d->readLine();
    }

    d->m_duration = (--d->m_lyrics.end())->first;
    d->m_readIndex = d->m_lyrics.begin();

    return true;
}

std::string LrcDecoder::get(const std::string &meta)
{
    string data;
    if (d->m_metadata.find(meta) != d->m_metadata.end())
        data = d->m_metadata[meta];

    return data;
}

LyricPacket LrcDecoder::readPacket()
{
    LyricPacket packet;
    if (d->m_readIndex != d->m_lyrics.end()) {
        packet.pts = d->m_readIndex->first;
        packet.lyric = d->m_readIndex->second;
        d->m_readIndex++;
    }

    return packet;
}

bool LrcDecoder::seek(int64_t timestamp, LrcDecoder::SeekFlag flag)
{
    if (flag == SeekFlag::SeekForward) {
        auto end = --d->m_lyrics.end();
        for (d->m_readIndex = d->m_lyrics.begin()
             ; d->m_readIndex != end; d->m_readIndex++) {
            if (d->m_readIndex->first >= timestamp) {
                return true;
            }
        }
    } else {
        for (d->m_readIndex = --d->m_lyrics.end()
             ; d->m_readIndex != d->m_lyrics.begin(); d->m_readIndex--) {
            if (d->m_readIndex->first <= timestamp) {
                return true;
            }
        }
    }

    return false;
}

int64_t LrcDecoder::duration() const
{
    return d->m_duration;
}

void LrcDecoder::dumpMetadata(FILE *out)
{
    fprintf(out, "[===Lyrics Metadata===]\n[Filename: %s]\n", d->m_filename.c_str());
    if (d->m_metadata.empty()) fprintf(out, "[No Metadata]\n");
    for (auto it: d->m_metadata) {
        fprintf(out, "[%s: %s]\n", it.first.c_str(), it.second.c_str());
    }
    fflush(out);
}

void LrcDecoder::dumpLyrics(FILE *out)
{
    fprintf(out, "[===Lyrics===]\n");
    for (auto it: d->m_lyrics) {
        fprintf(out, "[Pts: %lld]---[Lyric: %s]\n", it.first, it.second.c_str());
    }
    fflush(out);
}

std::string LrcDecoder::lastError() const
{
    return d->m_lastError;
}

void LrcDecoderPrivate::cleanup()
{
    m_currentIndex = 0;
    m_filename.clear();
    m_lastError.clear();
    m_lrcData.clear();
    m_metadata.clear();
    m_lyrics.clear();
}

size_t LrcDecoderPrivate::decodeHeader()
{
    size_t offset = 0;
    size_t length = m_lrcData.length();

    if (offset >= length) return offset;

    while(offset < length) {
        string meta, data;
        if (m_lrcData.at(offset) == '[') {
            while(++offset < length && m_lrcData.at(offset) != ':') {
                if (m_lrcData.at(offset) >= 'a' && m_lrcData.at(offset) <= 'z')
                    meta += m_lrcData.at(offset);
                else return offset - 1;
            }

            while(++offset < length && m_lrcData.at(offset) != ']') {
                data += m_lrcData.at(offset);
            }

            m_metadata[findMeta(meta)] = data;
        }

        offset++;
    }

    return offset;
}

void LrcDecoderPrivate::decodeLine(const std::string &line)
{
    size_t offset = 0;
    size_t length = line.length();

    if (offset >= length) return;

    string time;
    int64_t pts = 0;
    std::vector<int64_t> times;
    while(offset < length) {
        if (line.at(offset) == '[') {
            offset++;
        } else if (line.at(offset) >= '0' && line.at(offset) <= '9') {
            time += line.at(offset);
            offset++;
        } else if (line.at(offset) == ']') {
            //10 millisecond
            pts += stoi(time) * 10;
            times.push_back(pts);
            time.clear();
            pts = 0;
            offset++;
        } else if (line.at(offset) == ':') {
            //minute, = 60s * 1000ms
            pts += stoi(time) * 60 * 1000;
            time.clear();
            offset++;
        } else if (line.at(offset) == '.') {
            //second, = 1000 ms
            pts += stoi(time) * 1000;
            time += line.at(offset);
            time.clear();
            offset++;
        } else break;
    }

    string data;
    while(offset < length) {
        data += line.at(offset);
        offset++;
    }

    for (auto pts : times) {
        m_lyrics[pts] = data;
    }
}

string LrcDecoderPrivate::readLine()
{
    size_t length = m_lrcData.length();
    string line;
    while (m_currentIndex < length) {
        if (m_lrcData.at(m_currentIndex) == '\n') {
            m_currentIndex++;
            break;
        } else {
            line += m_lrcData.at(m_currentIndex);
            m_currentIndex++;
        }
    }

    return line;
}
