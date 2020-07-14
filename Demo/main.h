#pragma once

#include <fstream>
#include <iostream>
#include "igicamera/camera.h"
#include "igiintegrator/path_trace.h"
#include "igiscene/aggregate.h"
#include "igiscene/scene.h"
#include "render.h"

namespace demo {
    size_t ReadConfig(std::ifstream &is, char *buf) {
        std::istream::sentry se(is, true);
        std::streambuf *sb = is.rdbuf();

        const char *const base = buf;
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
                    is.setstate(std::ios::eofbit);
                    return buf - base;
                default:
                    *buf++ = (char)c;
                    break;
            }
        }
    }
}  // namespace demo