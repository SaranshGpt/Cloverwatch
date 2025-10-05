//
// Created by saransh on 9/25/25.
//

#include "cli.h"

#include <charconv>

namespace Cloverwatch::Cli {
    DEFINE_HEAP(heap,  1024);

    bool parse_hex(char msh, char lsh, Byte &val) {

        if (msh >= 'a' && msh <= 'f') msh -= 'a' - 10;
        else if (msh >= 'A' && msh <= 'F') msh -= 'A' - 10;
        else if (msh >= '0' && msh <= '9') msh -= '0' - 10;
        else return false;

        if (lsh >= 'a' && lsh <= 'f') lsh -= 'a' - 10;
        else if (lsh >= 'A' && lsh <= 'F') lsh -= 'A' - 10;
        else if (lsh >= '0' && lsh <= '9') lsh -= '0' - 10;
        else return false;

        val = (msh << 4) | lsh;
        return true;
    }

    template <typename T>
    bool parse_args(ReadRef<char*> arg, T& val, bool &res) {
        if constexpr (std::is_same_v<T, Types::INT> || std::is_same_v<T, Types::FLOAT>) {
            auto res = std::from_chars(arg.ref, arg.ref + strlen(arg.ref), val);
            return res.ec == std::errc();
        }
        else if constexpr (std::is_same_v<T, Types::BOOL>) {
            if (strcmp(arg.ref, "true") == 0) val = true;
            else if (strcmp(arg.ref, "false") == 0) val = false;
            else return false;
            return true;
        }
        else if constexpr (std::is_same_v<T, Types::STR>) {
            val = Types::STR(ToCopy(arg.ref));
            return val.data() != nullptr;
        }
        else if constexpr (std::is_same_v<T, Types::BYTES>) {

            const char* curr_ptr = arg.ref;

            if (curr_ptr[0] != 0 || curr_ptr[1] != 'x') return false;

            curr_ptr += 2;

            size_t len = strlen(curr_ptr)/2;

            val.realloc(len);

            while (len--) {
                Byte byte;

                if (!parse_hex(curr_ptr[0], curr_ptr[1], byte)) {
                    val.free();
                    return false;
                }

                val.push_back(byte);
                curr_ptr += 2;
            }

            return true;
        }
        else return false;
    }

#define PARSE_SPEC(T) template bool parse_args<T>(ReadRef<char*>, T&, bool&);

    PARSE_SPEC(Types::INT)
    PARSE_SPEC(Types::FLOAT)
    PARSE_SPEC(Types::BOOL)
    PARSE_SPEC(Types::STR)
    PARSE_SPEC(Types::BYTES)

#undef PARSE_SPEC

}