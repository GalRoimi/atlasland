# README

This repository is designed to provide you with an opportunity to learn how to work with QEMU, bare-metal, u-boot, and kernel. Through a series of stages, you'll learn how to run applications on QEMU and gain a deeper understanding of the bare-metal programming process.

## Stage 3 - Child's Play (![](../../resources/star.png) ![](../../resources/bstar.png) ![](../../resources/bstar.png))

Debbuging ELF on QEMU.

### Introduction

Looks like someone thought this was going to be a piece of cake!
I hope you haven't been banging your head against the wall too hard, because we're about to take things up a notch...
Let's put your GDB skills to the test, shall we? 
This time around, you'll need to show that program who's boss and dig up the dirt on the variable "secret". 
Happy sleuthing!

### How It Works

This stage contains a simple bare-metal application that is compiled for the versatilepb machine. When the trainee runs the application, it will calculate the code needed to unlock the next stage and save it inside the global variable `secret`.

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
    $ qemu-system-arm -M versatilepb -m 128 -nographic -kernel childs_play -S -s
    ```

2. Run the following command in differnet terminal (to debug the application):

    ```
    $ arm-none-eabi-gdb childs_play -ex "target remote localhost:1234"
    ```

    - Run the following command in GDB:

        ```
        $ b main
        ```

    - Run the following command in GDB:

        ```
        $ c
        ```

    - Run the following command in GDB (6 times):

        ```
        $ n
        ```

    - Run the following command in GDB:

        ```
        $ p/x secret
        ```
