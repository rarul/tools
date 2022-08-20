#include <string>
#include <memory>
#include <functional>

#include "ssid3.h"
#include "myfile.h"
#include "myid3base.h"
#include "myid3v1.h"

MyID3V1::MyID3V1(std::shared_ptr<MyFile> file) : MyID3Base(file) {
}

void MyID3V1::Analyze(const std::function<void(const print_context_t&)> func) {
    print_context_t context {0, 10, "TXT", "body", m_file->filename.c_str()};
    func(context);
}

