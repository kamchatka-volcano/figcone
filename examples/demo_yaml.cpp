#include "demo.h"
#include "print_demo.h"
#include <iostream>

int main()
{
    auto cfg = PhotoViewerCfg{};
    cfg.readYamlFile(std::filesystem::canonical("../../examples/demo.yaml"));
    std::cout << "Launching PhotoViewer in directory" << cfg.rootDir << std::endl;
    printDemoConfig(cfg);

    return 0;
}