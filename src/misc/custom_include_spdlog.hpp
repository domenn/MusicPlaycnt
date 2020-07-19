#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:26812)
#pragma warning(disable:26451)
#endif

#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif