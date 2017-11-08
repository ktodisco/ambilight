# ambilight
## About
This code drives an ambilight setup for the back of a television set.  It is
designed to run on a Raspberry Pi which receives an input video signal and is
connected to an LED strip via SPI.

There are three test programs to be used to test the individual components of
the project.  `test_led` will write a solid color to the LED strip.  `test_video`
will decode and output the video signal received to a temporary window.  `test_ambilight`
will perform video decoding and frame analysis, outputting the video signal to
one window and in another window will display a visual representation of the
calculated LED colors.

## Getting Started
Download and build OpenCV 3.1.0 on your Raspberry Pi.  Warning: this is a lengthy
step.

Verify your setup with the code.  You may need to change the macro `LED_COUNT`
in `ambilight/main.cpp` and `test_led/main.cpp` to the number of LEDs in your rig.

## Building
To build the project, run `make rel` from the top level.  This will build the
release version of the code.  To build debug, simply run `make`.

## Installing
Copy `service/ambilight` to `/etc/init.d/`.  Then, set the service to run at
startup using the following command:

```
    sudo update-rc.d ambilight defaults
```

If you ever want to remove the ambilight service from startup, run:

```
    sudo update-rc.d -f ambilight remove
```
