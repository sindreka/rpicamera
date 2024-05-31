# RPICamera:
A C++ example of how to use the new libcamera library on raspberry pi and a raspberry pi camera.
This has been tested on raspberry pi 5 bookworm, with raspberry pi wide angle camera module v3.
## Release notes:
* When compiled, the demo produces 10 grayscale PGM files with resolution 320x180 pixels. View them in you favorite online PGM viewer.
* The wide angle image sensor on raspberry pi camera module v3 is only used with sufficiently high resolution (1920x1080). If lower resolution is required, a downsizing algorithm is needed.
## Usage:
demo.cpp shows a minimalistic way of taking and saving images. Copy the code, edit it to your needs and enjoy!


## Compiling:
Compiling is done by gcc:
    gcc...
	
## To do:
* Add compile 
* release notes
* final tests
* Add sources

## Motivation:
I found no easy to use example on how to use the new libcamera library to easily capture images, and accessing pixel values. Thus this example was born.
