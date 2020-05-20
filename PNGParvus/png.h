#pragma once

#include <ostream>
#include <tuple>

namespace pngparvus {
    class pixel_rgb {
        uint8_t _c[3];

      public:
        constexpr pixel_rgb(uint8_t r, uint8_t g, uint8_t b)
            : _c { r, g, b } { }

        constexpr uint8_t operator[](size_t index) const {
#if _DEBUG
            if (index > 2) throw;
#endif
            return _c[index];
        }
    };

    class pixel_rgba {
        uint8_t _c[4];

      public:
        constexpr pixel_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
            : _c { r, g, b, a } { }

        constexpr uint8_t operator[](size_t index) const {
#if _DEBUG
            if (index > 3) throw;
#endif
            return _c[index];
        }
    };

    template <typename T>
    struct pixel_channel {
        static constexpr size_t value = 0;
    };

    template <>
    struct pixel_channel<pixel_rgb> {
        static constexpr size_t value = 3;
    };

    template <>
    struct pixel_channel<pixel_rgba> {
        static constexpr size_t value = 4;
    };

    template <typename T>
    constexpr size_t pixel_channel_v = pixel_channel<T>::value;

    template <typename T>
    constexpr bool is_pixel_v = pixel_channel_v<T> != 0;

    template <typename T>
    struct pixel_iterator_traits {
        using pixel_t = typename std::iterator_traits<T>::value_type;

        static constexpr size_t channel = std::is_same_v<pixel_t, pixel_rgb>
                                              ? 3
                                              : std::is_same_v<pixel_t, pixel_rgba> ? 4 : 0;
    };

    template <typename T>
    using it_pixel_t = typename pixel_iterator_traits<T>::pixel_t;

    template <typename T>
    constexpr size_t it_pixel_channel_v = pixel_iterator_traits<T>::channel;

    template <typename TIt>
    struct IPNG {
        virtual size_t getWidth() const = 0;

        virtual size_t getHeight() const = 0;

        virtual TIt getPixels() const = 0;
    };

    class png_writer {
        uint32_t _crc, _adlerA, _adlerB;

      public:
        template <typename TIt>
        std::ostream &write(std::ostream &out, IPNG<TIt> &png) {
            constexpr size_t nc = it_pixel_channel_v<TIt>;
            static_assert(nc == 3 || nc == 4);

            TIt pit    = png.getPixels();
            uint32_t w = png.getWidth(), h = png.getHeight();
            uint32_t bytesPerRow      = w * nc + 1;
            uint32_t codedBytesPerRow = (bytesPerRow << 24) | (bytesPerRow & 0xFF00) << 8
                                        | ((~bytesPerRow >> 8) & 0xFF) | ((~bytesPerRow & 0xFF) << 8);
            _adlerA = 1;
            _adlerB = 0;

            out.write("\x89PNG\r\n\32\n", 8);
            writeChunkBegin(out, "IHDR", 13);
            writeRawCRC(out, w);
            writeRawCRC(out, h);
            writeRawCRC(out, (uint8_t)8);
            writeRawCRC(out, (uint32_t)(nc == 3 ? 2 : 6) << 24);
            writeChunkEnd(out);

            writeChunkBegin(out, "IDAT", 6 + h * (5 + bytesPerRow));
            writeRawCRC(out, "\x78\1");
            for (size_t y = 0; y < h; y++) {
                writeRawCRC(out, (uint8_t)(y == h - 1));
                writeRawCRC(out, codedBytesPerRow);
                writeAdlerByte(out, (char)0);
                for (size_t x = 0; x < w; x++, ++pit)
                    writeRawAdler(out, *pit, std::make_index_sequence<nc>());
            }
            writeRawCRC(out, (_adlerB << 16) | _adlerA);
            writeChunkEnd(out);
            writeChunkBegin(out, "IEND", 0);
            writeChunkEnd(out);

            return out;
        }

      private:
        void writeRaw(std::ostream &out, uint32_t val) {
            for (size_t i = 0; i < 4; i++)
                out.put(val >> (24 - i * 8) & 0xff);
        }

        void writeRawCRC(std::ostream &out, uint8_t c) {
            static const uint32_t CRCTable[] = {
                0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4,
                0x4db26158, 0x5005713c, 0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
                0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
            };

            out.put(c);
            _crc ^= c;
            _crc = (_crc >> 4) ^ CRCTable[_crc & 15];
            _crc = (_crc >> 4) ^ CRCTable[_crc & 15];
        }

        void writeRawCRC(std::ostream &out, uint32_t val) {
            for (size_t i = 0; i < 4; i++)
                writeRawCRC(out, (uint8_t)(val >> (24 - i * 8)));
        }

        template <int N>
        void writeRawCRC(std::ostream &out, const char (&val)[N]) {
            for (size_t i = 0; i < N - 1; i++)
                writeRawCRC(out, (uint8_t)val[i]);
        }

        void writeAdlerByte(std::ostream &out, uint8_t c) {
            writeRawCRC(out, c);
            _adlerA = (_adlerA + c) % 65521;
            _adlerB = (_adlerA + _adlerB) % 65521;
        }

        template <typename TPixel, size_t... Is>
        void writeRawAdler(std::ostream &out, TPixel &&pixel, std::index_sequence<Is...>) {
            (writeAdlerByte(out, pixel[Is]), ...);
        }

        void writeChunkBegin(std::ostream &out, const char (&str)[5], uint32_t len) {
            writeRaw(out, len);
            _crc = ~0;
            writeRawCRC(out, str);
        }

        void writeChunkEnd(std::ostream &out) { writeRaw(out, ~_crc); }
    };
}  // namespace pngparvus
