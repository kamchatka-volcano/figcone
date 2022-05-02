#include <figcone/config.h>
#include <filesystem>
#include <iostream>
#include <vector>

struct ThumbnailCfg : public figcone::Config<figcone::NameFormat::SnakeCase>
{   
    int maxWidth = param<&ThumbnailCfg::maxWidth>();
    int maxHeight = param<&ThumbnailCfg::maxHeight>();
};
struct PhotoViewerCfg : public figcone::Config<figcone::NameFormat::SnakeCase>{
    //config fields can also be created with macros:
    FIGCONE_PARAM(rootDir, std::filesystem::path);
    FIGCONE_PARAMLIST(supportedFiles, std::vector<std::string>);
    FIGCONE_NODE(thumbnailSettings, ThumbnailCfg);
};

int main()
{
    auto cfg = PhotoViewerCfg{};
    cfg.readToml(R"(
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