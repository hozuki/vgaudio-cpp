#include "../../../api/modules/vga_progress_reporter.h"
#include "../../../lib/vgaudio/IProgressReport.h"
#include "progress_report.h"

using namespace std;
using namespace vgaudio;

namespace vgaudio::api {

    struct CallbackProgressReporter final : IProgressReport {

    private:

        const vga_progress_reporter *_reporter;
        const void *_reporterData;

    public:

        CallbackProgressReporter(const vga_progress_reporter *reporter, const void *reporterData)
            : _reporter(reporter), _reporterData(reporterData) {
        }

        void report(int32_t value) override {
            if (_reporter != nullptr) {
                _reporter->report(value, _reporterData);
            }
        }

        void reportAdd(int32_t value) override {
            if (_reporter != nullptr) {
                _reporter->reportAdd(value, _reporterData);
            }
        }

        void setTotal(int32_t value) override {
            if (_reporter != nullptr) {
                _reporter->setTotal(value, _reporterData);
            }
        }

        void logMessage(const string &message) override {
            if (_reporter != nullptr) {
                _reporter->logMessage(message.c_str(), _reporterData);
            }
        }

    };

    shared_ptr<IProgressReport> CreateProgressReporter(const vga_progress_reporter *reporter, const void *reporterData) {
        return make_shared<CallbackProgressReporter>(reporter, reporterData);
    }

}
