# Acer-Nitro-Linux-Driver
An experimental driver to use acer gaming wmi functions in linux.
⚠️Warning⚠️
This driver is experimental, dont use if you dont know what you are doing. I'm not responsible for melted plastics,broken cpus/gpus,bricked bioses, kernel panics. You are at your own.

Compiling:
You need your kernels linux-headers package. Just run command make to compile the module.
Running:
insmod acer-nitro-driver.ko
Usage:
Firstly type su . Driver is just running on root account beacuse of i dont set the udev rules yet.
To set your cpu fan to manual fan control mode you should type:
echo 1-0-768 > /dev/acer-nitro (⚠️Warning⚠️ Your fan speed will be 0 rpm)
To set cpu fan speed:
echo 0-1-{fan speed,should be a multiple of 256}
To set your gpu fan to manual fan control mode you should type:
echo 1-1-49152 > /dev/acer-nitro (⚠️Warning⚠️ Your fan speed will be 0 rpm)
To set gpu fan speed:
echo 0-4-{fan speed,should be a multiple of 256}

Maximum fan speeds can be changed on every machine, just try multiples of 256.
If you get Input value should be integer error in dmesg try reload the module.

Good luck.
