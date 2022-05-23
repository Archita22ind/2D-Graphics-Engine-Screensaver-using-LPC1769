# 2D-Graphics-Engine-Screensaver-using-LPC1769

In this project I have designed and built a system that displays a screen saver and forest on 1.8 inches diagonal LCD TFT display (128x160 resolution, 18-bit (262,144) color). The LPC1769 uses its SPI Interface to connect with the LCD.It has the MISO, MOSI and the SCK options and therefore is used as a master to interface with a SPI Flash which forms the slave peripheral.

## Demo

<img src="/images/demo.gif?raw=true" width="1000px">

## Hardware Design

The design prototype:

![](/images/designPrototype.png)

### Main Bill of material:

    LPCXpresso Module 1769(ARCortex-M3)
    Wire Wrapping Board
    Connecting wires
    Soldering rod
    USB cable
    1.8” TFT LCD

The design of this system focuses on implementing the master slave architecture required to carry out SPI communication. The LPCXpresso 1769 is an ARM Cortex-M3 based Microcontroller for embedded applications. The LCD Display module used supports serial peripheral interface(SPI) . The peripheral complement of the LPC1769 includes up to 512 kB of flash memory and up to 64 kB of data memory. It has a low power consumption and wide temperature ranges. The 1.8" display has 128x160 color pixels and it can display full 18-bit color (262,144 shades).
Here LPC1769 CPU module will act as the master whereas LCD module acts as the slave. The inputs to the LPC will be in the form MISO Master In Slave out) and that to the LCD module will have the form MOSI (Master out Slave in). The Serial Clock and the Chip Select logic are taken as inputs from the master.

The design schematic is as shown below:

![](/images/designschematics.png)

## Software Design

The software part of the intended system includes the LPCXpresso Integrated Development Environment (IDE) and the host computer to run it. The Liquid Crystal Display is configured via software commands given via the IDE. The display operations on LCD demands particular opcodes casted for the specific operations, setting the status registers, various initializations in LPC etc. Inclusion of particular header files is also essential for obtaining desirable results.
Also, we have used the dynamic of 2D vector graphics in our design of Screen savers.

## Result

Both the screen savers have been created using 2D graphic engine design and the important SPI interface between LPC 1769 and the LCD module. They have been implemented in the same .c file and a delay of 500 ms has been added in between the delay of both.

Final Screen saver of Squares:

<img src="https://github.com/Archita22ind/2D-Graphics-Engine-Screensaver-using-LPC1769/blob/main/images/SquareScreensaver.jpg" width=50% height=50%>

Final Forest Screen saver created:

<img src="https://github.com/Archita22ind/2D-Graphics-Engine-Screensaver-using-LPC1769/blob/main/images/ForestScreensaver.jpg" width=50% height=50%>
