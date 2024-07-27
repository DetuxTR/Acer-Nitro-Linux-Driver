# Acer-Nitro-Linux-Driver
An experimental driver to use acer gaming wmi functions in linux.

#⚠️Warning⚠️

This driver is experimental, dont use if you dont know what you are doing. I'm not responsible for melted plastics,broken cpus/gpus,bricked bioses, kernel panics. You are at your own.

Compiling:

You need your kernels linux-headers package. Just run command make to compile the module.

Running(root access needed):

insmod acer-nitro-driver.ko

Usage:



To set your cpu fan to manual fan control mode you should type:

echo 1-0-768 > /dev/acer-nitro (⚠️Warning⚠️ Your fan speed will be 0 rpm)

To set cpu fan speed:

echo 0-1-{fan speed,should be a multiple of 256}

To set your gpu fan to manual fan control mode you should type:

echo 1-1-49152 > /dev/acer-nitro (⚠️Warning⚠️ Your fan speed will be 0 rpm)-If this not works try set 16384

To set gpu fan speed:

echo 0-4-{fan speed,should be a multiple of 256}


Maximum fan speeds can be changed on every machine, just try multiples of 256.

If you get Input value should be integer error in dmesg try reload the module.

Normally I should be able get fan behaviors by passing method id 15 to wmi interface but it wasnt work. This very important because of for first time working this driver it should set fan mode automatically to make a good user friendly gui app. This is the biggest problem on this driver.

Good luck.
