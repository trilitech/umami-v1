#!/bin/bash

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac

if [ "${GITLAB_CI}" != "true" ]
then
    if [ "$machine" = "Linux" ]
    then
        yarn snap:build
    else
        yarn electron:build
    fi
fi
