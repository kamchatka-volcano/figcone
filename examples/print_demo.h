#pragma once
#include "demo.h"
#include <iostream>

inline void printDemoConfig(const PhotoViewerCfg& cfg)
{
    if (!cfg.supportedFiles.empty())
        std::cout << "Supported files:" << std::endl;
    for (const auto& file : cfg.supportedFiles)
        std::cout << "  " << file << std::endl;

    if (cfg.thumbnails.enabled) {
        std::cout << "Thumbnail settings:" << std::endl;
        std::cout << "  Max width:" << cfg.thumbnails.maxWidth << std::endl;
        std::cout << "  Max height:" << cfg.thumbnails.maxHeight << std::endl;
    }

    if (!cfg.sharedAlbums.empty())
        std::cout << "Shared albums:" << std::endl;
    for (const auto& album : cfg.sharedAlbums){
        std::cout << "  Album:" << album.name << std::endl;
        std::cout << "    Hosts:" << std::endl;
        for (const auto& host : album.hosts)
            std::cout << "      " << host.ip << ":" << host.port << std::endl;
    }

    if (!cfg.envVars.empty()){
        std::cout << "Environment:" << std::endl;
        for (const auto& [name, value] : cfg.envVars)
            std::cout << "  " << name << ":" << value << std::endl;
    }
}