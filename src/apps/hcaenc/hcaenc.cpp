#include <string>
#include <cstring>
#include <cassert>

#include <argparse.h>
#include <vgaudio.h>

using namespace std;

static void SetProgramArguments(argparse::ArgumentParser &program);

static const char *GetQualityDescription(CRIHCA_QUALITY quality);

int main(int argc, const char *argv[]) {
    argparse::ArgumentParser program("hcaenc", "hcadec");

    SetProgramArguments(program);

    const auto errInfo = program.parse(argc, argv);

    if (errInfo) {
        if (argc > 1) {
            fprintf(stderr, "%s\n\n", errInfo.what().c_str());
        }

        program.print_help();

        return -1;
    }

    const auto srcPath = program.get<std::string>("wave-in");
    const auto dstPath = program.get<std::string>("hca-out");

    CRIHCA_QUALITY quality;

    if (program.exists("quality")) {
        const auto s = program.get<std::string>("quality");
        auto n = std::stoi(s);

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

        quality = static_cast<CRIHCA_QUALITY>(n);
    } else {
        quality = CRIHCA_QUALITY_NOT_SET;
    }

    if (!vgaUtilFileExists(srcPath.c_str())) {
        fprintf(stderr, "Input file '%s' does not exist\n", srcPath.c_str());
        return 1;
    }

    fprintf(stdout, "Encode WAV to HCA\nInput: %s\nOutput: %s\nQuality: %s\n", srcPath.c_str(), dstPath.c_str(), GetQualityDescription(quality));

    HCA_ENCODE_CONFIG config;
    memset(&config, 0, sizeof(config));

    config.quality = quality;

    vgaEncodeHcaFile(srcPath.c_str(), dstPath.c_str(), &config);

    fprintf(stdout, "\nDone.\n");

    return 0;
}

static void SetProgramArguments(argparse::ArgumentParser &program) {
    program.add_argument()
        .name("wave-in")
        .position(0)
        .required(true)
        .description("path of input wave file");
    program.add_argument()
        .name("hca-out")
        .position(1)
        .required(true)
        .description("path of output HCA file");
    program.add_argument()
        .names({"-q", "--quality"})
        .description("HCA quality (1 [highest] to 5 [lowest], 0 = default/high)");
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
