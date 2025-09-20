//
// Created by saransh on 9/13/25.
//

#include "pattern_stats.h"
#include "../../data_structures/include/c_linked_deque.h"

#include "task_manager.h"

namespace Cloverwatch::Pattern {

    template <typename G, typename L>
    StatResult StatTracker<G, L>::add_packet(ReadVector<Byte> packet) {

        MutexLock lock(packet_mtx);

        uint16_t packet_size = packet.size();
        uint16_t bytes_written = 0;

        auto cleanup = [&]() {
            Byte byte;
            for (int i=0; i < bytes_written; i++)
                packet_queue.pop_back(byte);
        };

        Byte msb = packet_size >> 8;
        Byte lsb = packet_size & 0xff;

        bool res = packet_queue.push_front(msb);
        if (!res) {
            cleanup();
            return StatResult::INSUFFICIENT_CAPACITY;
        }

        res = packet_queue.push_front(lsb);
        if (!res) {
            cleanup();
            return StatResult::INSUFFICIENT_CAPACITY;
        }

        for (Byte byte: packet) {
            res = packet_queue.push_back(byte);
            if (!res) {
                cleanup();
                return StatResult::INSUFFICIENT_CAPACITY;
            }
        }

        return StatResult::OK;
    }

    template <typename G, typename L>
    StatResult StatTracker<G, L>::add_pattern(ReadBufferPtr<char> name, ReadVector<Byte> notation) {

        MutexLock lock(pattern_mtx);

        if (patterns.size() == L::max_patterns) return StatResult::INSUFFICIENT_CAPACITY;

        auto pattern_op = compile_pattern(notation);

        if (pattern_op.has_value()) {
            PatternInfo pattern_info;
            pattern_info.name = name.ptr;
            pattern_info.pattern = pattern_op.value();

            patterns.push_back(pattern_info);
        }
        else {
            return StatResult::INVALID_NOTATION;
        }

        return StatResult::OK;
    }

    template <typename G, typename L>
    void StatTracker<G, L>::packet_process_func(void* instance) {

        auto this_ptr = static_cast<StatTracker<G, L> *>(instance);
        auto &state = this_ptr->state;
        auto &queue = this_ptr->packet_queue;

        {
            MutexLock mtx(this_ptr->packet_mtx);

            bool res = true;

            if (state.step != PacketStep::PACKET) {
                Byte byte;
                res = queue.pop_front(byte);
                if (!res) return;

                state.expected_bytes = byte;

                if (state.step == PacketStep::MSB)
                    state.expected_bytes <<= 8;

                ++state.step;
            }

            size_t bytes_read = std::min(state.expected_bytes, L::max_byte_processed);

            for (size_t i=0; i < bytes_read; i++) {
                Byte byte;
                res = queue.pop_front(byte);
                if (!res) return;

                state.packet_buffer.push_back(byte);
            }
        }

        if (state.packet_buffer.size() == state.expected_bytes) {

            MutexLock mtx(this_ptr->packet_mtx);

            for (auto &pattern: this_ptr->patterns) {
                if (match_pattern(state.packet_buffer, pattern)) {
                    ++pattern.num_instances;

                    if (pattern.timestamps.size() == L::history_length)
                        pattern.timestamps.pop();

                    pattern.timestamps.push(Time::UpTime().milliseconds());
                }
            }

            state.packet_buffer.clear();
            state.step = PacketStep::MSB;
        }
    }

    template <typename G, typename L>
    void StatTracker<G, L>::start_process() {
        TaskManager::Task task = {
            .func = packet_process_func,
            .args = this,
            .priority = TaskManager::Priority::MEDIUM
        };

        TaskManager::Instance().forever_work_item(task, K_MSEC(1));
    }

}