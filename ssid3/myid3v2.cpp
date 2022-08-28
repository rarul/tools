#include <string>
#include <cstring>
#include <memory>
#include <functional>
#include <unordered_map>

#include "ssid3.h"
#include "myfile.h"
#include "myid3base.h"
#include "myid3v2.h"
#include "myid3util.h"

typedef struct {
    const char *name_desc;
    void(*func)(char*,const char*,size_t);
} id3v2_frame_entry_t;

// for ID3v2 v2.2
static const std::unordered_map<std::string, id3v2_frame_entry_t> frame_entry_tbl_v2 = {
    {"TT2", {"title", MyID3V2::AnalyzeString}},
    {"TP1", {"artist", MyID3V2::AnalyzeString}},
    {"TAL", {"album", MyID3V2::AnalyzeString}},
    {"TYE", {"year", MyID3V2::AnalyzeString}},
    {"TCM", {"composer", MyID3V2::AnalyzeString}},
    {"TPA", {"setof", MyID3V2::AnalyzeString}},
    {"TP2", {"performer", MyID3V2::AnalyzeString}},
    {"COM", {"comment", MyID3V2::AnalyzeString}},
    {"TRK", {"track", MyID3V2::AnalyzeString}},
    {"TCO", {"ctype", MyID3V2::AnalyzeString}},
    {"TEN", {"encode", MyID3V2::AnalyzeString}},
    {"TSS", {"setting", MyID3V2::AnalyzeString}},
    {"TDA", {"date", MyID3V2::AnalyzeString}},
    {"TIM", {"time", MyID3V2::AnalyzeString}},
    {"TKE", {"music key", MyID3V2::AnalyzeString}},
    {"ULT", {"lyrics", MyID3V2::AnalyzeString}},
    // including binary data
    {"PIC", {"picture", MyID3V2::AnalyzeString}},
};

// for ID3v2 v2.3 v2.4
static std::unordered_map<std::string, id3v2_frame_entry_t> frame_entry_tbl_common = {
    {"TIT1", {"group", MyID3V2::AnalyzeString}},
    {"TIT2", {"title", MyID3V2::AnalyzeString}},
    {"TPE1", {"artist", MyID3V2::AnalyzeString}},
    {"TPE2", {"artist2", MyID3V2::AnalyzeString}},
    {"TALB", {"album", MyID3V2::AnalyzeString}},
    {"TPOS", {"set of", MyID3V2::AnalyzeString}},
    {"TYER", {"year", MyID3V2::AnalyzeString}},
    {"COMM", {"comment", MyID3V2::AnalyzeString}},
    {"TRCK", {"track", MyID3V2::AnalyzeString}},
    {"TCON", {"ctype", MyID3V2::AnalyzeString}},
    {"TCOP", {"copyright", MyID3V2::AnalyzeString}},
    {"TENC", {"encode", MyID3V2::AnalyzeString}},
    {"WXXX", {"user url", MyID3V2::AnalyzeString}},
    {"TOPE", {"o art", MyID3V2::AnalyzeString}},
    {"TCOM", {"composer", MyID3V2::AnalyzeString}},
    {"TMED", {"mtype",  MyID3V2::AnalyzeString}},
    {"TLEN", {"length",  MyID3V2::AnalyzeString}},
    {"TSSE", {"setting", MyID3V2::AnalyzeString}},
    {"TXXX", {"user def", MyID3V2::AnalyzeString}},
    {"TDRC", {"rec time", MyID3V2::AnalyzeString}},
    {"TBPM", {"bpm", MyID3V2::AnalyzeString}},
    {"USLT", {"lyrics", MyID3V2::AnalyzeString}},
    {"TEXT", {"writer", MyID3V2::AnalyzeString}},
    {"TDEN", {"enc time", MyID3V2::AnalyzeString}},
    {"TDTG", {"tag time", MyID3V2::AnalyzeString}},
    {"TPUB", {"publisher", MyID3V2::AnalyzeString}},
    {"TIME", {"time", MyID3V2::AnalyzeString}},
    {"TKEY", {"music key", MyID3V2::AnalyzeString}},
    {"TDAT", {"date", MyID3V2::AnalyzeString}},
    {"TFLT", {"file type", MyID3V2::AnalyzeString}},
    {"TSOP", {"perf sort", MyID3V2::AnalyzeString}},
    {"TOFN", {"orig filename", MyID3V2::AnalyzeString}},
    // no character decoding required
    {"PRIV", {"private", MyID3V2::AnalyzeSimpleChar}},
    {"WOAR", {"official url", MyID3V2::AnalyzeSimpleChar}},
    {"UFID", {"uniq file", MyID3V2::AnalyzeSimpleChar}},
    {"WCOM", {"cm url", MyID3V2::AnalyzeSimpleChar}},
    {"WCOP", {"copyright", MyID3V2::AnalyzeSimpleChar}},
    {"WOAF", {"official url", MyID3V2::AnalyzeSimpleChar}},
    {"WOAS", {"official url", MyID3V2::AnalyzeSimpleChar}},
    {"WPAY", {"pay url", MyID3V2::AnalyzeSimpleChar}},
    {"WORS", {"radio url", MyID3V2::AnalyzeSimpleChar}},
    {"WPUB", {"publish url", MyID3V2::AnalyzeSimpleChar}},
    // including binary data
    {"APIC", {"picture", MyID3V2::AnalyzeSimpleChar}},
    {"GEOB", {"encapsul", MyID3V2::AnalyzeString}},
    // TOC binary data.
    {"MCDI", {"music cd id", MyID3V2::AnalyzeSimpleChar}},
    // iTunes specific ?
    {"TCMP", {"iTunes", MyID3V2::AnalyzeString}},
    // unclear frame tag
    {"XIMP", {"ximp", MyID3V2::AnalyzeString}},
    // unclear frame tag
    {"YIMP", {"yimp", MyID3V2::AnalyzeString}},
};

MyID3V2::MyID3V2(std::shared_ptr<MyFile> file)
    : MyID3Base(file),
      m_id3v2_header(static_cast<id3v2_header_t*>(file->ptr)),
      m_version(0), m_header_size(0), m_total_size(0)
{
}

size_t MyID3V2::ParseSyncSafeSize(const unsigned char *size) {
    return
        ((size[0] & 0x7f) << 14) +
        ((size[1] & 0x7f) << 7) +
        ((size[2] & 0x7f) << 0);
}

size_t MyID3V2::ParseDirectSize(const unsigned char *size) {
    return (size[0] << 16) + (size[1] << 8) + (size[2] << 0);
}

size_t MyID3V2::ParseVerDependSize(const unsigned char *size) {
    if (m_version < 4) {
        return ParseDirectSize(size);
    } else {
        return ParseSyncSafeSize(size);
    }
}

size_t MyID3V2::ParseHeaderSize() {
    // If extended header is valid
    if (m_id3v2_header->flag & (1<<6)) {
        return sizeof(id3v2_header_t) + ParseVerDependSize(&m_id3v2_header->ext_size[1]);
    }
    unsigned char *p = nullptr;
    return &((id3v2_header_t*)p)->ext_size[0] - p;
}

void MyID3V2::AnalyzeSimpleChar(char *out_buf, const char *ptr, size_t size) {
    char bufwork[size+1];
    memset (bufwork, 0, sizeof(bufwork));
    memcpy (bufwork, ptr+1, size-1);
    MyID3Util::strcpy_maybe_ascii(out_buf, bufwork);
}

void MyID3V2::AnalyzeString(char *out_buf, const char *ptr, size_t size) {
    char charcode[16];
    int retlen = 0;

    char bufwork[size+1];
    memset (bufwork, 0, sizeof(bufwork));
    memcpy (bufwork, ptr+1, size-1);

    unsigned char enc_val = ptr[0];
    switch (enc_val) {
        case 0x00:
            // ISO-8859-1
            retlen = sprintf(out_buf, "{ASCII}");
            if (MyID3Util::detect_charcode(bufwork, size+1, charcode)) {
                if (strcasecmp(charcode, "ASCII") == 0) {
                    MyID3Util::strcpy_charcode(&out_buf[retlen], bufwork, size, charcode);
                } else {
                    sprintf(&out_buf[retlen], "{BROKEN}{%s]", charcode);
                    MyID3Util::strcpy_charcode(&out_buf[strlen(out_buf)], bufwork, size+1, charcode);
                }
            } else {
                strcpy(&out_buf[retlen], "{BROKEN}");
                MyID3Util::strcpy_hex(&out_buf[strlen(out_buf)], bufwork);
            }
            break;
        case 0x01:
            // UTF-16 LE with BOM
            retlen = sprintf(out_buf, "{UTF-16LE}");
            if (MyID3Util::detect_charcode(bufwork, size+1, charcode)) {
                if (strcasecmp(charcode, "UTF-16") == 0) {
                    MyID3Util::strcpy_charcode(&out_buf[retlen], bufwork, size+1, charcode);
                } else {
                    sprintf(&out_buf[retlen], "{BROKEN}{%s]", charcode);
                    MyID3Util::strcpy_charcode(&out_buf[strlen(out_buf)], bufwork, size+1, charcode);
                }
            } else {
                strcpy(&out_buf[retlen], "{BROKEN}");
                MyID3Util::strcpy_hex(&out_buf[strlen(out_buf)], bufwork);
            }
            break;
        case 0x02:
            // UTF-16 BE without BOM. allowd version >= 4
            retlen = sprintf(out_buf, "{UTF-16BE}");
            MyID3Util::strcpy_maybe_charcode(&out_buf[retlen], bufwork, size+1, "UTF-16BE");
            break;
        case 0x03:
            // UTF-8. allowd version >= 4
            retlen = sprintf(out_buf, "{UTF-8}");
            if (MyID3Util::detect_charcode(bufwork, size, charcode)) {
                if (strcasecmp(charcode, "UTF-8") == 0 || strcasecmp(charcode, "ASCII") == 0) {
                    MyID3Util::strcpy_charcode(&out_buf[retlen], bufwork, size, charcode);
                } else {
                    sprintf(&out_buf[retlen], "{BROKEN}{%s]", charcode);
                    MyID3Util::strcpy_charcode(&out_buf[strlen(out_buf)], bufwork, size+1, charcode);
                }
            } else {
                strcpy(&out_buf[retlen], "{BROKEN}");
                MyID3Util::strcpy_hex(&out_buf[strlen(out_buf)], bufwork);
            }
            break;
        default:
            sprintf(out_buf, "(unknown enc) %02x", enc_val);
            break;
    }
}

bool MyID3V2::AnalyzeHeader(const std::function<void(const print_context_t&)> func) {
    char print_buf[256];
    print_context_t context {0, 10, "HEAD", print_buf, m_file->filename.c_str()};

    if (memcmp(m_id3v2_header->identify, "ID3", 3) != 0) {
        strcpy(print_buf, "ID3v2 header not found");
        func(context);
        return false;
    }

    // check if version is valid
    {
        unsigned char version_hi = m_id3v2_header->version[0];
        unsigned char version_lo = m_id3v2_header->version[1];
        if (version_lo == 0 &&
            (version_hi == 2 || version_hi == 3 || version_hi == 4)) {
            // OK
            m_version = version_hi;
        } else {
            // NG
            sprintf(print_buf, "invalid header version[%02x%02x]", version_hi, version_lo);
            func(context);
            return false;
        }
    }

    m_header_size = ParseHeaderSize();
    // need to add header size (but not extended header size)
    m_total_size = 10 + ParseSyncSafeSize(&m_id3v2_header->size[1]);
    context.size = m_header_size;
    sprintf(print_buf, "ID3v2.%d TotalTagSize(%zx)", m_version, m_total_size);
    func(context);

    if (m_header_size >= m_file->filesize || m_total_size >= m_file->filesize) {
        return false;
    }
    return true;
}

void MyID3V2::Analyze(const std::function<void(const print_context_t&)> func) {
    if (AnalyzeHeader(func) == false) {
        // no HEAD "ID3" found. Stop analyze
        return;
    }

    char print_buf[512];
    char buftext[5];
    memset (buftext, 0, sizeof(buftext));
    print_context_t context {0, 0, buftext, print_buf, m_file->filename.c_str()};
    for (size_t thisoffset = m_header_size; thisoffset < m_total_size; ) {
        size_t frame_size = 0;
        context.offset = thisoffset;
        if (m_version == 2) {
            auto *frame = reinterpret_cast<id3v2_frame_v2_t*>((char*)m_file->ptr + context.offset);
            auto body_size = ParseVerDependSize(&frame->size[0]);
            frame_size = sizeof(*frame) + body_size;
            memcpy(buftext, frame->text, sizeof(frame->text));
            if (buftext[0] == '\0' && buftext[1] == '\0' &&
                buftext[2] == '\0' && buftext[3] == '\0' ) {
                break;
            }
            auto elem = frame_entry_tbl_v2.find(buftext);
            if (elem != frame_entry_tbl_v2.end()) {
                auto elep = elem->second;
                int retlen = 0;
#if 0
                // add summary for tag
                retlen = sprintf(print_buf, "{%s}", elep.name_desc);
#endif
                auto body_ptr = reinterpret_cast<const char*>(m_file->ptr) + context.offset + sizeof(*frame);
                elep.func (&print_buf[retlen], body_ptr, body_size);
            } else {
                strcpy(print_buf, "(unknown frame)(v2)");
            }
        } else {
            auto *frame = reinterpret_cast<id3v2_frame_common_t*>((char*)m_file->ptr + context.offset);
            auto body_size = ParseVerDependSize(&frame->size[1]);
            frame_size = sizeof(*frame) + body_size;
            memcpy(buftext, frame->text, sizeof(frame->text));
            if (buftext[0] == '\0' && buftext[1] == '\0' &&
                buftext[2] == '\0' && buftext[3] == '\0' ) {
                break;
            }
            auto elem = frame_entry_tbl_common.find(buftext);
            if (elem != frame_entry_tbl_common.end()) {
                auto elep = elem->second;
                int retlen = 0;
#if 0
                retlen = sprintf(print_buf, "{%s}", elep.name_desc);
#endif
                auto body_ptr = reinterpret_cast<const char*>(m_file->ptr) + context.offset + sizeof(*frame);
                elep.func (&print_buf[retlen], body_ptr, body_size);
            } else {
                strcpy(print_buf, "(unknown frame)(common)");
            }
        }
        context.size = frame_size;
        func(context);

        // going to next loop
        thisoffset += frame_size;
    }

}
