# Sauron

Sauron is an open source project for converting your phone, camera, or telescope, into a robotic observatory system. Currently it's in a very early stage.

## Goal
Our goal is to design an easy-to-build telescope mod kit for astrophotography fans. By leveraging state-of-art technologies, we don't need expensive hardware or a lot of manual works.

* **Generic**. Compatible to different telescope systems, even non-telescopes.
* **Low cost**. Reuse your existing devices. Much cheaper than commercial computerized telescope.
* **High precision**. Using sensors and computer vision to improve precision.
* **Automatic**. Almost no manual alignment required. It can automatically point to astronomical objects you select.
* **Portable**. Lightweight and robost architecture, you can move it to best observation locations.

## Components
To build a full robotic telescope system, you need components listed below. Items in *italics* is not a part of Sauron. If you alreay have some, just reuse them as much as possible.

### Mount
* *Altazimuth*,
* 2 Servo motors, control altitude and azimuth
* 2 Gearboxes to transfer rotation from motors to altazimuth
* 9 axis sensor: accelerometer, gyroscope, and magnetic field sensors
* A GPS module provides locations
* Arduino for driving the mount

TBD

* WiFi/bluetooth module?
* Support equatorial mount?

### Finderscope
* *A smart phone or a web camera*
* A mount to attach phone to the telescope
* 5x zoom lens

### Telescope
* *Main telescope*
* *Main camera, can be DSLR, or web camera*
* *Adapter to attach camera to telescope*

### Computer
* *A PC, tablet, or phone*. The finderscope and mount are connected to computer.

### Softwares
* *Existing planetarium software, such as [Stellarium](http://stellarium.org/)*
* A driver set between the mount and the software
  * Supports ASCOM interface
  * Sensor fusion for higher precision
  * Computer vision algorithms to do automatic calibration and alignment

## Contributing
As an open source project, Sauron benefits greatly from both the volunteer work of helpful developers and good bug reports made by users.

### Bug Reports & Feature Requests
If you've noticed a bug or have an idea that you'd like to see come real, why not work on it? Bug reports and feature requests are typically submitted to the issue tracker https://github.com/gongminmin/Sauron/issues.

## Why this name
Remember the Eye of Sauron from The Lord of the Rings?
