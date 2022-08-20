#ifndef _MYFILE_H_
#define _MYFILE_H_

struct MyFile {
    MyFile(const char *f);
    ~MyFile();
    size_t filesize;
    std::string filename;
    void* ptr;
};

#endif /* _MYFILE_H_ */
