# Analog TV Tools project

The tools for decoding the analogue TV signals. <br>For the moment it is able to decode SECAM cvbs tv signal. NTSC and PAL is coming soon.

## Build

Prerequisites:
- vcpkg installed
- cmake installed
- VCPKG_INSTALLATION_ROOT environment variable should point to the vcpkg root folder.

### Windows build
- Visual Studio 2022 installed
### Ubuntu Build
Be sure you have installed all of those:
```
sudo apt-get update
sudo apt-get install autoconf automake pkg-config autoconf-archive
sudo apt-get install liburing-dev libx11-dev libxft-dev libtool bison gperf libgles2-mesa-dev libxext-dev libxrandr-dev libxi-dev 
sudo apt-get install libxcursor-dev libxdamage-dev libxinerama-dev libltdl-dev libxtst-dev doxygen gnuradio gnuradio-dev
sudo pip3 install pytest
```
##
Copyright &copy; 2025 Roman Ukhov

