#include <figcone/config.h>
#include <figcone/errors.h>
#include <figcone/shortmacros.h>
#include <filesystem>
#include <iostream>

struct NotEmpty{
    template<typename TList>
    void operator()(const TList& list)
    {
        if (!list.empty())
            throw figcone::ValidationError{"can't be empty."};
    }
};

struct PhotoViewerCfg : public figcone::Config<>{
    PARAM(rootDir, std::filesystem::path).ensure([](const auto& path) {
        if (!std::filesystem::exists(path))
            throw figcone::ValidationError{"a path must exist"};
    });
    PARAMLIST(supportedFiles, std::string).ensure<NotEmpty>();
    DICT(envVars)();
};

int main()
{
    auto cfg = PhotoViewerCfg{};
    cfg.readYaml(R"(
      rootDir: ~/Photos
      supportedFiles: []
    )");

    std::cout << "Launching PhotoViewer in directory " << cfg.rootDir << std::endl;

    if (!cfg.supportedFiles.empty())
        std::cout << "Supported files:" << std::endl;
    for (const auto& file : cfg.supportedFiles)
        std::cout << "  " << file << std::endl;

    return 0;
}