#ifndef _MYID3V2_H_
#define _MYID3V2_H_
#endif /* _MYID3V2_H_ */

#define ID3V2_TRUNC_BIG_FRAME_SIZE (64*1024)

#define ID3V2_HEADER_FLAG_UNSYNC_BIT (7)
#define ID3V2_HEADER_FLAG_EXT_HEADER_BIT (6)
#define ID3V2_HEADER_FLAG_EXPERIMENTAL_BIT (5)
#define ID3V2_HEADEF_FLAG_FOOTER (4)

typedef struct {
    char identify[3];
    char version[2];
    unsigned char flag;
    unsigned char size[4];
    // following is the extended header.
    unsigned char ext_size[4];
    unsigned char ext_flag_size;
    unsigned char ext_flags[16];
} id3v2_header_t;

#define ID3V2_FRAME_FLAG_GROUP_ID_BIT (6)
#define ID3V2_FRAME_FLAG_COMPRESS_BIT (3)
#define ID3V2_FRAME_FLAG_ENCRYPT_BIT  (2)
#define ID3V2_FRAME_FLAG_UNSYNCH_BIT  (1)
#define ID3V2_FRAME_FLAG_DATA_LEN_BIT (0)

// for ID3v2 v2.2
typedef struct {
    char text[3];
    unsigned char size[3];
} id3v2_frame_v2_t;

// for ID3v2 v2.3 v2.4
typedef struct {
    char text[4];
    unsigned char size[4];
    unsigned char flags[2];
} id3v2_frame_common_t;

class MyID3V2 : public MyID3Base {
public:
    MyID3V2(std::shared_ptr<MyFile> file);
public:
    static std::shared_ptr<MyID3V2> Create(std::shared_ptr<MyFile> file);
    void Analyze(const std::function<void(const print_context_t&)>) override;
    static void AnalyzeSimpleChar(char *out_buf, const char *ptr, size_t size);
    static void AnalyzeStringWithEncode(char *out_buf, const char *ptr, size_t size, unsigned char enc);
    static void AnalyzeString(char *out_buf, const char *ptr, size_t size);
    // for complex parse
    static void AnalyzeUSLT(char *out_buf, const char *ptr, size_t size);
    static void AnalyzeGEOB(char *out_buf, const char *ptr, size_t size);
private:
    bool AnalyzeHeader(const std::function<void(const print_context_t&)> func);
    static size_t ParseSyncSafeSize(const unsigned char *size);
    static size_t ParseDirectSize(const unsigned char *size);
    size_t DecodeUnsynchronizedBuf(const char *inbuf, size_t size, char *outbuf);
    size_t ParseVerDependSize(const unsigned char *size);
    size_t ParseHeaderSize();
    id3v2_header_t *m_id3v2_header;
    unsigned char   m_version;
    size_t          m_header_size;
    size_t          m_total_size;
};

