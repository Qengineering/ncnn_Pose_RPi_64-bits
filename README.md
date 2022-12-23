# Pose Raspberry Pi 4
![output image]( https://qengineering.eu/github/Pose_NCNN.webp )<br/>
## Pose estimation with ncnn running at 7.0 FPS on bare Raspberry Pi 4.
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)<br/><br/>
A fast C++ implementation of person detection and pose estimation with the ncnn framework on a bare Raspberry Pi 4 64-bit OS.<br/>
Once overclocked to 1825 MHz, the app runs at 7.1 FPS without any hardware accelerator. Thanks [dog-qiuqiu](https://github.com/dog-qiuqiu/Ultralight-SimplePose) for all the hard work.<br/>
Special made for a Raspberry Pi 4 see [Q-engineering deep learning examples](https://qengineering.eu/deep-learning-examples-on-raspberry-32-64-os.html) <br/>

------------

Papers: https://arxiv.org/abs/1804.06208<br/>

------------

## Dependencies.
To run the application, you have to:
- A raspberry Pi 4 with a 32 or 64-bit operating system. It can be the Raspberry 64-bit OS, or Ubuntu 18.04 / 20.04. [Install 64-bit OS](https://qengineering.eu/install-raspberry-64-os.html) <br/>
- The Tencent ncnn framework installed. [Install ncnn](https://qengineering.eu/install-ncnn-on-raspberry-pi-4.html) <br/>
- OpenCV 64 bit installed. [Install OpenCV 4.5](https://qengineering.eu/install-opencv-4.5-on-raspberry-64-os.html) <br/>
- Code::Blocks installed. (```$ sudo apt-get install codeblocks```)

------------

## Installing the app.
To extract and run the network in Code::Blocks <br/>
$ mkdir *MyDir* <br/>
$ cd *MyDir* <br/>
$ wget https://github.com/Qengineering/ncnn_Pose_RPi_64-bits/archive/refs/heads/master.zip <br/>
$ unzip -j master.zip <br/>
Remove master.zip and README.md as they are no longer needed. <br/> 
$ rm master.zip <br/>
$ rm README.md <br/> <br/>
Your *MyDir* folder must now look like this: <br/> 
Dance.mp4 <br/>
person_detectord.bin <br/>
person_detectord.param <br/>
Ultralight-Nano-SimplePose.bin <br/>
Ultralight-Nano-SimplePose.param <br/>
ncnn_pose.cpb <br/>
ncnn_pose.cpp<br/>

------------

## Running the app.
Run ncnn_pose.cpb with Code::Blocks. More info or<br/> 
if you want to connect a camera to the app, follow the instructions at [Hands-On](https://qengineering.eu/deep-learning-examples-on-raspberry-32-64-os.html#HandsOn).<br/>
I fact you can run this example on any aarch64 Linux system.

------------

[![paypal](https://qengineering.eu/images/TipJarSmall4.png)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=CPZTM5BB3FCYL) 



