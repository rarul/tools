#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <functional>

#include "ssid3.h"
#include "myfile.h"
#include "myid3base.h"
#include "myid3v1.h"
#include "myid3util.h"

const size_t MyID3V1::ID3V1_FRAME_SIZE = 128;

MyID3V1::MyID3V1(std::shared_ptr<MyFile> file) : MyID3Base(file) {
}

bool MyID3V1::AnalyzeHeader(const std::function<void(const print_context_t&)> func) {
    char print_buf[64];
    print_context_t context {m_file->filesize - ID3V1_FRAME_SIZE, 3,
        "HEAD", print_buf, m_file->filename.c_str()};

    if (m_file->filesize < ID3V1_FRAME_SIZE) {
        context.offset = 0;
        sprintf (print_buf, "filesize %zd < %zd", m_file->filesize, ID3V1_FRAME_SIZE);
        func(context);
        return false;
    }

    const char *tag_pos = static_cast<const char*>(m_file->ptr) + context.offset;
    const char tag_base[] = "TAG";
    char tag_buf[sizeof(tag_base)];
    memset (tag_buf, 0, sizeof(tag_buf));
    memcpy (tag_buf, tag_pos, sizeof(tag_base)-1);
    MyID3Util::strcpy_maybe_ascii(print_buf, tag_buf);
    func(context);

    if (memcmp(tag_base, tag_pos, sizeof(tag_base)-1) != 0) {
        return false;
    }
    return true;
}

bool MyID3V1::AnalyzeEnhance(const std::function<void(const print_context_t&)> func) {
    char print_buf[64];
    size_t search_offset = 227 + ID3V1_FRAME_SIZE;
    print_context_t context {m_file->filesize - search_offset,
        4, "ENHANCE", print_buf, m_file->filename.c_str()};
    if (m_file->filesize < search_offset) {
        return false;
    }
    const char *tag_pos = static_cast<const char*>(m_file->ptr) + context.offset;
    const char tag_base[] = "TAG+";
    char tag_buf[sizeof(tag_base)];
    memset (tag_buf, 0, sizeof(tag_buf));
    memcpy (tag_buf, tag_pos, sizeof(tag_base)-1);
    MyID3Util::strcpy_maybe_ascii(print_buf, tag_buf);
    if (memcmp(tag_base, tag_pos, sizeof(tag_base)-1) != 0) {
        return false;
    }
    func(context);
    return true;
}

void MyID3V1::AnalyzeString(const std::function<void(const print_context_t&)> func,
                            const char *frame_name, size_t offset, size_t size) {
    char print_buf[128];
    print_context_t context {m_file->filesize - ID3V1_FRAME_SIZE + offset,
        size, frame_name, print_buf, m_file->filename.c_str()};

    const char *tag_pos = static_cast<const char*>(m_file->ptr) + context.offset;
    // care for UTF-16's null terminator
    char tag_buf[size+2];
    memset (tag_buf, 0, sizeof(tag_buf));
    memcpy (tag_buf, tag_pos, size);

    char charcode[16];
    if (tag_buf[0] == '\0') {
        print_buf[0] = '\0';
    } else {
        int i = 0;
        // Last byte may be broken as a half of multiple bytes are written.
        // So try 2 more times with clearing the tail of buffer.
        for(; i<=2; i++) {
            tag_buf[size-i] = 0;
            if(MyID3Util::detect_charcode(tag_buf, size+2-i, charcode)) {
                int retlen = 0;
                if (charcode[0] != '\0' && strcasecmp(charcode,"ASCII") != 0) {
                    char broken[32] = "";
                    if (i >= 1) {
                        strcpy(broken, "{HalfByteBroken}");
                    }
                    retlen = sprintf(print_buf, "%s{%s}", broken, charcode);
                }
                MyID3Util::strcpy_charcode(&print_buf[retlen], tag_buf, size+2-i, charcode);
                break;
            }
        }
        if (i > 2) {
            // Case for cannot deteted the charcode
            strcpy(print_buf, "{HEX}");
            MyID3Util::strcpy_hex(&print_buf[strlen(print_buf)], tag_buf);
        }
    }

    func(context);
}

void MyID3V1::AnalyzeInt(const std::function<void(const print_context_t&)> func,
                         const char *frame_name, size_t offset, size_t size) {
    char print_buf[16];
    print_context_t context {m_file->filesize - ID3V1_FRAME_SIZE + offset,
        size, frame_name, print_buf, m_file->filename.c_str()};

    const unsigned char *int_pos = static_cast<const unsigned char*>(m_file->ptr) + context.offset;
    sprintf(print_buf, "%d", *int_pos);
    func(context);
}

void MyID3V1::AnalyzeTrack(const std::function<void(const print_context_t&)> func) {
    const char *tag_pos = static_cast<const char*>(m_file->ptr) + m_file->filesize
        - ID3V1_FRAME_SIZE + 125;
    if (*tag_pos == '\0') {
        AnalyzeInt(func, "track", 126, 1);
    }
}

void MyID3V1::AnalyzeGenre(const std::function<void(const print_context_t&)> func) {
    char print_buf[64];
    print_context_t context {m_file->filesize - ID3V1_FRAME_SIZE + 127,
        1, "Genre", print_buf, m_file->filename.c_str()};
    unsigned char genre_code = *(static_cast<const unsigned char*>(m_file->ptr) + context.offset);
    sprintf(print_buf, "{%s}%d", MyID3Util::genre_name(genre_code), genre_code);

    func(context);
}

void MyID3V1::Analyze(const std::function<void(const print_context_t&)> func) {
    if (AnalyzeHeader(func) == false) {
        // no HEAD "TAG" found. Stop analyze
        return;
    }

    typedef struct {
        const char *name;
        size_t offset;
        size_t size;
    } id3v1_frame_string_t;
    // https://id3.org/ID3v1
    std::vector<id3v1_frame_string_t> frame_string_array =  {
        {"Song", 3, 30},
        {"Artist", 33, 30},
        {"Album", 63, 30},
        {"Year", 93, 4},
        {"Comment", 97, 30},
    };
    for (auto elem : frame_string_array) {
        AnalyzeString(func, elem.name, elem.offset, elem.size);
    }

    AnalyzeTrack(func);

    AnalyzeGenre(func);

    // ID3v1 Enhanced tag is rare. So printout only if TAG+ found.
    AnalyzeEnhance(func);
}

