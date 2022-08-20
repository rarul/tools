#include <string>
#include <memory>
#include <functional>

#include <iconv.h>

#include "ssid3.h"
#include "myfile.h"
#include "myid3base.h"


MyID3Base::MyID3Base(std::shared_ptr<MyFile> file) : m_file(file) {
}

