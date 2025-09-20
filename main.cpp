#include "task_manager.h"
#include "data_structures/include/module_ids.h"

#include "serial_setup.h"
#include "pattern_setup.h"

namespace Cloverwatch {

    [[noreturn]] void start() {

        TaskManager::Instance().init();
        serial_io_wire_startup();

        Logger<ModuleId::MAIN_THREAD>::log(ReadPtr<char>("Starting"), LogLevel::INFO);

        while (true) {
            k_sleep(K_MSEC(1000));
        }
    }


}

extern "C" {

int main(void) {
    Cloverwatch::start();
}

}