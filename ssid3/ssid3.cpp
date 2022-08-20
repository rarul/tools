#include <string>
#include <memory>
#include <functional>
#include <typeinfo>

#include "ssid3.h"
#include "myfile.h"
#include "myid3base.h"
#include "myid3v1.h"
#include "myid3v2.h"

static void SimplePrinter(const print_context_t& context) {
    printf("offset: %zd size: %zd frame: %s body: %s\n",
           context.offset, context.size,
           context.frame_name, context.frame_body);
}

template<class T>
static void MyAnalysis(std::shared_ptr<MyFile> file) {
    auto ptr = MyID3Base::Create<T>(file);
    if (ptr == nullptr) {
        return;
    }

    printf("%s ########## %s\n", file->filename.c_str(), typeid(T).name());
    ptr->Analyze(SimplePrinter);
    printf("\n");
}



static void do_file(const char *filename) {
    std::shared_ptr<MyFile> file = std::make_shared<MyFile>(filename);
    if (file->ptr != nullptr) {
        MyAnalysis<MyID3V1>(file);
        MyAnalysis<MyID3V2>(file);
    }
}


int main(int argc, char *argv[]) {
    for(auto i=1; i<argc; i++) {
        do_file(argv[i]);
    }
    return 0;
}
