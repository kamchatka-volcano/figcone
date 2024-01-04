#include "demo.h"
#include "print_demo.h"
#include <figcone/configreader.h>
#include <iostream>

int main()
{
    auto cfgReader = figcone::ConfigReader{};
    auto cfgList = cfgReader.readYamlFile<PhotoViewerCfg, figcone::RootType::NodeList>(
            std::filesystem::canonical("../../examples/demo_root_list.yaml"));
    for (const auto& cfg : cfgList) {
        std::cout << "Launching PhotoViewer in directory " << cfg.rootDir << std::endl;
        printDemoConfig(cfg);
        std::cout << "---" << std::endl;
    }
}