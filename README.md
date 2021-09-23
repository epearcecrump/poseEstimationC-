### Installation of OpenCV and other libraries to compile the code:

1) To install OpenCV using Homebrew on MacOS, open a terminal and paste:

`brew install opencv`

2) Install `pkg-config` which will help insert the correct compiler flags on the command line and find the correct linker flags for OpenCV

`brew install pkg-config`

3) To view the linker flags for OpenCV, execute the following:

`pkg-config --cflags --libs opencv`

### How to call the program using the command line arguments:

To obtain help in executing the program, run:
```
./main -h
./main --help
```

Examples of calling the program from the command line with inputs:

1) to pass in an input image file named `input.jpg` and output it to a file called `out.jpg`:

`./main -p=input.jpg -o=out.jpg`

2) to open the camera on your local device, don't pass in any file under -p, just leave it blank.

### To terminate the program whilst it is running:

Press 0 on your keyboard.
