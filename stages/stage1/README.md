# README

This repository is designed to provide you with an opportunity to learn how to work with QEMU, bare-metal, u-boot, and kernel. Through a series of stages, you'll learn how to run applications on QEMU and gain a deeper understanding of the bare-metal programming process.

## Stage 1 - Baby Steps (![](../../resources/star.png) ![](../../resources/bstar.png) ![](../../resources/bstar.png))

Running ELF on QEMU.

### Introduction

Well, well, well, look who's here! Ready to rock and roll with the first stage? Let's see if you can work your magic and get this program up and running... but uh-oh, looks like it's not working on my machine :( Maybe it's just being a drama queen and doesn't like playing with non-versatilepb devices.

### How It Works

This stage contains a simple bare-metal application that is compiled for the versatilepb machine. When the trainee runs the application, it will print out that the address of the '.secret' section is the code needed to unlock the next stage.

### Usage

To build the stage, run the following command in your terminal:
```
    $ make setup
```

To clean up the environment and remove all generated files, run the following command:
```
    $ make clean
```

### Solution

To solve this stage, follow these steps:

1. Run the following command in your terminal:

    ```
    $ qemu-system-arm -M versatilepb -m 128 -nographic -kernel baby_steps
    ```

2. Run the following command in your terminal (to find the address of `.secret` section):

    ```
    $ arm-none-eabi-objdump -h -j .secret baby_steps
    ```
