#pragma once
#include <figcone/figcone.h>
#include <figcone/shortmacros.h> //enables macros without FIGCONE_ prefix
#include <string>
#include <vector>
#include <map>

#ifdef FIGCONE_EXAMPLE_STATIC_REFLECTION

struct ThumbnailCfg {
    bool enabled = true;
    int maxWidth;
    int maxHeight;

    using traits = figcone::FieldTraits<
            figcone::OptionalField<&ThumbnailCfg::enabled>>;
};
struct HostCfg {
    std::string ip;
    int port;
};
struct SharedAlbumCfg {
    std::filesystem::path dir;
    std::string name;
    std::vector<HostCfg> hosts;

    using traits = figcone::FieldTraits<
            figcone::OptionalField<&SharedAlbumCfg::hosts>>;
};
struct PhotoViewerCfg {
    std::filesystem::path rootDir;
    std::vector<std::string> supportedFiles;
    ThumbnailCfg thumbnails;
    std::vector<SharedAlbumCfg> sharedAlbums;
    std::map<std::string, std::string> envVars;

    using traits = figcone::FieldTraits<
            figcone::OptionalField<&PhotoViewerCfg::envVars>,
            figcone::OptionalField<&PhotoViewerCfg::sharedAlbums>,
            figcone::CopyNodeListField<&PhotoViewerCfg::sharedAlbums>>;
};

#else

struct ThumbnailCfg : public figcone::Config
{
    PARAM(enabled, bool)(true);
    PARAM(maxWidth, int);
    PARAM(maxHeight, int);
};
struct HostCfg : public figcone::Config{
    PARAM(ip, std::string);
    PARAM(port, int);
};
struct SharedAlbumCfg : public figcone::Config{
    PARAM(dir, std::filesystem::path);
    PARAM(name, std::string);
    NODELIST(hosts, std::vector<HostCfg>)();
};
struct PhotoViewerCfg : public figcone::Config{
    PARAM(rootDir, std::filesystem::path);
    PARAMLIST(supportedFiles, std::vector<std::string>);
    NODE(thumbnails, ThumbnailCfg);
    COPY_NODELIST(sharedAlbums, std::vector<SharedAlbumCfg>)();
    using StringMap = std::map<std::string, std::string>;
    DICT(envVars, StringMap)();
};

#endif