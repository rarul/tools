#ifndef _MYID3BASE_H_
#define _MYID3BASE_H_

class MyID3Base {
protected:
    MyID3Base(std::shared_ptr<MyFile> file);
    std::shared_ptr<MyFile> m_file;
public:
    virtual void Analyze(const std::function<void(const print_context_t&)>) = 0;
    template<class T>
    static std::shared_ptr<MyID3Base> Create(std::shared_ptr<MyFile> file) {
        if (file->filename.find(".mp3") == std::string::npos) {
            return nullptr;
        }
        std::shared_ptr<MyID3Base> retptr;
        MyID3Base *baseptr = new T(file);
        retptr.reset(baseptr);
        return retptr;
    }
};

#endif /* _MYID3BASE_H_ */
