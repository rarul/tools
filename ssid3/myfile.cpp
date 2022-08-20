#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <string>

#include "myfile.h"


MyFile::MyFile(const char* f) : filesize(0), filename(f), ptr(nullptr) {
    int fd = open(f, O_RDONLY);
    if (fd < 0) {
        perror(f);
        return;
    }
    struct stat sz;
    if (fstat(fd, &sz) != 0) {
        perror(f);
    } else {
        ptr = mmap(nullptr, sz.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if(ptr == MAP_FAILED) {
            perror("mmap");
            ptr = nullptr;
        } else {
            filesize = sz.st_size;
        }
    }
    close(fd);
}


MyFile::~MyFile() {
    if (ptr != nullptr) {
        munmap(ptr, filesize);
    }
}
