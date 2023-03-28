# README

This repository is designed to provide you with an opportunity to learn how to work with QEMU, bare-metal, u-boot, and kernel. Through a series of stages, you'll learn how to run applications on QEMU and gain a deeper understanding of the bare-metal programming process.

## Stage 4 - Easy Peasy (![](../../resources/star.png) ![](../../resources/bstar.png) ![](../../resources/bstar.png))

Running ELF with flash image on QEMU.

### Introduction

Oh boy, it's you again! You must really have a thing for punishment, my friend.
This time around, the program you're gonna need to run on the ol' machine has some pretty pesky demands.

In order to get the secret for the next stage, you'll need to provide the machine with a "flash memory" in the following structure:
* At address 0x000000dd, you gotta throw in the value "0x3294adee".
* At address 0x000012ab, you gotta plop in the value "0xaabbccdd".
* At address 0x000ba199, you gotta slap in the value "0x13141819".
* At address 0x008cc57b, you gotta fling in the value "0xff93532d".

Don't worry, it's not rocket science. It's just memory science.
Good luck!

### How It Works

This stage contains a simple bare-metal application that is compiled for the versatilepb machine. The trainee is required to generate a flash image, sized at 64 bytes, containing four values at specific offsets. Afterward, the trainee should execute the machine with the memory image he has created, where the secret for the next stage will be evaluated during runtime based on those values and printed out.

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

1. Create a Python script named `solution.py` with the following code:

    ```
    import struct

    def main():
        # Create a 64MB byte array
        image = bytearray(64 * 1024 * 1024)

        # Define the values and offsets in little-endian format
        values = [0x3294adee, 0xaabbccdd, 0x13141819, 0xff93532d]
        offsets = [0x000000dd, 0x000012ab, 0x000ba199, 0x008cc57b]

        # Write the values at the specified offsets
        for i, value in enumerate(values):
            offset = offsets[i]
            packed_value = struct.pack("<I", value)
            for j, byte in enumerate(packed_value):
                image[offset + j] = byte

        # Write the image to disk
        with open("flash.img", "wb") as f:
            f.write(image)

    if __name__ == "__main__":
        main()
    ```

2. Run the following command in your terminal to execute the script:

    ```
    $ python3 solution.py
    ```

3. Run the following command in your terminal to launch QEMU:

    ```
    $ qemu-system-arm -M versatilepb -m 128 -nographic -kernel easy_peasy -drive if=pflash,file=flash.img,format=raw
    ```
