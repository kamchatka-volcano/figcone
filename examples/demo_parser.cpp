#include "demo.h"
#include <figcone_tree/iparser.h>
#include <figcone_tree/tree.h>
#include <figcone_tree/errors.h>

class DemoTreeProvider : public figcone::IParser
{
    figcone::TreeNode parse(std::istream& stream) final
    {
        auto tree = figcone::makeTreeRoot();
        tree.asItem().addParam("rootDir", "~/Photos");
        tree.asItem().addParamList("supportedFiles", {".jpg", ".png"});

        auto& thumbNode = tree.asItem().addNode("thumbnails");
        thumbNode.asItem().addParam("enabled", "1");
        thumbNode.asItem().addParam("maxWidth", "128");
        thumbNode.asItem().addParam("maxHeight", "128");

        auto& albumsNodeList = tree.asItem().addNodeList("sharedAlbums");
        auto& albumNode = albumsNodeList.asList().addNode();
        albumNode.asItem().addParam("dir", "summer_2019");
        albumNode.asItem().addParam("name", "Summer (2019)");
        auto& hostsNodeList = albumNode.asItem().addNodeList("hosts");
        auto& hostNode = hostsNodeList.asList().addNode();
        hostNode.asItem().addParam("ip", "127.0.0.1");
        hostNode.asItem().addParam("port", "80");

        //For error notifications use figcone::ConfigError exceptions
        if (stream.bad())
            throw figcone::ConfigError{"stream error"};

        //Stream position information can be added to objects
        auto pos = figcone::StreamPosition{13, 1};
        auto& envVarsNode = tree.asItem().addNode("envVars", pos);
        envVarsNode.asItem().addParam("DISPLAY", "0.1");

        //and to ConfigError exceptions
        if (stream.fail())
            throw figcone::ConfigError{"stream error", pos};

        return tree;
    }
};

int main()
{
    auto cfg = PhotoViewerCfg{};
    auto parser = DemoTreeProvider{};
    cfg.read("", parser);
    std::cout << "Launching PhotoViewer in directory " << cfg.rootDir;
    return 0;
}