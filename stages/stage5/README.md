# README

This repository is designed to provide you with an opportunity to learn how to work with QEMU, bare-metal, u-boot, and kernel. Through a series of stages, you'll learn how to run applications on QEMU and gain a deeper understanding of the bare-metal programming process.

## Stage 5 - No Sweat (![](../../resources/star.png) ![](../../resources/bstar.png) ![](../../resources/bstar.png))

Running binary with serial communication on QEMU.

### Introduction

Well, well, well, looks like you're up for another challenge!
And this time, it's going to take some serious communication skills to get through.
You'll need to run the program on the machine and communicate with it serially using a special 'Challenge-Response' protocol. No pressure, right?

Oh, and just a heads up, the machine has 4 serial ports.
The first port is used for i/o, the second port is for the protocol, and the other ports are not in use.
Good luck!

Oh my, before I completely forget - we really must execute the binary from address zero!

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
                secret_key = b'super duper secret...'
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
    $ qemu-system-arm -M versatilepb -nographic -device loader,file=no_sweat.bin,addr=0x00000000 -serial mon:stdio -serial pty
    ```

3. Run the following command in your terminal to execute the script:

    ```
    $ python3 solution.py {pts}
    ```
