#include "Jlog/sinks/ansicolor_sink.h"

int main(){
    Jlog::sinks::ansicolor_stdout_sink_st sink;
    sink.setShouldColor(true);
    Jlog::details::LogMessage log_msg("jlog", Jlog::level::k_err, "AnsicolorSink test");
    sink.log(log_msg);
    
    return 0;    
}