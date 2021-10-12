#!/bin/bash

rm -rf build/
mkdir build
cd build

if [ $(uname -s) == "Linux" ] ; then

cmake ../source/wb32-dfu-updater_cli


elif [ $(uname -s) == "Darwin" ] ; then 

cmake ../source/wb32-dfu-updater_cli

elif [[ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]] || [[ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]] ; then

cmake -G "MinGW Makefiles" ../source/wb32-dfu-updater_cli

fi #ifend

make
cp ../source/wb32-dfu-updater_cli/build/wb32-dfu-updater_cli* ./
make clean
cd ..

echo "press any key quit!"
read -n 1





