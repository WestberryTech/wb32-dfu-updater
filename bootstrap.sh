#!/bin/bash

function execute_cmd() {
    echo "$1"
    $1

    if [ $? -ne 0 ] ; then
        echo -e "\n$1 execution error !!!!!!"
        echo -e "Please handle the error and retry."
        exit 1
    fi
}

function downloadLibusb() {
    execute_cmd "wget https://github.com/libusb/libusb/releases/download/v1.0.24/libusb-1.0.24.tar.bz2"
    execute_cmd "tar -xjf ./libusb-1.0.24.tar.bz2"
    execute_cmd "cd libusb-1.0.24"
    execute_cmd "bash ./configure"
    execute_cmd "make"
    execute_cmd "make install"
    execute_cmd "make clean"
    execute_cmd "cd .."
    execute_cmd "rm -rf libusb-1.0.24*"
    execute_cmd "rm -rf libusb-1.0.24.tar.bz2* "
}

function findPkg() {

    for find_path in /usr/ /home/ /mingw64/
    do
        if [ ! $LIBUSB_INCLUDE_DIRS ] ; then
            LIBUSB_INCLUDE_DIRS=`find $find_path -name 'libusb-1.0' -type d | grep include | head -n 1`
            # echo "LIBUSB_INCLUDE_DIRS=$LIBUSB_INCLUDE_DIRS"
        fi

        for lib_name in 'libusb-1.0.so' 'libusb-1.0.dylib' 'libusb-1.0.dll'
        do

            LIBUSB_LIBRARIES=`find $find_path -name $lib_name | head -n 1`
            if [ $LIBUSB_LIBRARIES ] && [ $LIBUSB_INCLUDE_DIRS ] ; then
                # echo "LIBUSB_LIBRARIES=$LIBUSB_LIBRARIES"
                return 0
            fi
        done
    done

    return 1
}

function checkLibusb() {

    findPkg

    if [ $? == 0 ] ; then
        echo -e "--------------Have found libusb--------------"
        echo -e "LIBUSB_INCLUDE_DIRS=$LIBUSB_INCLUDE_DIRS"
        echo -e "LIBUSB_LIBRARIES=$LIBUSB_LIBRARIES"
    else
        echo -e "Not found libusb!"
        echo -e "1. Install the libusb."
        echo -e "2. Enter the libusbd path."
        echo -e "3. Quit."
        read -n1 -p "Please select that: " input_cmd
        echo

        case $input_cmd in

            1)
                echo -e "\nThe installation of Libusb starts!"
                downloadLibusb
                return 0 ;;
           2)
                while true
                do
                    echo -n "LIBUSB_INCLUDE_DIRS= "
                    read LIBUSB_INCLUDE_DIRS

                    if [[ $LIBUSB_INCLUDE_DIRS != */libusb-1.0* ]] ; then

                        echo "LIBUSB_INCLUDE_DIRS is incorrect. Please re-enter or press CTRL + C to exit: $LIBUSB_INCLUDE_DIRS"
                        continue
                    fi
                    break;
                done

                while true
                do
                    echo -n "LIBUSB_LIBRARIES= "
                    read LIBUSB_LIBRARIES

                    if [[ $LIBUSB_LIBRARIES != */libusb-1.0.* ]] ; then
                        echo "LIBUSB_LIBRARIES is incorrect. Please re-enter or press CTRL + C to exit:  $LIBUSB_LIBRARIES"
                        continue
                    fi
                    break
                done
                
                return 0 ;;
            3)
                echo -e "\nQuit!"
                exit 1 ;;
            *)
                echo -e "\nAbnormal exit!"
                exit 2 ;;
        esac
    fi
}

function findBuild(){
    if [ ! -d "./build" ];then
        mkdir ./build
    else
        rm -rf build/
        mkdir ./build
    fi
    
    cd build
}

function buildProject() {

    if [ $(uname -s) == "Linux" ] ; then

    cmake .. -DCMAKE_BUILD_TYPE=Release -DLIBUSB_INCLUDE_DIRS=$LIBUSB_INCLUDE_DIRS -DLIBUSB_LIBRARIES=$LIBUSB_LIBRARIES

    elif [ $(uname -s) == "Darwin" ] ; then 

    cmake .. -DCMAKE_BUILD_TYPE=Release -DLIBUSB_INCLUDE_DIRS=$LIBUSB_INCLUDE_DIRS -DLIBUSB_LIBRARIES=$LIBUSB_LIBRARIES

    elif [[ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]] || [[ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]] ; then

    cmake .. -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles" -DLIBUSB_INCLUDE_DIRS=$LIBUSB_INCLUDE_DIRS -DLIBUSB_LIBRARIES=$LIBUSB_LIBRARIES
        
    else

    echo "The system version does not support compilation!"

    fi #ifend

    cmake --build .
}

function installProject() {
  cmake --install .
}

function cmakePackage(){
    cpack --config ./CPackSourceConfig.cmake
}

function readLogFile(){
    if [ -e "logfile" ];then
        package_name=`sed -n '$p' logfile |sed -n "s/.*CPack:.*\/\(.*tar\.gz\).*generated./\1/gp"`
        package_version=`echo "${package_name}" | sed -n 's/.*v\(.*\)\-.*/\1/gp' |sed -n 's/\(.*\)\-.*/\1/gp'`
        fileSHA256=`openssl dgst -sha256 ../package/${package_name}|sed -n "s/.*= \(.*\)/\1/gp"`
        echo "---name: ${package_name}"
        echo "---version: ${package_version}"
        echo sha256 \""$fileSHA256"\" | tee -a ./logfile
    else
        echo "file not found"
    fi
}

function PackagingProject() {
    if [ ! -e "CPackSourceConfig.cmake" ];then
        echo "The package file was not found. Please re-execute this script!!!"
    else
        echo "The package file has been generated and about to start packing." 
        name=`cmakePackage`
        echo "------------------------------"
        echo "$name"| tee -a ./logfile
    fi

    readLogFile
}

function commitToBrew(){
    if [ ! -n "$package_name" ];then
        echo "Cannot submit to brew."
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

checkLibusb
findBuild
buildProject
while [ $# -gt 0 ]
do
    key="$1"
    case $key in
        packing)
            PackagingProject
            shift;;
        install)
            installProject
            shift;;
        *)
        exit 1 ;;
    esac
done

# commitToBrew

cd ..

# read -n1 -p "Press Enter key to quit!"
