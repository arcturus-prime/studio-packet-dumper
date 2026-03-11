#include <cstddef>
#include <cstdint>
#include <optional>

struct NetworkStream
{
    uint8_t* data;
    size_t size;
    size_t bit_cursor;

    NetworkStream(uint8_t* data, size_t size) : data(data), size(size), bit_cursor(0)
    {
    }

    size_t bits_remaining() const
    {
        size_t total_bits = this->size * 8;
        return (bit_cursor < total_bits) ? (total_bits - bit_cursor) : 0;
    }

    bool is_empty() const
    {
        return bits_remaining() == 0;
    }

    inline std::optional<bool> read_bit()
    {
        if (bits_remaining() < 1)
        {
            return std::nullopt;
        }

        size_t byte = bit_cursor / 8;
        size_t bit = bit_cursor % 8;
        bit_cursor += 1;

        return (data[byte] >> (7 - bit)) & 1;
    }

    inline std::optional<uint64_t> read_bits(size_t count)
    {
        if (count == 0)
        {
            return 0;
        }

        if (count > 64 || bits_remaining() < count)
        {
            return std::nullopt;
        }

        uint64_t result = 0;
        for (size_t i = 0; i < count; ++i)
        {
            size_t byte = bit_cursor / 8;
            size_t bit = bit_cursor % 8;
            uint64_t b = (data[byte] >> (7 - bit)) & 1;
            result = (result << 1) | b;
            bit_cursor += 1;
        }

        return result;
    }

    inline std::optional<bool> read_bool_byte() {
        auto value = read_u8();

        if (!value)
            return std::nullopt;

        return *value == 0x01;
    }

    inline std::optional<uint8_t> read_u8()
    {
        auto value = read_bits(8);

        if (!value)
            return std::nullopt;

        return static_cast<uint8_t>(*value);
    }

    inline std::optional<uint16_t> read_u16_be()
    {
        auto value = read_bits(16);

        if (!value)
            return std::nullopt;

        return static_cast<uint16_t>(*value);
    }

    inline std::optional<uint32_t> read_u32_be()
    {
        auto value = read_bits(32);

        if (!value)
            return std::nullopt;

        return static_cast<uint32_t>(*value);
    }

    inline std::optional<uint64_t> read_u64_be()
    {
        return read_bits(64);
    }

    inline std::optional<int16_t> read_i16_be()
    {
        auto value = read_u16_be();

        if (!value)
            return std::nullopt;

        return static_cast<int16_t>(*value);
    }

    inline std::optional<int32_t> read_i32_be()
    {
        auto value = read_u32_be();

        if (!value)
            return std::nullopt;

        return static_cast<int32_t>(*value);
    }

    inline std::optional<int64_t> read_i64_be()
    {
        auto value = read_u64_be();

        if (!value)
            return std::nullopt;

        return static_cast<int64_t>(*value);
    }

    inline std::optional<float> read_f32_be()
    {
        auto value = read_u32_be();

        if (!value)
            return std::nullopt;

        float f;
        std::memcpy(&f, &*value, sizeof(f));

        return f;
    }

    inline std::optional<double> read_f64_be()
    {
        auto value = read_u64_be();

        if (!value)
            return std::nullopt;

        double d;
        std::memcpy(&d, &*value, sizeof(d));

        return d;
    }

    inline std::optional<uint16_t> read_u16_le()
    {
        auto value = read_u16_be();

        if (!value)
            return std::nullopt;

        uint16_t v = *value;

        return static_cast<uint16_t>((v >> 8) | (v << 8));
    }

    inline std::optional<uint32_t> read_u32_le()
    {
        auto value = read_u32_be();

        if (!value)
            return std::nullopt;

        uint32_t v = *value;
        return ((v >> 24) & 0x000000FF) | ((v >> 8) & 0x0000FF00) | ((v << 8) & 0x00FF0000) | ((v << 24) & 0xFF000000);
    }

    inline std::optional<uint64_t> read_u64_le()
    {
        auto value = read_u64_be();

        if (!value)
            return std::nullopt;

        uint64_t v = *value;
        return ((v >> 56) & 0x00000000000000FF) | ((v >> 40) & 0x000000000000FF00) | ((v >> 24) & 0x0000000000FF0000) |
               ((v >> 8) & 0x00000000FF000000) | ((v << 8) & 0x000000FF00000000) | ((v << 24) & 0x0000FF0000000000) |
               ((v << 40) & 0x00FF000000000000) | ((v << 56) & 0xFF00000000000000);
    }

    inline std::optional<int16_t> read_i16_le()
    {
        auto value = read_u16_le();

        if (!value)
            return std::nullopt;

        return static_cast<int16_t>(*value);
    }

    inline std::optional<int32_t> read_i32_le()
    {
        auto value = read_u32_le();

        if (!value)
            return std::nullopt;

        return static_cast<int32_t>(*value);
    }

    inline std::optional<int64_t> read_i64_le()
    {
        auto value = read_u64_le();

        if (!value)
            return std::nullopt;

        return static_cast<int64_t>(*value);
    }

    inline std::optional<float> read_f32_le()
    {
        auto value = read_u32_le();

        if (!value)
            return std::nullopt;

        float f;
        std::memcpy(&f, &*value, sizeof(f));

        return f;
    }

    inline std::optional<double> read_f64_le()
    {
        auto value = read_u64_le();

        if (!value)
            return std::nullopt;

        double d;
        std::memcpy(&d, &*value, sizeof(d));

        return d;
    }

    inline void align_to_byte()
    {
        size_t remainder = bit_cursor % 8;
        if (remainder != 0)
        {
            bit_cursor += (8 - remainder);
        }
    }

    inline bool skip_bits(size_t count)
    {
        if (bits_remaining() < count)
        {
            return false;
        }

        bit_cursor += count;

        return true;
    }
};
