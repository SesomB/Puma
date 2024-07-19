
#include "argparse/argparse.hpp"
#include "puma-control-plane/puma-control-plane.hpp"
#include "spdlog/spdlog.h"

void onApplicationInterrupted(void *cookie)
{
    reinterpret_cast<PumaControlPlane *>(cookie)->Stop();
    spdlog::info("Shutting Down...");
}

int main(int argc, char *argv[])
{
    argparse::ArgumentParser program(argv[0], "0.1.0", argparse::default_arguments::help);

    program.add_argument("-v", "--verbose")
        .default_value(std::string("info"))
        .help("set verbose level - trace/debug/info/warn/error");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        spdlog::error(err.what());
        return 1;
    }

    std::string verboseLevel = program.get<std::string>("--verbose");
    spdlog::set_level(spdlog::level::from_str(verboseLevel));

    std::cout << "Selected verbosity level is " << spdlog::level::to_string_view(spdlog::get_level()).data()
              << std::endl;

    PumaControlPlane pumaControlPlane;

    // Register the on app close event handler
    pcpp::ApplicationEventHandler::getInstance().onApplicationInterrupted(onApplicationInterrupted, &pumaControlPlane);

    pumaControlPlane.Run();

    return 0;
}
