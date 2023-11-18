# CSE 167 HW3

## Optimization I done:
1. for 3_3, When press R, reset the camera to the initial position
2. code refactoring


## Normal Part
1. using the same way as write up instruct to run
2. pre-rendering image is in `output` folder
3. run the scene I create with `./balboa -hw 3_5 file_name`



### Notes: Need to change `cameraSpeedFactor` and `sensitivity` in `MyCamera.h` to get proper speed.
```c++
const float cameraSpeedFactor = 150.0f;
const float sensitivity = 0.1f;  // Adjust sensitivity as needed
```

### Notes: Need to change `hw_3_4_extra` to true in `hw3.cpp` to run extra credit for hw_3_4
```c++
const bool hw_3_4_extra = false;
```

### Note: Call `./balboa -hw 3_3_extra` to run extra credit for hw_3_3

## Extra Credit
1. mouse control (the ouput is combined with output for hw_3_3)
2. light animation (hw_4_name_animation.mp4)
3. texture




