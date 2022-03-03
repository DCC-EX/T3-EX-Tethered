# T3-Ex-Tethered
### Arduino based throttle for DCC++Ex

#### ⚠ This is Beta code and may have unknown bugs, please report any issues on GitHub ⚠

#### It uses off the shelf parts but will require a degree Tinkerer knowledge to build your own. This document contains my own wiring methods, I'm not liable for any damage you cause from incorrect wiring.

## Parts
 - Arduino Mega
 - [Adafruit 2.8" TFT Touch Shield - Capacitive](https://learn.adafruit.com/adafruit-2-8-tft-touch-shield-v2)
 - Rotary Encoder Board KY-040
 - Jumper wire
 - 4 core cable
 - 8GB microSD card (not required but needed for custom loco configs)

**\*NOTES\***
I've tried to keep it as simple as possible but it will require some 3D printed parts (STEP & STL included).
Not all KY-040 boards are created equal so it's best to check dimensions before doing any printing.

## Wiring
The KY-040 encoder needs to be wired as follows
|KY-040|Mega|
|--|--|
|GND|GND|
|+|5v|
|SW|A12|
|DT|18|
|CLK|19|

### Power
The throttle can be powered via the 5v pin with a regulated 5v supply or if your CS only has a motor shield you can get 5v from that. Remember to connect a ground to the CS from the throttle.
**\*WARNING\***
Don't connect the throttle via USB at the same time as being powered by the 5v pin.

### Connecting to the CS
The throttle uses `Serial2` (Pins 16 & 17), you'll need to connect these to a Serial port on the CS (If the CS is a Mega there's 3 to choose from).
Make sure you connect the GND's of both the throttle and CS together too.

**Command Station v4**
CS v4 made it much simpler to add an extra `Serial` input.
You just need to add which serial to use to your `config.h` file, e.g.
`#define SERIAL1_COMMANDS`
**Command Station v3.1**
The CS sketch will need a couple lines of extra code to handle the throttle.
Just before `void setup()` you'll need to create a new parser object, `DCCEXParser serialParser2;`.
In `void setup()` you'll need to start the serial port you decide to use, e.g. `Serial1.begin(115200);` then in `void loop()` you'll need to tell the serial parser to handle the serial, e.g. `serialParser2.loop(Serial1);`

## SD card structure
```
.
├── locos
│   ├── 1.json
│   └── 2.json
├── icons
│   ├── lights-on.bmp
│   └── lights-off.bmp
├── fns
│   ├── hornby.json
│   └── dcc-concepts.json
└── groups.json
```

## Loco config
The JSON file name is the loco address, e.g. `1234.json` has the name and function mapping for a loco on address #1234.

```js
{
  /* loco name/description, max 20 characters */
  "name": "00000000000000000000",
  /* decoder functions */
  "functions": [
    /* function buttons are in rows, a new array starts a new row. max 4 buttons per row. */
    [
      {
        /* function # */
        "fn": 0,
        /* Optional: button label, max 20 characters, *NOTE* 20 is the max but if you have multiple buttons on a row the text may overflow */
        "label": "Lights",
        /* Optional: is the button latching? default is true */
        "latching": true,
        /* Optional: button styles, if not specified it defaults to black and white */
        "btn": {
          /* idle button style */
          "idle": {
            /* fill colour, RGB */
            "fill": 1,
            /* text colour, RGB */
            "text": 1,
            /* icon file name, needs to be in the icons directory, the bmp file extension isn't needed as all icons need to be bmp images */
            "icon": "lights-off"
          },
          /* pressed button style */
          "pressed": {
            /* fill colour, RGB */
            "fill": 1,
            /* text colour, RGB */
            "text": 1,
            /* icon file name, needs to be in the icons directory, the bmp file extension isn't needed as all icons need to be bmp images */
            "icon": "lights-on"
          }
        }
      },
      {
        "fn": 2,
        "label": "Horn",
        "latching": false
      }
    ]
  ]
}
```
You can create a reusable function map file for specific decoders to save duplicating, e.g. `dcc-concepts.json`.
This needs to be in the `fns` directory and its name used as the value for the `functions` key, e.g. `"functions":"dcc-concepts"`.

These configs only need the rows and function declarations.
e.g.
```js
[
  [
    {
      "name": "Function 1",
      "fn": 1,
      // ...
    },
    {
      "name": "Function 2",
      "fn": 2,
      // ...
    }
  ]
]
```

## Icons
Icons need to be 24bit bmp images with max dimensions of 30x30.
As bmp's don't have opacity you'll need to set the background to the same colour you use for the fill.

## General functionality
A loco can be acquired by using the `By Address`, `By Name` or `Favs` buttons.

**By Address** will display a keypad where the loco address can be entered.
**By Name** will list detected loco names from the json configs. Touching a name button will acquire the loco (*\*The order is unspecified and may change with any changes to the SD card\**).
**Groups** allows you to create named loco groups which will list loco names in the order specified by the `groups.json` config. The `groups.json` file should contain a JSON object where the key is the group name and the value should be an array of loco addresses, e.g.
```json
{
  "Group Name": [1,2,3,4],
  "Class 66": [5,6,7,8]
}
```

**Release** will release the currently acquired loco.

**Program**, this allows for reading and writing CV's. A keypad will be displayed to allow entering numeric CV values.

**Menu Icon**, touching this icon will show the menu, if a loco has been acquired touching again will take you back to the loco display so you don't need to reselect it.

**Rotary Encoder**, clockwise rotation will increase the current loco speed and anti-clockwise rotation will decrease the loco speed.
A press of the rotary encoder will change the current loco direction.

**Emergency Stop**, press and hold the rotary encoder for 2+ seconds and all active locos will stop.
