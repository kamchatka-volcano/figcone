#include <figcone/config.h>
#include <figcone/configreader.h>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>

#ifdef FIGCONE_NAMEOF_AVAILABLE
struct ThumbnailCfg : public figcone::Config {
    int maxWidth = param<&ThumbnailCfg::maxWidth>();
    int maxHeight = param<&ThumbnailCfg::maxHeight>();
};
#else
struct ThumbnailCfg : public figcone::Config {
    int maxWidth = param<&ThumbnailCfg::maxWidth>("maxWidth");
    int maxHeight = param<&ThumbnailCfg::maxHeight>("maxHeight");
};
#endif
struct PhotoViewerCfg : public figcone::Config {
    //config fields can also be created with macros:
    FIGCONE_PARAM(rootDir, std::filesystem::path);
    FIGCONE_PARAMLIST(supportedFiles, std::vector<std::string>);
    FIGCONE_NODE(thumbnailSettings, ThumbnailCfg);
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