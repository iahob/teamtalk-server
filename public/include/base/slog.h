//
//  slog_api.h
//  slog
//
//  Created by luoning on 15-1-6.
//  Copyright (c) 2015年 luoning. All rights reserved.
//

#ifndef __slog__slog_api__
#define __slog__slog_api__
#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

static spdlog::level::level_enum GetlogLevel(const std::string& level_name) {
    if (level_name == "trace") {
        return spdlog::level::trace;
    }

    if (level_name == "debug") {
        return spdlog::level::debug;
    }

    if (level_name == "info") {
        return spdlog::level::info;
    }

    if (level_name == "warn") {
        return spdlog::level::warn;
    }

    if (level_name == "error") {
        return spdlog::level::err;
    }

    if (level_name == "fatal") {
        return spdlog::level::critical;
    }

    return spdlog::level::debug;
}



static void Init(const char* file_name, const char* level) {
    using namespace spdlog::sinks;
    auto file_sink = std::make_shared<rotating_file_sink_mt>(file_name, 1024 * 1024 * 10, 10);
    file_sink->
            set_level(GetlogLevel(level)
    );

    auto stderr_sink = std::make_shared<stderr_color_sink_mt>();
    stderr_sink->
            set_level(spdlog::level::debug);

    auto m_logger = std::make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{file_sink, stderr_sink});
    m_logger->
            set_level(spdlog::level::trace);
    m_logger->
            flush_on(GetlogLevel(level)
    );
    m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e %z] [T:%-5t] [P:%-5P] [%-5l] [%s:%#] %v");
    set_default_logger(m_logger);
}


#endif /* defined(__slog__slog_api__) */
