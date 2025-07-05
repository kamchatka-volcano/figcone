#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR/..

release_dir=release
mkdir -p $release_dir/formats

cp build*/seal_lake_*/seal_lake.cmake release_pack/figcone_deps
cp build*/seal_lake_*/CPM.cmake release_pack/figcone_deps

cd release_pack/figcone_deps
cmake -S .
cd ../../
cp -r release_pack/figcone_deps/deps $release_dir

cp build*/seal_lake_*/seal_lake.cmake release_pack/figcone_formats_deps
cp build*/seal_lake_*/CPM.cmake release_pack/figcone_formats_deps

cd release_pack/figcone_formats_deps
cmake -S .
cd ../../
cp -r release_pack/figcone_formats_deps/deps $release_dir/formats

cp release_pack/CMakeLists.txt $release_dir
cp release_pack/formats/CMakeLists.txt $release_dir/formats

cp build*/seal_lake_*/seal_lake.cmake $release_dir
cp build*/seal_lake_*/CPM.cmake $release_dir
cp -r include $release_dir
cp LICENSE.md $release_dir
cp README.md $release_dir