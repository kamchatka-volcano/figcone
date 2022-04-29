#include <figcone/config.h>
#include <figcone/shortmacros.h> //enables macros without FIGCONE_ prefix
#include <filesystem>
#include <iostream>

struct Host{
    std::string ip;
    int port;
};

namespace figcone{
template<>
struct StringConverter<Host>{
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
}
struct SharedAlbumCfg : public figcone::Config<>{
    PARAM(dir, std::filesystem::path);
    PARAM(name, std::string);
    PARAMLIST(hosts, Host)();
};
struct PhotoViewerCfg : public figcone::Config<>{
    PARAM(rootDir, std::filesystem::path);
    PARAMLIST(supportedFiles, std::string);
    COPY_NODELIST(sharedAlbums, SharedAlbumCfg)();
    DICT(envVars)();
};

int main()
{
    auto cfg = PhotoViewerCfg{};
    cfg.readYaml(R"(
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
    for (const auto& album : cfg.sharedAlbums){
        std::cout << "  Album:" << album.name << std::endl;
        std::cout << "    Hosts:" << std::endl;
        for (const auto& host : album.hosts)
            std::cout << "      " << host.ip << ":" << host.port << std::endl;
    }

    return 0;
}
