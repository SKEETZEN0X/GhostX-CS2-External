#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

constexpr uint8_t rotl8(uint8_t v, unsigned int c) {
    return (v << c) | (v >> (8 - c));
}
constexpr uint8_t rotr8(uint8_t v, unsigned int c) {
    return (v >> c) | (v << (8 - c));
}

constexpr uint32_t lcg_next(uint32_t seed) {
    return (seed * 0x343FD + 0x269EC3) & 0xFFFFFFFF;
}

template <std::size_t N>
constexpr void permute_blocks(std::array<uint8_t, N>& arr) {
    for (std::size_t i = 0; i + 3 < N - 1; i += 4) {
        uint8_t tmp0 = arr[i];
        uint8_t tmp1 = arr[i + 1];
        arr[i] = arr[i + 3];
        arr[i + 1] = arr[i + 2];
        arr[i + 2] = tmp1;
        arr[i + 3] = tmp0;
    }
}

template <std::size_t N>
constexpr void inverse_permute_blocks(std::array<uint8_t, N>& arr) {
    permute_blocks(arr);
}

constexpr uint32_t hash_constexpr(const char* str, size_t idx = 0, uint32_t hash = 2166136261u) {
    return str[idx] == 0 ? hash : hash_constexpr(str, idx + 1, (hash ^ str[idx]) * 16777619u);
}

constexpr uint32_t mix32(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

template <std::size_t N>
class ObfuscatedString {
    std::array<uint8_t, N> encrypted_;
    mutable std::array<uint8_t, N> decrypted_{};
    mutable bool decrypted_flag_ = false;

    static constexpr uint32_t base_seed1 = mix32(
        hash_constexpr(
            "seed1_" STR(N) "_" __TIME__ "_" __DATE__ "_" __FILE__ "_" STR(__LINE__) "_" STR(__COUNTER__)
        )
    );

    static constexpr uint32_t base_seed2 = mix32(
        hash_constexpr(
            "seed2_" STR(N) "_" __TIME__ "_" __DATE__ "_" __FILE__ "_" STR(__LINE__) "_" STR(__COUNTER__)
        )
    );

    static constexpr uint32_t seed1_ = mix32(base_seed1 ^ 0xA5A5A5A5u);
    static constexpr uint32_t seed2_ = mix32(base_seed2 ^ 0x5A5A5A5Au);

    static constexpr uint8_t final_mask_ = static_cast<uint8_t>(
        ((seed1_ ^ seed2_) & 0xFF) ^
        ((seed1_ >> 8) & 0xFF) ^
        ((seed2_ >> 16) & 0xFF) ^
        0xAB
        );

    constexpr void decrypt_internal() const {
        for (std::size_t i = 0; i < N; ++i)
            decrypted_[i] = encrypted_[i];

        for (std::size_t i = 0; i < N - 1; ++i)
            decrypted_[i] ^= final_mask_;

        uint32_t rot_key = seed1_;
        for (std::size_t i = 0; i < N - 1; ++i) {
            unsigned int rot = static_cast<unsigned int>((rot_key % 7) + 1);
            decrypted_[i] = rotr8(decrypted_[i], rot);
            rot_key = lcg_next(rot_key);
        }

        std::size_t len_to_permute = (N - 1) - ((N - 1) % 4);
        for (std::size_t i = 0; i + 3 < len_to_permute; i += 4) {
            uint8_t tmp0 = decrypted_[i];
            uint8_t tmp1 = decrypted_[i + 1];
            decrypted_[i] = decrypted_[i + 3];
            decrypted_[i + 1] = decrypted_[i + 2];
            decrypted_[i + 2] = tmp1;
            decrypted_[i + 3] = tmp0;
        }

        uint32_t key1 = seed1_;
        uint32_t key2 = seed2_;
        for (std::size_t i = 0; i < N - 1; ++i) {
            if ((i & 1) == 0) {
                decrypted_[i] ^= static_cast<uint8_t>(key1 & 0xFF);
                key1 = lcg_next(key1);
            }
            else {
                decrypted_[i] ^= static_cast<uint8_t>(key2 & 0xFF);
                key2 = lcg_next(key2);
            }
        }
        decrypted_[N - 1] = 0;
    }

public:

    __forceinline static constexpr std::array<uint8_t, N> encrypt(const char* str) {
        std::array<uint8_t, N> data{};

        for (std::size_t i = 0; i < N; ++i)
            data[i] = static_cast<uint8_t>(str[i]);

        uint32_t key1 = seed1_;
        uint32_t key2 = seed2_;
        for (std::size_t i = 0; i < N - 1; ++i) {
            if ((i & 1) == 0) {
                data[i] ^= static_cast<uint8_t>(key1 & 0xFF);
                key1 = lcg_next(key1);
            }
            else {
                data[i] ^= static_cast<uint8_t>(key2 & 0xFF);
                key2 = lcg_next(key2);
            }
        }

        std::size_t len_to_permute = (N - 1) - ((N - 1) % 4);
        for (std::size_t i = 0; i + 3 < len_to_permute; i += 4) {
            uint8_t tmp0 = data[i];
            uint8_t tmp1 = data[i + 1];
            data[i] = data[i + 3];
            data[i + 1] = data[i + 2];
            data[i + 2] = tmp1;
            data[i + 3] = tmp0;
        }

        uint32_t rot_key = seed1_;
        for (std::size_t i = 0; i < N - 1; ++i) {
            unsigned int rot = static_cast<unsigned int>((rot_key % 7) + 1);
            data[i] = rotl8(data[i], rot);
            rot_key = lcg_next(rot_key);
        }

        for (std::size_t i = 0; i < N - 1; ++i)
            data[i] ^= final_mask_;

        data[N - 1] = 0;

        return data;
    }

    __forceinline constexpr ObfuscatedString(const std::array<uint8_t, N>& encrypted)
        : encrypted_(encrypted) {
    }

    __forceinline const char* decrypt() const {
        if (!decrypted_flag_) {
            decrypt_internal();
            decrypted_flag_ = true;
        }
        return reinterpret_cast<const char*>(decrypted_.data());
    }
};

template <std::size_t N>
__forceinline constexpr auto make_obfuscated(const char(&str)[N]) {
    return ObfuscatedString<N>(ObfuscatedString<N>::encrypt(str));
}

#define OBF(str) ([]() -> const char* { \
    constexpr auto obf_str = make_obfuscated(str); \
    return obf_str.decrypt(); \
}())
