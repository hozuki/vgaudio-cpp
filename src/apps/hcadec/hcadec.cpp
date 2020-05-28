#include <string>
#include <cstring>
#include <cinttypes>

#include <argparse.h>
#include <vgaudio.h>

using namespace std;

static void SetProgramArguments(argparse::ArgumentParser &program);

static uint64_t GetEncryptionKeyParams(argparse::ArgumentParser &program);

int main(int argc, const char *argv[]) {
    argparse::ArgumentParser program("hcadec", "hcadec");

    SetProgramArguments(program);

    const auto errInfo = program.parse(argc, argv);

    if (errInfo) {
        if (argc > 1) {
            fprintf(stderr, "%s\n\n", errInfo.what().c_str());
        }

        program.print_help();

        return -1;
    }

    const auto srcPath = program.get<std::string>("hca-in");
    const auto dstPath = program.get<std::string>("wave-out");
    const auto key = GetEncryptionKeyParams(program);

    if (!vgaUtilFileExists(srcPath.c_str())) {
        fprintf(stderr, "Input file '%s' does not exist\n", srcPath.c_str());
        return 1;
    }

    fprintf(stdout, "Decode HCA to WAV\nInput: %s\nOutput: %s\nKey: %" PRIu64 "\n", srcPath.c_str(), dstPath.c_str(), key);

    HCA_DECODE_CONFIG config;
    memset(&config, 0, sizeof(config));

    config.key = key;

    vgaDecodeHcaFile(srcPath.c_str(), dstPath.c_str(), &config);

    fprintf(stdout, "\nDone.\n");

    return 0;
}

static void SetProgramArguments(argparse::ArgumentParser &program) {
    program.add_argument()
        .name("hca-in")
        .position(0)
        .required(true)
        .description("path of input HCA file");
    program.add_argument()
        .name("wave-out")
        .position(1)
        .required(true)
        .description("path of output wave file");
    program.add_argument()
        .name("--key-hex")
        .description("decryption key (hex)");
    program.add_argument()
        .names({"-k", "--key"})
        .description("decryption key");
    program.add_argument()
        .names({"-a", "--lower-key-hex"})
        .description("decryption key (lower 32 bits, hex)");
    program.add_argument()
        .names({"-b", "--higher-key-hex"})
        .description("decryption key (higher 32 bits, hex)");
    program.add_argument()
        .names({"-s", "--sub-key"})
        .description("decryption sub-key");
    program.add_argument()
        .name("--sub-key-hex")
        .description("decryption sub-key (hex)");
}

static inline uint64_t TransformKey(uint64_t key, uint16_t subKey) {
    return key * ((static_cast<uint64_t>(subKey) << 16u) | (static_cast<uint16_t>(~subKey) + 2u));
}

template<typename T>
static T ConvertKeyFromString(const std::string &value, int base) {
    T result = std::stoull(value, nullptr, base);
    return result;
}

#define DEFINE_CONVERTER(ret_type, base, postfix) \
    static ret_type ConvertKey_##postfix(const std::string &value) { \
        return ConvertKeyFromString<ret_type>(value, base); \
    }

DEFINE_CONVERTER(uint16_t, 10, U16_OCT)

DEFINE_CONVERTER(uint16_t, 16, U16_HEX)

DEFINE_CONVERTER(uint32_t, 16, U32_HEX)

DEFINE_CONVERTER(uint64_t, 10, U64_OCT)

DEFINE_CONVERTER(uint64_t, 16, U64_HEX)

template<typename T, typename F = T *(const std::string &)>
T GetArg(argparse::ArgumentParser &parser, const char *argName, F converter) {
    if (parser.exists(argName)) {
        auto argValue = parser.get<std::string>(argName);
        return static_cast<T>(converter(argValue));
    } else {
        return T();
    }
}

static uint64_t GetEncryptionKeyParams(argparse::ArgumentParser &program) {
    uint64_t mainKey = 0;
    uint16_t subKey = 0;

    do {
        auto u64 = GetArg<uint64_t>(program, "key", ConvertKey_U64_OCT);

        if (u64 != 0) {
            mainKey = u64;
            break;
        }

        u64 = GetArg<uint64_t>(program, "key-hex", ConvertKey_U64_HEX);

        if (u64 != 0) {
            mainKey = u64;
            break;
        }

        auto keyLow = GetArg<uint32_t>(program, "lower-key-hex", ConvertKey_U32_HEX);
        auto keyHigh = GetArg<uint32_t>(program, "higher-key-hex", ConvertKey_U32_HEX);

        mainKey = (static_cast<uint64_t>(keyHigh) << 32u) | (static_cast<uint64_t>(keyLow));
    } while (false);

    if (mainKey == 0) {
        return 0;
    }

    do {
        auto u16 = GetArg<uint16_t>(program, "sub-key", ConvertKey_U16_OCT);

        if (u16 != 0) {
            subKey = u16;
            break;
        }

        u16 = GetArg<uint16_t>(program, "sub-key-hex", ConvertKey_U16_HEX);

        subKey = u16;
    } while (false);

    uint64_t key;

    if (subKey != 0) {
        key = TransformKey(mainKey, subKey);
    } else {
        key = mainKey;
    }

    return key;
}
