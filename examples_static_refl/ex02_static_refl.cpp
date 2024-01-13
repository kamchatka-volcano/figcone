#include <figcone/figcone.h>
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

int main()
{
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::SnakeCase};
    auto cfg = cfgReader.readToml<PhotoViewerCfg>(R"(
        root_dir = "/home/kamchatka-volcano/photos"
        supported_files = [".jpg", ".png"]
        [thumbnail_settings]
          max_width = 256
          max_height = 256
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