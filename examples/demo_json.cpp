#include "demo.h"
#include "print_demo.h"
#include <iostream>

int main()
{
    auto cfg = PhotoViewerCfg{};
    cfg.readJsonFile(std::filesystem::canonical("../../examples/demo.json"));
    std::cout << "Launching PhotoViewer in directory" << cfg.rootDir << std::endl;
    printDemoConfig(cfg);

    return 0;
}