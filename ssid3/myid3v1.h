#ifndef _MYID3V1_H_
#define _MYID3V1_H_

class MyID3V1 : public MyID3Base {
public:
    static const size_t ID3V1_FRAME_SIZE;
    MyID3V1(std::shared_ptr<MyFile> file);
public:
    static std::shared_ptr<MyID3V1> Create(std::shared_ptr<MyFile> file);
    void Analyze(const std::function<void(const print_context_t&)>) override;
private:
    bool AnalyzeHeader(const std::function<void(const print_context_t&)> func);
    bool AnalyzeEnhance(const std::function<void(const print_context_t&)> func);
    void AnalyzeString(const std::function<void(const print_context_t&)> func,
                       const char *frame_name, size_t offset, size_t size);
    void AnalyzeInt(const std::function<void(const print_context_t&)> func,
                    const char *frame_name, size_t offset, size_t size);
    void AnalyzeTrack(const std::function<void(const print_context_t&)> func);
    void AnalyzeGenre(const std::function<void(const print_context_t&)> func);
};

#endif /* _MYID3V1_H_ */

