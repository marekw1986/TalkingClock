# TalkingClock

## Overwiev
This is repository of firmware for Talking Clock project. 
It uses vintage parts, such as WDC65C02 CPU, M6242B RTC, MC6840 timer or R65C51 UART. 
It also consists SP0256-AL2 chip, providing retro speach synthesis capability.

This project is still in development.

## Required tools
You need [cc65](https://github.com/cc65/cc65) to build this project.
Additionaly there are two PLD files, consisting descriptions of address decoders for GAL chips used in this project. 
To synthesise them you need [GalAsm](https://github.com/daveho/GALasm).
