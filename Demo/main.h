#pragma once

#include <fstream>
#include <iostream>
#include "igicamera/camera.h"
#include "igiintegrator/path_trace.h"
#include "igiscene/aggregate.h"
#include "igiscene/scene.h"
#include "igiutilities/serialize.h"
#include "render.h"

namespace demo {
    char *ReadConfig(const char *path, std::pmr::polymorphic_allocator<char> &alloc) {
        std::ifstream fs("demo.json");

        fs.seekg(0, std::ios_base::end);
        size_t nbuf = fs.tellg();
        fs.seekg(0);

        char *buf = alloc.allocate(nbuf);

        std::streambuf *sb = fs.rdbuf();

        char *const base = buf;
        while (true) {
            int c = sb->sbumpc();
            switch (c) {
                case '\n':
                    *buf++ = '\n';
                    break;
                case '\r':
                    if (sb->sgetc() == '\n')
                        sb->sbumpc();
                    *buf++ = '\n';
                    break;
                case std::streambuf::traits_type::eof():
                    fs.setstate(std::ios::eofbit);
                    *buf = '\0';
                    return base;
                default:
                    *buf++ = (char)c;
                    break;
            }
        }

        fs.close();
    }
}  // namespace demo