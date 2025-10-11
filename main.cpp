#include "task_manager.h"
#include "data_structures/include/module_ids.h"

#include "serial_setup.h"
#include "pattern_setup.h"

#include "CLI/include/cli.h"
#include "cli_setup.h"

namespace Cloverwatch {

    [[noreturn]] void start() {

        TaskManager::Instance().init();
        serial_io_wire_startup();
        pattern_setup();

        Objects::logger.log("Starting", ModuleId::MAIN_THREAD, LogLevel::INFO);

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