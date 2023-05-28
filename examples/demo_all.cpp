#include "demo.h"
#include "print_demo.h"
#include <figcone/configreader.h>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

PhotoViewerCfg readCfg(const fs::path& path)
{
    auto cfgReader = figcone::ConfigReader{};
    if (path.extension() == ".ini")
        return cfgReader.readIniFile<PhotoViewerCfg>(std::filesystem::weakly_canonical(path));
    else if (path.extension() == ".json")
        return cfgReader.readJsonFile<PhotoViewerCfg>(std::filesystem::weakly_canonical(path));
    else if (path.extension() == ".shoal")
        return cfgReader.readShoalFile<PhotoViewerCfg>(std::filesystem::weakly_canonical(path));
    else if (path.extension() == ".toml")
        return cfgReader.readTomlFile<PhotoViewerCfg>(std::filesystem::weakly_canonical(path));
    else if (path.extension() == ".xml")
        return cfgReader.readXmlFile<PhotoViewerCfg>(std::filesystem::weakly_canonical(path));
    else if (path.extension() == ".yaml")
        return cfgReader.readYamlFile<PhotoViewerCfg>(std::filesystem::weakly_canonical(path));
    else
        throw std::runtime_error{"unknown format"};
}

int mainApp(const std::filesystem::path& cfgPath)
{
    try {
        auto cfg = readCfg(cfgPath);
        std::cout << "Launching PhotoViewer in directory " << cfg.rootDir << std::endl;
        printDemoConfig(cfg);
    }
    catch(figcone::ConfigError& error)
    {
        std::cout << "Configuration error: " << error.what() << std::endl;
        return 1;
    }
    return 0;
}

#ifdef _WIN32
int wmain(int argc, wchar_t** argv)
{
    if (argc < 2)
        throw std::runtime_error{"A path to the config file must be specified in the command line"};
    auto cfgPath = std::filesystem::path{argv[1]};
    return mainApp(cfgPath);
}
#else
int main(int argc, char** argv)
{
    if (argc < 2)
        throw std::runtime_error{"A path to the config file must be specified in the command line"};
    auto cfgPath = std::filesystem::path{argv[1]};
    return mainApp(cfgPath);
}
#endif