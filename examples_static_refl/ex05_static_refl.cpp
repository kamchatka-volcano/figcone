#include <figcone/figcone.h>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>

struct ThumbnailCfg {
    int maxWidth;
    int maxHeight;
};

struct PhotoViewerCfg {
    std::filesystem::path rootDir;
    std::vector<std::string> supportedFiles;
    ThumbnailCfg thumbnailSettings;
};

namespace figcone {
template<>
void PostProcessor<PhotoViewerCfg>::operator()(PhotoViewerCfg& cfg)
{
    auto supportPng = std::find(cfg.supportedFiles.begin(), cfg.supportedFiles.end(), ".png") !=
            cfg.supportedFiles.end();
    if (supportPng && cfg.thumbnailSettings.maxWidth > 128)
        throw ValidationError{"thumbnail width can't be larger than 128px when png images are present"};
}
} //namespace figcone

int main()
{
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.readToml<PhotoViewerCfg>(R"(
        rootDir = "~/Photos"
        supportedFiles = [".jpg", ".png"]
        [thumbnailSettings]
          maxWidth = 256
          maxHeight = 256
    )");
    //At this point your config is ready to use
    std::cout << "Launching PhotoViewer in directory " << cfg.rootDir << std::endl;

    if (!cfg.supportedFiles.empty())
        std::cout << "Supported files:" << std::endl;
    for (const auto& file : cfg.supportedFiles)
        std::cout << "  " << file << std::endl;

    std::cout << "Thumbnail settings:" << std::endl;
    std::cout << "  Max width:" << cfg.thumbnailSettings.maxWidth << std::endl;
    std::cout << "  Max height:" << cfg.thumbnailSettings.maxHeight << std::endl;
    return 0;
}