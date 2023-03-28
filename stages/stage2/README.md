# README

This repository is designed to provide you with an opportunity to learn how to work with QEMU, bare-metal, u-boot, and kernel. Through a series of stages, you'll learn how to run applications on QEMU and gain a deeper understanding of the bare-metal programming process.

## Stage 2 - Piece of Cake (![](../../resources/star.png) ![](../../resources/bstar.png) ![](../../resources/bstar.png))

Running binary at specific address on QEMU.

### Introduction

Alright, looks like you're getting the hang of this... or are you just a lucky guesser?
Now it's time to get this binary (`.bin`) file to dance on the machine!
And as for that ELF file, well, let's just say it's as functional as a toothbrush for a fish.
So let's see what you've got!

### How It Works

This stage contains a simple bare-metal application that is compiled for the versatilepb machine. But, before running this program, the trainee needs to figure out it's entry point. Once they know it starts from address zero, they need to know how to make qemu start it from there. After that, the program will print out the secret code for the next stage. The ELF file was corrupted to prevent its execution on the machine.

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

1. Run the following command in your terminal (to find the entry point of the binary):

    ```
    $ arm-none-eabi-readelf -l piece_of_cake
    ```

2. Run the following command in your terminal:

    ```
    $ qemu-system-arm -M versatilepb -m 128 -nographic -device loader,file=piece_of_cake.bin,addr=0x00000000
    ```
