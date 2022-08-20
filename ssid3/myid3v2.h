#ifndef _MYID3V2_H_
#define _MYID3V2_H_
#endif /* _MYID3V2_H_ */

class MyID3V2 : public MyID3Base {
public:
    MyID3V2(std::shared_ptr<MyFile> file);
public:
    static std::shared_ptr<MyID3V2> Create(std::shared_ptr<MyFile> file);
    void Analyze(const std::function<void(const print_context_t&)>) override;
};

