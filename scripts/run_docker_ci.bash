#!/bin/bash
result=${PWD##*/}
result=${result:-/}

pkg_directory="uavsdk"

if [ $result != $pkg_directory ]; then
    echo Please, launch the script in your uavsdk directory
else
    echo Current dir is $result. Proceeding...
    cd ..
    # cp -r uavsdk uavsdk-ci
    echo Using ${PWD}/uavsdk directory
    docker build uavsdk -t uavsdk-ci 
fi

