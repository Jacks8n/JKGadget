#pragma once

#include <ostream>
#include <tuple>

namespace pngparvus {
    template <typename It, typename TPixel>
    struct IPNG {
        virtual size_t getWidth() const = 0;

        virtual size_t getHeight() const = 0;

        virtual It getPixels() const = 0;
    };

    class png_writer {
        uint32_t _crc, _adlerA, _adlerB;

    public:
        template <typename It, typename TPixel>
        std::ostream& write(std::ostream& out, IPNG<It, TPixel>& png) {
            constexpr size_t nc = sizeof(TPixel);
            static_assert(nc == 3 | nc == 4, "The size of TPixel must be 3 or 4");

            It pit = png.getPixels();
            uint32_t w = png.getWidth(), h = png.getHeight();
            uint32_t bytesPerRow = w * nc + 1;
            uint32_t codedBytesPerRow = (bytesPerRow << 24) | (bytesPerRow & 0xFF00) << 8
                | ((~bytesPerRow >> 8) & 0xFF) | ((~bytesPerRow & 0xFF) << 8);
            _adlerA = 1;
            _adlerB = 0;

            out.write("\x89PNG\r\n\32\n", 8);
            WriteChunkBegin(out, "IHDR", 13);
            WriteRawCRC(out, w);
            WriteRawCRC(out, h);
            WriteRawCRC(out, (char)8);
            WriteRawCRC(out, (uint32_t)(nc == 3 ? 2 : 6) << 24);
            WriteChunkEnd(out);

            WriteChunkBegin(out, "IDAT", 6 + h * (5 + bytesPerRow));
            WriteRawCRC(out, "\x78\1");
            for (size_t y = 0; y < h; y++) {
                WriteRawCRC(out, (char)(y == h - 1));
                WriteRawCRC(out, codedBytesPerRow);
                WriteAdlerByte(out, (char)0);
                for (size_t x = 0; x < w; x++, ++pit)
                    WriteRawAdler<TPixel>(out, *pit, std::make_index_sequence<nc>());
            }
            WriteRawCRC(out, (_adlerB << 16) | _adlerA);
            WriteChunkEnd(out);
            WriteChunkBegin(out, "IEND", 0);
            WriteChunkEnd(out);

            return out;
        }

    private:
        void WriteRaw(std::ostream& out, uint32_t val) {
            for (size_t i = 0; i < 4; i++)
                out.put(val >> (24 - i * 8) & 0xff);
        }

        void WriteRawCRC(std::ostream& out, char c) {
            static const uint32_t CRCTable[] = {
                0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4,
                0x4db26158, 0x5005713c, 0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
                0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };

            out.put(c);
            _crc ^= (unsigned)c;
            _crc = (_crc >> 4) ^ CRCTable[_crc & 15];
            _crc = (_crc >> 4) ^ CRCTable[_crc & 15];
        }

        void WriteRawCRC(std::ostream& out, uint32_t val) {
            for (size_t i = 0; i < 4; i++)
                WriteRawCRC(out, (char)(val >> (24 - i * 8)));
        }

        template <int N>
        void WriteRawCRC(std::ostream& out, const char(&val)[N]) {
            for (size_t i = 0; i < N - 1; i++)
                WriteRawCRC(out, (char)val[i]);
        }

        void WriteAdlerByte(std::ostream& out, char c) {
            WriteRawCRC(out, c);
            _adlerA = (_adlerA + c) % 65521;
            _adlerB = (_adlerA + _adlerB) % 65521;
        }

        template <typename T, typename ...Ts>
        void WriteAdlerByte(std::ostream& out, T t, Ts ...ts) {
            WriteAdlerByte(out, t);
            WriteAdlerByte(out, ts...);
        }

        template <typename TPixel, size_t ...N>
        void WriteRawAdler(std::ostream& out, TPixel pixel, std::index_sequence<N...>) {
            WriteAdlerByte(out, reinterpret_cast<char*>(&pixel)[N]...);
        }

        void WriteChunkBegin(std::ostream& out, const char(&str)[5], uint32_t len) {
            WriteRaw(out, len);
            _crc = ~0;
            WriteRawCRC(out, str);
        }

        void WriteChunkEnd(std::ostream& out) { WriteRaw(out, ~_crc); }
    };
} // namespace PNGParvus
