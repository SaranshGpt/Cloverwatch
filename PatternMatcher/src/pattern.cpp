//
// Created by saransh on 9/5/25.
//

#include "pattern.h"

#include <cstdint>

#include "../../Logger/include/logger.h"


namespace Cloverwatch::Pattern {

    DEFINE_HEAP(pattern_heap, PatternConfig::heap_size);

    static uint16_t get_uint16(ReadVector<Byte> data, size_t &offset) {

        uint16_t value = data[offset];
        value<<=8;
        value |= data[offset + 1];

        offset+=2;

        return value;
    }

    static bool evaluate_condition(ReadVector<Byte> data, const Condition& condition) {

        size_t start_range = condition.start_range;
        size_t end_range = condition.end_range;

        if (end_range > data.len()) return false;
        if (start_range > end_range) return false;

        size_t size = end_range - start_range;

        auto match_fixed = [&](const size_t offset = 0)  {
            size_t ind = 0;
            for (; ind<size && data[ind + start_range] == condition.vals[ind + offset]; ind++);
            return size == ind;
        };

        auto lexical_between = [&]() {
            size_t ind;
            for (ind = 0; ind < size && data[ind + start_range] == condition.vals[ind]; ind++);

            if (ind < size && data[ind + start_range] < condition.vals[ind]) return false;

            for (ind = 0; ind < size && data[ind + start_range] == condition.vals[ind + size]; ind++);

            return ind == size || data[ind + start_range] <= condition.vals[ind + size];
        };

        auto match_selection = [&]() {
            for (size_t offset = 0; offset < condition.vals.len(); offset += size) {
                if (match_fixed(offset)) return true;
            }

            return false;
        };

        switch (condition.type) {
            case ConditionType::FIXED:
                return match_fixed();
            case ConditionType::RANGE:
                return lexical_between();
            case ConditionType::SELECTION:
                return match_selection();
            case ConditionType::NFIXED:
                return !match_selection();
            case  ConditionType::NRANGE:
                return !lexical_between();
            case ConditionType::NSELECTION:
                return !match_selection();
            default:
                return false;
        }

    }

    bool match_pattern(ReadVector<Byte> data, Pattern& pattern) {

        FixedVector<bool, PatternConfig::max_stack_size> stack;

        size_t cond_index = 0;

        for (const auto op: pattern.operations) {

            if (op == OperationType::PUSH) {
                stack.push_back(evaluate_condition(data, pattern.conditions[cond_index++]));
                continue;
            }

            const bool val = stack.back();
            stack.pop_back();

            switch (op) {
                case OperationType::OR:
                    stack.back() |= val;
                    break;
                case OperationType::AND:
                    stack.back() &= val;
                    break;
                case OperationType::NOR:
                    stack.back()  = !(stack.back() | val);
                    break;
                case OperationType::NAND:
                    stack.back()  = !(stack.back() & val);
                    break;
                case OperationType::XOR:
                    stack.back()  ^= val;
                    break;
                case OperationType::XNOR:
                    stack.back() = !(stack.back() ^ val);
                    break;
                default:
                    break;
            }
        }

        return stack.back();

    }

    std::optional<Pattern> compile_pattern(ReadVector<Byte> notation) {

        Pattern pattern;

        size_t curr_ind = 0;

        const auto num_conds = get_uint16(notation, curr_ind);

        if (num_conds < 0) return std::nullopt;

        pattern.conditions.realloc(num_conds);

        for (int i=0; i<num_conds; i++) {
            const auto cond_type = static_cast<ConditionType>(notation[curr_ind++]);

            const auto start_range = get_uint16(notation, curr_ind);
            const auto end_range = get_uint16(notation, curr_ind);

            if (curr_ind >= notation.len()) {
                pattern.free_memory();
                return std::nullopt;
            }

            pattern.conditions.push_back(Condition());

            Condition& cond = pattern.conditions.back();

            cond.start_range = start_range;
            cond.end_range = end_range;

            cond.type = cond_type;

            size_t vals_size = end_range - start_range;

            switch (cond_type) {
                case ConditionType::RANGE:
                    vals_size *= 2;
                    break;
                case ConditionType::SELECTION: {
                    const auto num_vals = get_uint16(notation, curr_ind);
                    vals_size *= num_vals;
                    break;
                }
                case ConditionType::NRANGE:
                    vals_size *= 2;
                    break;
                case ConditionType::NSELECTION: {
                    const auto num_vals = get_uint16(notation, curr_ind);
                    vals_size *= num_vals;
                    break;
                }
                default:
                    break;
            }

            if (curr_ind + vals_size >= notation.len()) {
                pattern.free_memory();
                return std::nullopt;
            }

            cond.vals.realloc(vals_size);

            for (size_t j=0; j<vals_size; j++) {
                cond.vals.push_back(notation[curr_ind++]);
            }

        }

        const auto num_operands = get_uint16(notation, curr_ind);

        pattern.operations.realloc(num_operands);

        int stack_size = 0;

        for (int i=0; i<num_operands; i++) {

            const auto op_type = static_cast<OperationType>(notation[curr_ind++]);

            if (op_type == OperationType::PUSH)
                stack_size++;
            else
                stack_size--;

            if (stack_size <= 0) {
                Logger<ModuleId::PATTERN_MATCHER>::log(ReadPtr<char>("Invalid Notation Recieved"));
                pattern.free_memory();
                return std::nullopt;
            }
            pattern.operations.push_back(op_type);
        }

        if (stack_size > 1) {
            pattern.free_memory();
            return std::nullopt;
        }

        return std::make_optional(pattern);
    }


}
