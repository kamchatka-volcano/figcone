#include <figcone/figcone.h>
#include <filesystem>
#include <iostream>
#include <map>
#include <vector>

struct Host {
    std::string ip;
    int port;
};

namespace figcone {
template<>
struct StringConverter<Host> {
    static std::optional<Host> fromString(const std::string& data)
    {
        auto delimPos = data.find(':');
        if (delimPos == std::string::npos)
            return {};
        auto host = Host{};
        host.ip = data.substr(0, delimPos);
        host.port = std::stoi(data.substr(delimPos + 1, data.size() - delimPos - 1));
        return host;
    }
};
} //namespace figcone

struct SharedAlbumCfg {
    std::filesystem::path dir;
    std::string name;
    std::vector<Host> hosts;
};
struct PhotoViewerCfg {
    std::filesystem::path rootDir;
    std::vector<std::string> supportedFiles;
    std::vector<SharedAlbumCfg> sharedAlbums;
    std::map<std::string, std::string> envVars;

    using traits = figcone::FieldTraits<figcone::OptionalField<&PhotoViewerCfg::envVars>>;
};

int main()
{
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.readYaml<PhotoViewerCfg>(R"(
      rootDir: ~/Photos
      supportedFiles: [ ".jpg", "png"]
      sharedAlbums:
        -
          dir: "summer_2019"
          name: "Summer 2019"
          hosts: ["127.0.0.1:8080"]
    )");

    std::cout << "Launching PhotoViewer in directory " << cfg.rootDir << std::endl;

    if (!cfg.supportedFiles.empty())
        std::cout << "Supported files:" << std::endl;
    for (const auto& file : cfg.supportedFiles)
        std::cout << "  " << file << std::endl;

    if (!cfg.sharedAlbums.empty())
        std::cout << "Shared albums:" << std::endl;
    for (const auto& album : cfg.sharedAlbums) {
        std::cout << "  Album:" << album.name << std::endl;
        std::cout << "    Hosts:" << std::endl;
        for (const auto& host : album.hosts)
            std::cout << "      " << host.ip << ":" << host.port << std::endl;
    }

    return 0;
}
