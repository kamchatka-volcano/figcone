#include "demo.h"
#include "print_demo.h"
#include <figcone/configreader.h>
#include <iostream>

int main()
{
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.readTomlFile<PhotoViewerCfg>(std::filesystem::canonical("../../examples/demo.toml"));
    std::cout << "Launching PhotoViewer in directory " << cfg.rootDir << std::endl;
    printDemoConfig(cfg);
    return 0;
}