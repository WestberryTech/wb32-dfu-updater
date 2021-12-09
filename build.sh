#!/bin/bash

# 寻找build文件夹，存在就删除重建
function findBuild(){
    if [ ! -d "./build" ];then
        mkdir ./build
    else
        rm -rf build/
        mkdir ./build
    fi
    
    cd build # 进入Build文件夹
}

# 创建并编译
function buildProject() {
    if [ $(uname -s) == "Linux" ] ; then

    cmake ../source/wb32-dfu-updater_cli

    elif [ $(uname -s) == "Darwin" ] ; then 

    cmake ../source/wb32-dfu-updater_cli

    elif [[ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]] || [[ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]] ; then

    cmake *std_cmake_args -DCMAKE_INSTALL_PREFIX=D:/Desktop/ -G "MinGW Makefiles" ../source/wb32-dfu-updater_cli

    fi #ifend

    make
}

# 打包文件
function cmakePackage(){
    cpack --config ./CPackSourceConfig.cmake
}

# 读取日志文件，读入文件名称
function readLogFile(){
    if [ -e "logfile" ];then
        package_name=`sed -n '$p' logfile |sed -n "s/.*CPack:.*\/\(.*tar\.gz\).*generated./\1/gp"`
        package_version=`echo "${package_name}" | sed -n 's/.*v\(.*\)\-.*/\1/gp' |sed -n 's/\(.*\)\-.*/\1/gp'`
        fileSHA256=`openssl dgst -sha256 ../pack/${package_name}|sed -n "s/.*= \(.*\)/\1/gp"`
        echo "---name: ${package_name}"
        echo "---version: ${package_version}"
        echo sha256 \""$fileSHA256"\" | tee -a ./logfile
    else
        echo "file not found"
    fi
}

# 本地提交至brew仓库，远程提交
function commitToBrew(){
    if [ ! -n "$package_name" ];then
        echo "不能提交到brew"
    else
        # cd $(brew --repo rangaofei/saka)/Formula
        sed -i "s/wb32-dfu-updater_cli-v[0-9]\.[0-9]\.[0-9]/wb32-dfu-updater_cli-v${package_version}/g" ../wb32-dfu-updater_cli.rb
        sed -i "s/sha256 \".*\"/sha256 \"$fileSHA256\"/g" ../wb32-dfu-updater_cli.rb
        # git add wb32-dfu-updater_cli.rb
        # git commit -m "new version:$package_version"
        # git push
        # cd -
    fi
}

findBuild
buildProject
# cmakePackage

if [ ! -e "CPackSourceConfig.cmake" ];then
    echo "未找到打包文件，请重新执行此脚本"
else
    echo "已生成打包文件，即将开始打包"
    name=`cmakePackage`
    echo "------------------------------"
    echo "$name"| tee -a ./logfile
fi

readLogFile
# commitToBrew

# cp ../source/wb32-dfu-updater_cli/build/wb32-dfu-updater_cli* ./
# make clean

cd ..

echo "press any key quit!"
read -n 1





