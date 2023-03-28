# README

This repository is designed to provide you with an opportunity to learn how to work with QEMU, bare-metal, u-boot, and kernel. Through a series of stages, you'll learn how to run applications on QEMU and gain a deeper understanding of the bare-metal programming process.

## Stage 6 - Butterfingers (![](../../resources/star.png) ![](../../resources/bstar.png) ![](../../resources/bstar.png))

Fixing vector table location in bare-metal code.

### Introduction

Oh boy, I've got some news that might burst your bubble - the last step was just a warm-up!
But fear not, my dear friend, for now it's time to put your newfound Challenge-Response protocol knowledge to the test and get cracking once again!

Uh-oh, I seem to have misplaced the binary for the program.
It's like trying to find a needle in a haystack, but luckily we've got our trusty development environment to help us rebuild it from scratch!

Hang on a sec, something's not quite right here... Did a tornado just hit our code?
We were only able to recover some of the source files for the environment, and for the damaged files, all we could salvage were their objects files.

Do you think you're up for the challenge of compiling the program and getting it to run?
Let's hope so, because this time you'll need to make it run from address 0x10000.
It's like a game of "Where's Waldo", but with computer code!

### How It Works

This stage contains a simple bare-metal application that is compiled for the versatilepb machine. The trainee is required to write a Python script that implements the Challenge-Response protocol over serial connection.
Afterward, the trainee should execute the machine and run the script via the secondary serial connection, where the secret for the next stage will be printed out upon completion of the protocol. It is important to note that the application prints the HMAC key at the beginning of execution.

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
    import hmac
    import serial
    import struct
    import argparse

    def main():
        parser = argparse.ArgumentParser(description='Connect to a serial device, send opcode 0xaa, receive 4 bytes '
                                                    '(called NONCE) in big-endian order, convert them to little-endian '
                                                    'order, calculate HMAC-SHA256 on these bytes with the secret "hello", '
                                                    'and send the resulting hash back to the device.')
        parser.add_argument("port", help="serial port to connect to")
        args = parser.parse_args()

        try:
            # Establish serial connection to device
            with serial.Serial(args.port) as ser:
                # Send opcode 0xaa to device
                ser.write(bytes([0xaa]))
                ser.flush()

                # Receive 4 bytes (called NONCE) from device in big-endian order
                nonce = ser.read(4)

                # Convert from big-endian to little-endian order
                nonce_le = int.from_bytes(nonce, byteorder='big')

                # Calculate HMAC-SHA256 hash of little-endian nonce using secret "hello"
                secret_key = b'super pluper secret...'
                hmac_obj = hmac.new(secret_key, nonce_le.to_bytes(4, byteorder='little'), digestmod='sha256')
                hmac_digest = hmac_obj.digest()

                # Send HMAC back to device
                ser.write(hmac_digest)
                ser.flush()

        except serial.SerialException as e:
            # Handle serial connection errors
            print(f"Serial connection error: {e}")
        except Exception as e:
            # Handle other errors
            print(f"Error: {e}")
        finally:
            # Ensure serial connection is closed
            ser.close()


    if __name__ == '__main__':
        main()
    ```
2. Run the following command in your terminal to launch QEMU:

    ```
    $ qemu-system-arm -M versatilepb -nographic -kernel butterfingers -serial mon:stdio -serial pty
    ```

3. Run the following command in your terminal to execute the script:

    ```
    $ python3 solution.py {pts}
    ```
