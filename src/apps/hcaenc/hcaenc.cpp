#include <string>

#include <argparse.hpp>
#include <vgaudio.h>

using namespace std;

static void SetProgramArguments(argparse::ArgumentParser &program);

static const char *GetQualityDescription(CRIHCA_QUALITY quality);

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

    const auto srcPath = program.get<std::string>("wave-in");
    const auto dstPath = program.get<std::string>("hca-out");
    const auto quality = program.get<CRIHCA_QUALITY>("--quality");

    if (!vgaFileExists(srcPath.c_str())) {
        fprintf(stderr, "Input file '%s' does not exist\n", srcPath.c_str());
        return 1;
    }

    fprintf(stdout, "Encode WAV to HCA\nInput: %s\nOutput: %s\nQuality: %s\n", srcPath.c_str(), dstPath.c_str(), GetQualityDescription(quality));

    HCA_ENCODE_CONFIG config;
    memset(&config, 0, sizeof(config));

    config.quality = quality;

    EncodeHcaFile(srcPath.c_str(), dstPath.c_str(), &config);

    fprintf(stdout, "\nDone.\n");

    return 0;
}

static void SetProgramArguments(argparse::ArgumentParser &program) {
    program.add_argument("wave-in")
        .required()
        .help("path of input wave file");
    program.add_argument("hca-out")
        .required()
        .help("path of output HCA file");
    program.add_argument("--quality", "-q")
        .default_value(CRIHCA_QUALITY_NOT_SET)
        .help("HCA quality (1 [highest] to 5 [lowest], 0 = default/high)")
        .action([](const std::string &value) {
            auto n = std::stoi(value);

            switch (n) {
                case CRIHCA_QUALITY_NOT_SET:
                case CRIHCA_QUALITY_HIGHEST:
                case CRIHCA_QUALITY_HIGH:
                case CRIHCA_QUALITY_MIDDLE:
                case CRIHCA_QUALITY_LOW:
                case CRIHCA_QUALITY_LOWEST:
                    break;
                default:
                    n = CRIHCA_QUALITY_NOT_SET;
                    break;
            }

            return n;
        });
}

static const char *GetQualityDescription(CRIHCA_QUALITY quality) {
    switch (quality) {
        case CRIHCA_QUALITY_NOT_SET:
            return "Not set (default=High)";
        case CRIHCA_QUALITY_HIGHEST:
            return "Highest";
        case CRIHCA_QUALITY_HIGH:
            return "High";
        case CRIHCA_QUALITY_MIDDLE:
            return "Middle";
        case CRIHCA_QUALITY_LOW:
            return "Low";
        case CRIHCA_QUALITY_LOWEST:
            return "Lowest";
        default:
            return "Invalid";
    }
}
