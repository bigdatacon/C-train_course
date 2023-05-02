#pragma once

#include <string>

#include <fstream>
#include <iostream>

// напишите эту функцию
//inline bool DecodeRLE(const std::string& src_name, const std::string& dst_name);




inline bool DecodeRLE(const std::string& src_name, const std::string& dst_name) {
    std::ifstream in(src_name, std::ios::binary);
    if (!in) {
        return false;
    }

    std::ofstream out(dst_name, std::ios::binary);
    if (!out) {
        return false;
    }

    char block[CompressorRLE::max_block_size];
    char repeat_char = 0;
    int repeat_count = 0;

    while (true) {
        unsigned char code = in.get();
        if (in.eof()) {
            break;
        }

        if ((code & 1) == 0) {
            // обычный блок
            int size = (code >> 1) + 1;
            in.read(block, size);

            if (in.gcount() != size) {
                // ошибка чтения
                return false;
            }

            out.write(block, size);
        } else {
            // специальный блок
            int size = (code >> 1) + 1;
            repeat_count = size - 1;

            if (in.eof()) {
                // ошибка чтения
                return false;
            }

            repeat_char = in.get();
            out.write(&repeat_char, 1);

            for (int i = 1; i < repeat_count; ++i) {
                out.write(&repeat_char, 1);
            }
        }
    }

    return true;
}


