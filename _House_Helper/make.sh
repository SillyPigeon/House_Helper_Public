#!/bin/bash
source /opt/fsl-imx-x11/4.1.15-2.1.0/environment-setup-cortexa7hf-neon-poky-linux-gnueabi
mkdir arm_build
cd arm_build
qmake ../
make -j16
echo "OK complie is done"
