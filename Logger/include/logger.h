//
// Created by saransh on 7/23/25.
//

#ifndef LOGGER_H
#define LOGGER_H

#include "../../build/zephyr/misc/generated/syscalls_links/include_zephyr/logging/log_instance.h"
#include "../../data_structures/include/c_queue.h"

#include "../../data_structures/include/ptr_types.h"
#include "../../data_structures/include/c_types.h"
#include "../../data_structures/include/c_string.h"
#include "../../data_structures/include/c_heap_string.h"
#include "../../data_structures/include/module_ids.h"
#include "../../data_structures/include/c_mutex.h"
#include "../../data_structures/include/circular_count.h"

#include <zephyr/kernel.h>

#include "../../build/zephyr/misc/generated/syscalls_links/include_zephyr_logging/log.h"
#include "../../data_structures/include/time.h"


namespace Cloverwatch {

    enum class LogLevel: uint8_t {
        INFO,
        DEBUG,
        WARNING,
        ERROR
    };

    struct LogEntry {
        LogLevel level = LogLevel::INFO;
        ModuleId module = ModuleId::MAX;
        const char* message = nullptr;
        Heap* source_heap = nullptr;
        Time timestamp = Time();
    };

    template <typename G, typename L>
    class Logger {

        static constexpr size_t max_entries = L::max_entries;

        CQueue_concurrent_MPMC<LogEntry, max_entries> entries;

        inline void add_entry(LogEntry entry) {
            entry.timestamp = Time::UpTime();

            if (auto ret = entries.displace(std::move(entry)); ret.has_value()) {
                auto val = ret.value();
                if (val.source_heap != nullptr) {
                    val.source_heap->free((char*)val.message);
                }
            }
        }

    public:

        template <size_t N>
        constexpr void log(const char (&message)[N], const ModuleId module, const LogLevel level = LogLevel::INFO) {
            add_entry({level, module, message, nullptr});
        }

        template <Heap* heap>
        void log_dyn(CopyStr message, const ModuleId module, const LogLevel level = LogLevel::INFO) {
            HeapString<heap> heap_str(message);
            add_entry({level, module, heap_str.data(), heap});
        }

        template <Heap* heap>
        void log_dyn(HeapString<heap> message, const ModuleId module, const LogLevel level = LogLevel::INFO) {
            add_entry({level, module, message.data(), heap});
        }

        void get_entries(WriteRef<FixedVector<LogEntry, max_entries>> log_store) {
            std::optional<LogEntry> entry;
            for (size_t i = 0; i < max_entries && (entry = entries.pop()).has_value(); i++) {
                log_store.ref.push_back(entry.value());
            }
        }

    };

}

#endif //LOGGER_H
