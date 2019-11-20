#include "Jlog/details/pattern_formatter.h"
//#include "Jlog/details/log_msg.h"

using namespace Jlog;

int main() {
    details::PatternFormatter pf(details::os::default_eol);
    details::LogMessage log_msg("jlog", level::k_info, "PatternFormatter test");
    memory_buf_t buf;

    pf.format(log_msg, buf);
    fwrite(buf.data(), sizeof(char), buf.size(), stdout);
    return 0;
}

// output
//[2019-11-17 11:05:27.052] [jlog] [info] PatternFormatter test