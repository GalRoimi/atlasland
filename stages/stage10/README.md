# README

This repository is designed to provide you with an opportunity to learn how to work with QEMU, bare-metal, u-boot, and kernel. Through a series of stages, you'll learn how to run applications on QEMU and gain a deeper understanding of the bare-metal programming process.

## Stage 10 - Pusing Through (![](../../resources/star.png) ![](../../resources/bstar.png) ![](../../resources/bstar.png))

RE and patching bare-metal application.

### Introduction

Hey there! Are you prepared for an unexpected challenge that lies ahead?
Let's dive deep into the mysterious world of reverse engineering... 
Does it make your heart skip a beat? Hahaha, oh, it's totally justified!

Um, do you remember the previous stage? (Duh... it was just a hot minute ago),
Well, turns out our little app has a mischievous side and can pull off a mind-boggling feat — running directly on our 'versatilepb' platform.
Yep, you heard it right, no need for that ol' bootstrap jazz!

Now, I bet you're dying to know, "How on earth is that even possible...?" 
Well, my friend, we won't be revealing that juicy secret just yet.

Let's see you work your magic and make this app dance on our outdated platform! 
Frankly, I have my doubts if you'll succeed...

In other words, you need to give that binary file a little makeover so it can waltz its way to compatibility.
Get ready for a wild ride!

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
                secret_key = b'the last one...'
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
    $ qemu-system-arm -M versatilepb -nographic -kernel pushing_through -serial mon:stdio -serial pty
    ```

3. Run the following command in your terminal to execute the script:

    ```
    $ python3 solution.py {pts}
    ```
