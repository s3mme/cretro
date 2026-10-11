#!/usr/bin/env bash

echo "cppcheck"
cppcheck src/*

echo "scan-build"
scan-build make 

