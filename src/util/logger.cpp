#include <mzx/util/logger.h>

namespace mzx {

LoggerLevel Logger::level_ = LoggerLevel::Warn;
Logger::FnPrintLog Logger::log_print_ = PrintLog;

}
