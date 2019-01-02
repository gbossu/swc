# SWC (Sliding Window Container)
SWC is a window-embeder (and probably much more in the near future). Its job is to embed windows in a Qt container, so that the window can be slided in and out of the screen.


## Using swc
Syntax is as follow:
```
# Create a new container with a chosen swc-id
swc <input option> [options] <swc-id>

# Toggle an existing container (slide in or out)
swc <swc-id>
```

### Input options
You can either use an existing window as input, or create a new process
- window-id of an existing window (```-w <WID>```)
- pid of an existing process (```-p <PID>```)
- className/class of an existing window (```-c/-C <class>```) 
- window name of an existing window (```-n <windowName>```)
- create a process from a binary file. A classname can be given to help the window lookup. (```-b "<binaryFile> [parameters]" [-c <className>]```)

### Other options
- geometry (```-g x,y[,widht,height]```)
- pause the process when the container is hidden (```-P```)
- sliding direction (```-s u|d|r|l```)


## Configuration file **(Not implemented yet)**
More options can be adjusted in the configuration file.

### Container placement
- position
- anchor
- forcedSize
- immovable

### Container decorations
- curved corners
- transparency
- blur

### Animation properties
- direction
- duration
- easing
- displayTime

### Window Hints
- skipWindowManager
- alwaysOnTop

### Launch properties
- keepWindow
- exitAndRelease
- exitAndRemove
- containerClassName
