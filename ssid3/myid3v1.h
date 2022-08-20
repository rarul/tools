#ifndef _MYID3V1_H_
#define _MYID3V1_H_

class MyID3V1 : public MyID3Base {
public:
    MyID3V1(std::shared_ptr<MyFile> file);
public:
    static std::shared_ptr<MyID3V1> Create(std::shared_ptr<MyFile> file);
    void Analyze(const std::function<void(const print_context_t&)>) override;
};

#endif /* _MYID3V1_H_ */

