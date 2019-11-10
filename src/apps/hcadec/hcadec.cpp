#include <string>
#include <cinttypes>

#include <argparse.hpp>
#include <vgaudio.h>

using namespace std;

static void SetProgramArguments(argparse::ArgumentParser &program);

static uint64_t GetEncryptionKeyParams(argparse::ArgumentParser &program);

int main(int argc, const char *argv[]) {
    const std::string strProgramName("hcaenc");
    argparse::ArgumentParser program(strProgramName);

    SetProgramArguments(program);

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &e) {
        if (argc > 1) {
            // Called with at least one parameter
            fprintf(stderr, "%s\n\n", e.what());
        }

        program.print_help();

        return 0;
    }

    const auto srcPath = program.get<std::string>("hca-in");
    const auto dstPath = program.get<std::string>("wave-out");
    const auto key = GetEncryptionKeyParams(program);

    if (!vgaFileExists(srcPath.c_str())) {
        fprintf(stderr, "Input file '%s' does not exist\n", srcPath.c_str());
        return 1;
    }

    fprintf(stdout, "Decode HCA to WAV\nInput: %s\nOutput: %s\nKey: %" PRIu64 "\n", srcPath.c_str(), dstPath.c_str(), key);

    HCA_DECODE_CONFIG config;
    memset(&config, 0, sizeof(config));

    config.key = key;

    DecodeHcaFile(srcPath.c_str(), dstPath.c_str(), &config);

    fprintf(stdout, "\nDone.\n");

    return 0;
}

static void SetProgramArguments(argparse::ArgumentParser &program) {
    program.add_argument("hca-in")
        .required()
        .help("path of input HCA file");
    program.add_argument("wave-out")
        .required()
        .help("path of output wave file");
    program.add_argument("--key-hex")
        .default_value(static_cast<uint64_t>(0))
        .help("Decryption key (hex)")
        .action([](const std::string &value) {
            uint64_t result = std::stoull(value, nullptr, 16);
            return result;
        });
    program.add_argument("--key", "-k")
        .default_value(static_cast<uint64_t>(0))
        .help("Decryption key")
        .action([](const std::string &value) {
            uint64_t result = std::stoull(value, nullptr, 10);
            return result;
        });
    program.add_argument("--lower-key-hex", "-a")
        .default_value(static_cast<uint32_t>(0))
        .help("Decryption key (lower 32 bits, hex)")
        .action([](const std::string &value) {
            uint32_t result = std::stoull(value, nullptr, 16);
            return result;
        });
    program.add_argument("--higher-key-hex", "-b")
        .default_value(static_cast<uint32_t>(0))
        .help("Decryption key (higher 32 bits, hex)")
        .action([](const std::string &value) {
            uint32_t result = std::stoull(value, nullptr, 16);
            return result;
        });
    program.add_argument("--sub-key", "-s")
        .default_value(static_cast<uint16_t>(0))
        .help("Decryption sub-key")
        .action([](const std::string &value) {
            uint16_t result = std::stoull(value, nullptr, 10);
            return result;
        });
    program.add_argument("--sub-key-hex")
        .default_value(static_cast<uint16_t>(0))
        .help("Decryption sub-key (hex)")
        .action([](const std::string &value) {
            uint16_t result = std::stoull(value, nullptr, 16);
            return result;
        });
}

static inline uint64_t TransformKey(uint64_t key, uint16_t subKey) {
    return key * ((static_cast<uint64_t>(subKey) << 16u) | (static_cast<uint16_t>(~subKey) + 2u));
}

static uint64_t GetEncryptionKeyParams(argparse::ArgumentParser &program) {
    uint64_t mainKey = 0;
    uint16_t subKey = 0;

    do {
        auto u64 = program.get<uint64_t>("--key");

        if (u64 != 0) {
            mainKey = u64;
            break;
        }

        u64 = program.get<uint64_t>("--key-hex");

        if (u64 != 0) {
            mainKey = u64;
            break;
        }

        auto keyLow = program.get<uint32_t>("--lower-key-hex");
        auto keyHigh = program.get<uint32_t>("--higher-key-hex");

        mainKey = (static_cast<uint64_t>(keyHigh) << 32u) | (static_cast<uint64_t>(keyLow));
    } while (false);

    if (mainKey == 0) {
        return 0;
    }

    do {
        auto u16 = program.get<uint16_t>("--sub-key");

        if (u16 != 0) {
            subKey = u16;
            break;
        }

        u16 = program.get<uint16_t>("--sub-key-hex");

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
