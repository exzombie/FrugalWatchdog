EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:FrugalWatchdog-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Frugal Watchdog"
Date ""
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ATTINY45-P IC?
U 1 1 56233B6F
P 5400 3450
F 0 "IC?" H 4250 3850 40  0000 C CNN
F 1 "ATTINY45-P" H 6400 3050 40  0000 C CNN
F 2 "DIP8" H 6400 3450 35  0000 C CIN
F 3 "" H 5400 3450 60  0000 C CNN
	1    5400 3450
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X06 P?
U 1 1 56233C51
P 2850 3550
F 0 "P?" H 2850 3900 50  0000 C CNN
F 1 "CONN_01X06" V 2950 3550 50  0001 C CNN
F 2 "" H 2850 3550 60  0000 C CNN
F 3 "" H 2850 3550 60  0000 C CNN
	1    2850 3550
	-1   0    0    1   
$EndComp
Text Notes 3050 3800 0    60   ~ 0
GND
Text Notes 3050 3700 0    60   ~ 0
RXD
Text Notes 3050 3600 0    60   ~ 0
TXD
Text Notes 3050 3500 0    60   ~ 0
3V3
Text Notes 3050 3400 0    60   ~ 0
VCC
Text Notes 3050 3300 0    60   ~ 0
5V
Text Notes 2750 3200 0    60   ~ 0
Cheap Chinese\nUSB to TTL\nconverter
Wire Wire Line
	3050 3500 3250 3500
Wire Wire Line
	3250 3500 3250 3400
Wire Wire Line
	3050 3400 3500 3400
NoConn ~ 3050 3300
Wire Wire Line
	3050 3800 3050 4000
Wire Wire Line
	6750 3700 6750 4100
Wire Wire Line
	3500 2950 6750 2950
Wire Wire Line
	6750 2950 6750 3200
Connection ~ 3250 3400
Wire Wire Line
	3550 3200 4050 3200
Wire Wire Line
	3550 3600 3050 3600
Wire Wire Line
	4050 3700 4050 4100
Wire Wire Line
	3600 3300 4050 3300
Wire Wire Line
	3600 3700 3050 3700
$Comp
L LED D?
U 1 1 5623C3ED
P 3900 3800
F 0 "D?" H 3900 3900 50  0000 C CNN
F 1 "LED" H 3900 3700 50  0000 C CNN
F 2 "" H 3900 3800 60  0000 C CNN
F 3 "" H 3900 3800 60  0000 C CNN
	1    3900 3800
	0    -1   -1   0   
$EndComp
$Comp
L R R?
U 1 1 5623C436
P 3750 4000
F 0 "R?" V 3830 4000 50  0000 C CNN
F 1 "R" V 3750 4000 50  0000 C CNN
F 2 "" V 3680 4000 30  0000 C CNN
F 3 "" H 3750 4000 30  0000 C CNN
	1    3750 4000
	0    1    1    0   
$EndComp
$Comp
L CONN_01X01 P?
U 1 1 5623C469
P 3750 3500
F 0 "P?" H 3750 3600 50  0000 C CNN
F 1 "CONN_01X01" V 3850 3500 50  0001 C CNN
F 2 "" H 3750 3500 60  0000 C CNN
F 3 "" H 3750 3500 60  0000 C CNN
	1    3750 3500
	-1   0    0    1   
$EndComp
Text Notes 3700 3450 0    39   ~ 0
Computer\nreset pin
Wire Wire Line
	3050 4000 3600 4000
Wire Wire Line
	3600 4000 3600 4100
Wire Wire Line
	3600 4100 6750 4100
Connection ~ 4050 4100
Wire Wire Line
	3900 3600 4050 3600
NoConn ~ 4050 3400
Wire Wire Line
	3950 3500 4050 3500
Text Notes 3450 3900 0    39   ~ 0
Timeout LED
Wire Wire Line
	3500 3400 3500 2950
Wire Wire Line
	3550 3200 3550 3600
Wire Wire Line
	3600 3700 3600 3300
$EndSCHEMATC
