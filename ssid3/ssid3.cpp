#include <string>
#include <memory>
#include <functional>
#include <typeinfo>

#include "ssid3.h"
#include "myfile.h"
#include "myid3base.h"
#include "myid3v1.h"
#include "myid3v2.h"

static bool verbose_mode = false;

static void SimplePrinter(const print_context_t& context) {
    printf("offset[%4zx]\tsize[%2zx]\tframe[%s]\tbody[%s]\n",
           context.offset, context.size,
           context.frame_name, context.frame_body);
}

static void VerbosePrinter(const print_context_t& context) {
    printf("filename[%s]\toffset[%4zx]\tsize[%2zx]\tframe[%s]\tbody[%s]\n",
           context.filename,
           context.offset, context.size,
           context.frame_name, context.frame_body);
}

template<class T>
static void MyAnalysis(std::shared_ptr<MyFile> file) {
    auto ptr = MyID3Base::Create<T>(file);
    if (ptr == nullptr) {
        return;
    }

    if (verbose_mode) {
        ptr->Analyze(VerbosePrinter);
    } else {
        printf("%s ########## %s\n", file->filename.c_str(), typeid(T).name());
        ptr->Analyze(SimplePrinter);
        printf("\n");
    }
}



static void do_file(const char *filename) {
    std::shared_ptr<MyFile> file = std::make_shared<MyFile>(filename);
    if (file->ptr != nullptr) {
        //MyAnalysis<MyID3V1>(file);
        MyAnalysis<MyID3V2>(file);
    }
}


int main(int argc, char *argv[]) {
    int i = 1;
    for(; i<argc; i++) {
        if (argv[i][0] != '-') {
            break;
        }
        switch (argv[i][1]) {
            case 'v':
                verbose_mode = true;
                break;
            default:
                break;
        }
    }

    for(; i<argc; i++) {
        do_file(argv[i]);
    }
    return 0;
}
