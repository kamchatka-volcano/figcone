#pragma once
#include <figcone/config.h>
#include <figcone/shortmacros.h> //enables macros without FIGCONE_ prefix
#include <string>

struct ThumbnailCfg : public figcone::Config<>
{
    PARAM(enabled, bool)(true);
    PARAM(maxWidth, int);
    PARAM(maxHeight, int);
};
struct HostCfg : public figcone::Config<>{
    PARAM(ip, std::string);
    PARAM(port, int);
};
struct SharedAlbumCfg : public figcone::Config<>{
    PARAM(dir, std::filesystem::path);
    PARAM(name, std::string);
    NODELIST(hosts, HostCfg)();
};
struct PhotoViewerCfg : public figcone::Config<>{
    PARAM(rootDir, std::filesystem::path);
    PARAMLIST(supportedFiles, std::string);
    NODE(thumbnails, ThumbnailCfg);
    COPY_NODELIST(sharedAlbums, SharedAlbumCfg)();
    DICT(envVars)();
};