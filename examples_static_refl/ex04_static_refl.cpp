#include <figcone/figcone.h>
#include <filesystem>
#include <iostream>
#include <map>
#include <vector>

struct NotEmpty {
    template<typename TList>
    void operator()(const TList& list)
    {
        if (!list.empty())
            throw figcone::ValidationError{"can't be empty."};
    }
};

struct PathExists {
    void operator()(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path))
            throw figcone::ValidationError{"a path must exist"};
    }
};

struct PhotoViewerCfg {
    std::filesystem::path rootDir;
    std::vector<std::string> supportedFiles;
    std::map<std::string, std::string> envVars;

    using traits = figcone::FieldTraits<
            figcone::ValidatedField<&PhotoViewerCfg::rootDir, PathExists>,
            figcone::ValidatedField<&PhotoViewerCfg::supportedFiles, NotEmpty>,
            figcone::OptionalField<&PhotoViewerCfg::envVars>>;
};

int main()
{
    try {
        auto cfgReader = figcone::ConfigReader{};
        auto cfg = cfgReader.readYaml<PhotoViewerCfg>(R"(
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
    catch (const figcone::ConfigError& e) {
        std::cout << "Config error:" << e.what();
        return 1;
    }
}