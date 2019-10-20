EESchema Schematic File Version 4
LIBS:3D Scanner-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "3D - Scanner"
Date "2019-07-25"
Rev "1.0"
Comp "Nazrim"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Nazrim:AZDelivery-ESP32_NodeMCU A1
U 1 1 5D323C13
P 4250 6550
F 0 "A1" H 4250 6527 50  0000 C CNN
F 1 "AZDelivery-ESP32_NodeMCU" H 4250 6436 50  0000 C CNN
F 2 "Nazrim:AZDelivery-ESP32_NodeMCU" H 4250 6550 50  0001 C CNN
F 3 "https://cdn.shopify.com/s/files/1/1509/1638/files/ESP_-_32_NodeMCU_Developmentboard_Datenblatt_a3bf98d8-6a53-4d26-8f1c-c61b1c82af39.pdf?76837" H 4250 6550 50  0001 C CNN
	1    4250 6550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 5D336B28
P 1750 7150
F 0 "C2" H 1750 7250 50  0000 C CNN
F 1 "100μ" H 1750 7050 50  0000 C CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 1788 7000 50  0001 C CNN
F 3 "~" H 1750 7150 50  0001 C CNN
	1    1750 7150
	1    0    0    -1  
$EndComp
Connection ~ 1600 7300
Wire Wire Line
	1600 7300 1600 7550
Wire Wire Line
	1600 7050 1600 7300
$Comp
L Device:C C3
U 1 1 5D36E9E3
P 2100 6900
F 0 "C3" H 2215 6946 50  0000 L CNN
F 1 "473n" H 2215 6855 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.4mm_W2.1mm_P2.50mm" H 2138 6750 50  0001 C CNN
F 3 "~" H 2100 6900 50  0001 C CNN
	1    2100 6900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 7050 2450 6850
$Comp
L Nazrim:A4988 A3RT1
U 1 1 5D394751
P 4800 2000
F 0 "A3RT1" H 4850 2881 50  0000 C CNN
F 1 "Pololu_Breakout_A4988" H 4850 2790 50  0000 C CNN
F 2 "Nazrim:A4988" H 5075 1250 50  0001 L CNN
F 3 "https://www.pololu.com/product/2980/pictures" H 4900 1700 50  0001 C CNN
	1    4800 2000
	1    0    0    -1  
$EndComp
Wire Notes Line
	2000 6800 2450 6800
Wire Notes Line
	2450 6800 2450 7000
Wire Notes Line
	2450 7000 2000 7000
Text Notes 2700 7400 0    50   ~ 0
This capacitor may not\nbe needed but was req-\nuired on my breadboard\nto provide stable voltage.
Wire Notes Line
	1900 7400 1900 7000
Text Notes 1650 7750 0    50   ~ 0
The datasheet recommends\nthe usage of Tantal capacitors.
Wire Notes Line
	1350 7000 1900 7000
Wire Notes Line
	1350 7400 1900 7400
$Comp
L Nazrim:LT1086CT-5 U2
U 1 1 5D3B7448
P 1600 6750
F 0 "U2" H 1600 7015 50  0000 C CNN
F 1 "LT1086CT-5" H 1600 6924 50  0000 C CNN
F 2 "Nazrim:TO-220-3_Vertical_Cooler" H 1450 6900 50  0001 C CNN
F 3 "https://www.analog.com/media/en/technical-documentation/data-sheets/1086ffs.pdf" H 1450 6900 50  0001 C CNN
	1    1600 6750
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_Dual_AAC D1
U 1 1 5D31A750
P 1700 5800
F 0 "D1" H 1700 6225 50  0000 C CNN
F 1 "LED_Dual_AAC" H 1700 6134 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1700 5800 50  0001 C CNN
F 3 "~" H 1700 5800 50  0001 C CNN
	1    1700 5800
	0    -1   -1   0   
$EndComp
Text Notes 650  4550 0    50   ~ 0
This resistor is used to adjust\nthe brightness of the red LED.\nRequired for a proper yellow\nwhen both LEDs are turned on.
Wire Wire Line
	1700 6450 1700 6100
Wire Wire Line
	1850 5250 1800 5250
Wire Wire Line
	1800 5250 1800 5500
$Comp
L Device:R R2
U 1 1 5D3C4A65
P 2000 5250
F 0 "R2" V 2207 5250 50  0000 C CNN
F 1 "100" V 2116 5250 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 1930 5250 50  0001 C CNN
F 3 "~" H 2000 5250 50  0001 C CNN
	1    2000 5250
	0    -1   -1   0   
$EndComp
Wire Notes Line
	1850 5000 2150 5000
Wire Notes Line
	2150 5000 2150 5350
Wire Notes Line
	2150 5350 1850 5350
Wire Notes Line
	1850 4200 1850 5350
Wire Wire Line
	3900 3950 3400 3950
Wire Wire Line
	3400 3950 3400 6250
Wire Wire Line
	3400 6250 3550 6250
Wire Notes Line
	1850 4200 650  4200
Wire Wire Line
	3900 4050 3300 4050
Wire Wire Line
	3300 4050 3300 6450
Connection ~ 3300 6450
Wire Wire Line
	3300 6450 3550 6450
Wire Wire Line
	3900 4150 3200 4150
Wire Wire Line
	3200 4150 3200 6350
Wire Wire Line
	3200 6350 3550 6350
$Comp
L Device:LED_Series D2
U 1 1 5D3F85E9
P 7000 6200
F 0 "D2" V 7046 6113 50  0000 R CNN
F 1 "LED_Series" V 6955 6113 50  0000 R CNN
F 2 "Nazrim:TerminalBlock_2_P5.08mm" H 6900 6200 50  0001 C CNN
F 3 "~" H 6900 6200 50  0001 C CNN
	1    7000 6200
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED_Series D3
U 1 1 5D3F9378
P 7000 5500
F 0 "D3" V 7046 5413 50  0000 R CNN
F 1 "LED_Series" V 6955 5413 50  0000 R CNN
F 2 "Nazrim:TerminalBlock_2_P5.08mm" H 6900 5500 50  0001 C CNN
F 3 "~" H 6900 5500 50  0001 C CNN
	1    7000 5500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3100 3600 3100 6450
Connection ~ 3100 6450
Wire Wire Line
	3100 6450 3300 6450
Wire Wire Line
	2450 6850 2700 6850
Wire Wire Line
	6350 5250 6350 5950
Connection ~ 6350 5950
Wire Wire Line
	6350 5950 6350 7450
Wire Notes Line
	6850 5250 6850 6450
Wire Notes Line
	7550 6450 7550 5250
$Comp
L Motor:Stepper_Motor_bipolar M2TT1
U 1 1 5D423486
P 2750 1700
F 0 "M2TT1" H 2938 1824 50  0000 L CNN
F 1 "Stepper_Motor_bipolar" H 2938 1733 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 2760 1690 50  0001 C CNN
F 3 "http://www.infineon.com/dgdl/Application-Note-TLE8110EE_driving_UniPolarStepperMotor_V1.1.pdf?fileId=db3a30431be39b97011be5d0aa0a00b0" H 2760 1690 50  0001 C CNN
	1    2750 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 5050 4950 5050
Wire Wire Line
	4950 5750 5450 5750
Wire Wire Line
	5450 5750 5450 4500
Wire Wire Line
	5550 4600 5550 5450
Wire Wire Line
	5550 5450 4950 5450
Wire Wire Line
	5650 4700 5650 5550
Wire Wire Line
	5650 5550 4950 5550
Wire Wire Line
	7600 4850 5750 4850
Wire Wire Line
	5750 4850 5750 5650
Wire Wire Line
	5750 5650 4950 5650
Wire Wire Line
	7800 4600 7800 5050
Wire Wire Line
	7700 4500 7700 4950
Wire Wire Line
	7700 4950 5850 4950
Wire Wire Line
	5950 5250 5950 5050
Wire Wire Line
	5950 5050 7100 5050
Wire Wire Line
	4950 5250 5950 5250
Wire Wire Line
	5850 4950 5850 6450
Wire Wire Line
	5850 6450 5350 6450
Wire Wire Line
	7900 4700 7900 5150
Wire Wire Line
	7900 5150 4950 5150
Wire Wire Line
	6350 5250 7000 5250
Wire Wire Line
	6350 5950 7000 5950
Wire Wire Line
	2050 1800 2450 1800
Wire Wire Line
	2650 1300 2650 1400
Wire Wire Line
	3550 5750 3000 5750
Wire Wire Line
	3000 3300 850  3300
Wire Wire Line
	850  1700 1150 1700
Wire Wire Line
	4950 5350 6050 5350
Wire Wire Line
	1050 1600 1150 1600
Wire Wire Line
	3550 5550 2900 5550
Wire Wire Line
	950  1500 1150 1500
Wire Wire Line
	1150 1900 750  1900
Wire Wire Line
	750  1900 750  2000
Wire Wire Line
	750  2100 1150 2100
Wire Wire Line
	1150 2000 750  2000
Connection ~ 750  2000
Wire Wire Line
	750  2000 750  2100
Connection ~ 750  2100
Wire Wire Line
	2900 3400 2900 5550
Wire Wire Line
	950  3400 2900 3400
Wire Wire Line
	2800 3500 2800 6750
Connection ~ 2800 6750
Wire Wire Line
	750  3500 2800 3500
Wire Wire Line
	1550 900  1550 750 
Wire Wire Line
	1550 750  750  750 
Wire Wire Line
	1550 3600 2700 3600
$Comp
L Device:C C4
U 1 1 5D4D1511
P 2450 2500
F 0 "C4" V 2702 2500 50  0000 C CNN
F 1 "100μ" V 2611 2500 50  0000 C CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 2488 2350 50  0001 C CNN
F 3 "~" H 2450 2500 50  0001 C CNN
	1    2450 2500
	0    1    1    0   
$EndComp
Wire Notes Line
	2000 6800 2000 7400
Wire Notes Line
	1650 7750 2850 7750
Wire Wire Line
	1750 2500 2300 2500
Wire Wire Line
	2950 750  1750 750 
Wire Wire Line
	1750 750  1750 900 
Wire Wire Line
	4400 1600 4300 1600
Wire Wire Line
	4300 1600 4300 1700
Wire Wire Line
	4300 1700 4400 1700
Wire Wire Line
	3000 3300 3000 5750
Wire Wire Line
	2150 5250 3550 5250
Wire Wire Line
	750  2100 750  3500
Wire Wire Line
	850  1700 850  3300
Wire Wire Line
	950  1500 950  3400
Wire Wire Line
	1050 1600 1050 3200
$Comp
L Device:R R1
U 1 1 5D3C4324
P 2000 6450
F 0 "R1" V 2207 6450 50  0000 C CNN
F 1 "100" V 2116 6450 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 1930 6450 50  0001 C CNN
F 3 "~" H 2000 6450 50  0001 C CNN
	1    2000 6450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2150 6450 3100 6450
Wire Wire Line
	1850 6450 1700 6450
Wire Wire Line
	4400 1900 4300 1900
Wire Wire Line
	4300 1900 4300 3000
Wire Wire Line
	2400 3000 2400 5650
Wire Wire Line
	2400 5650 3550 5650
Wire Wire Line
	4400 2000 4200 2000
Wire Wire Line
	4200 2000 4200 2900
Wire Wire Line
	4200 2900 2200 2900
Wire Wire Line
	2200 2900 2200 5450
Wire Wire Line
	2200 5450 3550 5450
Wire Wire Line
	1750 3100 2500 3100
Wire Wire Line
	2600 2500 2950 2500
Wire Wire Line
	4400 2100 4100 2100
Wire Wire Line
	4100 2100 4100 2800
Wire Wire Line
	4100 2800 2300 2800
Wire Wire Line
	2300 2800 2300 5350
Wire Wire Line
	2300 5350 3550 5350
Wire Wire Line
	4400 2500 4000 2500
Wire Wire Line
	4000 2500 4000 2400
Wire Wire Line
	4000 2300 4400 2300
Wire Wire Line
	4400 2400 4000 2400
Connection ~ 4000 2400
Wire Wire Line
	4000 2400 4000 2300
Connection ~ 4000 2300
Wire Wire Line
	750  750  750  650 
Wire Wire Line
	750  650  4000 650 
$Comp
L Device:C C5
U 1 1 5D6A66B7
P 5700 2900
F 0 "C5" V 5952 2900 50  0000 C CNN
F 1 "100μ" V 5861 2900 50  0000 C CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 5738 2750 50  0001 C CNN
F 3 "~" H 5700 2900 50  0001 C CNN
	1    5700 2900
	0    1    1    0   
$EndComp
Wire Wire Line
	5000 2900 5550 2900
Wire Wire Line
	5000 1150 5000 1300
Wire Wire Line
	4000 650  4000 1150
Wire Wire Line
	4800 1300 4800 1150
Wire Wire Line
	4800 1150 4000 1150
Connection ~ 4000 1150
Wire Wire Line
	4000 1150 4000 2300
Wire Wire Line
	5000 1150 6250 1150
Wire Wire Line
	5850 2900 6250 2900
Wire Wire Line
	5000 3000 6150 3000
Wire Wire Line
	1600 5050 3550 5050
$Comp
L Device:LED_Series D4
U 1 1 5D7C9477
P 7900 5500
F 0 "D4" V 7946 5413 50  0000 R CNN
F 1 "LED_Series" V 7855 5413 50  0000 R CNN
F 2 "Nazrim:TerminalBlock_2_P5.08mm" H 7800 5500 50  0001 C CNN
F 3 "~" H 7800 5500 50  0001 C CNN
	1    7900 5500
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED_Series D5
U 1 1 5D7CB01B
P 7900 6200
F 0 "D5" V 7946 6113 50  0000 R CNN
F 1 "LED_Series" V 7855 6113 50  0000 R CNN
F 2 "Nazrim:TerminalBlock_2_P5.08mm" H 7800 6200 50  0001 C CNN
F 3 "~" H 7800 6200 50  0001 C CNN
	1    7900 6200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7000 5250 7650 5250
Wire Wire Line
	7650 5250 7650 5950
Connection ~ 7000 5250
Connection ~ 7650 5250
Wire Wire Line
	7650 5950 7900 5950
Wire Wire Line
	7650 5250 7900 5250
Wire Wire Line
	7000 5750 7750 5750
Wire Wire Line
	7750 5750 7750 6450
Wire Wire Line
	7750 6450 7900 6450
Connection ~ 7000 5750
Wire Wire Line
	7750 5750 7900 5750
Connection ~ 7750 5750
Wire Notes Line
	8450 6450 8450 5250
Wire Notes Line
	6850 5250 8450 5250
Wire Notes Line
	6850 5850 8450 5850
Text Notes 8500 6450 0    50   ~ 0
Each LED_Series is a 3528\nLED stripe which are used as\nthe spotlights for the scanner.\nI tested to run up to four spot-\nlights simultaneously which\nworked fine, but I noticed that\nI only need two of them.\nEach Spotlight consits of 72 LEDs.
Wire Notes Line
	6850 6450 9850 6450
$Comp
L Device:R R3
U 1 1 5D8E6B84
P 5350 6250
F 0 "R3" H 5420 6296 50  0000 L CNN
F 1 "10k" H 5420 6205 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 5280 6250 50  0001 C CNN
F 3 "~" H 5350 6250 50  0001 C CNN
	1    5350 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 6050 5350 6100
Wire Wire Line
	5350 6400 5350 6450
Connection ~ 5350 6450
Wire Wire Line
	4950 6450 5350 6450
$Comp
L Device:R R4
U 1 1 5D92ACF1
P 7250 5050
F 0 "R4" V 7457 5050 50  0000 C CNN
F 1 "330" V 7366 5050 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7180 5050 50  0001 C CNN
F 3 "~" H 7250 5050 50  0001 C CNN
	1    7250 5050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7400 5050 7800 5050
$Comp
L Transistor_FET:IRLB8721PBF Q1
U 1 1 5D39DC22
P 5950 6850
F 0 "Q1" H 6156 6896 50  0000 L CNN
F 1 "IRLB8721PBF" H 6156 6805 50  0000 L CNN
F 2 "Nazrim:TO-220-3_Vertical_Cooler" H 6200 6775 50  0001 L CIN
F 3 "http://www.infineon.com/dgdl/irlb8721pbf.pdf?fileId=5546d462533600a40153566056732591" H 5950 6850 50  0001 L CNN
	1    5950 6850
	1    0    0    -1  
$EndComp
Connection ~ 6350 7450
Wire Wire Line
	6350 7450 6600 7450
Wire Wire Line
	6050 7050 6050 7350
Connection ~ 6050 7550
Wire Wire Line
	4950 5850 5650 5850
Wire Wire Line
	5650 5850 5650 6100
Wire Wire Line
	5650 6850 5750 6850
$Comp
L Device:R R6
U 1 1 5D4282A7
P 5650 6250
F 0 "R6" H 5720 6296 50  0000 L CNN
F 1 "100" H 5720 6205 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 5580 6250 50  0001 C CNN
F 3 "~" H 5650 6250 50  0001 C CNN
	1    5650 6250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 5D42921B
P 5650 7100
F 0 "R5" H 5720 7146 50  0000 L CNN
F 1 "10k" H 5720 7055 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 5580 7100 50  0001 C CNN
F 3 "~" H 5650 7100 50  0001 C CNN
	1    5650 7100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 6950 5650 6850
Connection ~ 5650 6850
Wire Wire Line
	5650 7250 5650 7350
Wire Wire Line
	5650 7350 6050 7350
Connection ~ 6050 7350
Wire Wire Line
	6050 7350 6050 7550
Wire Wire Line
	6050 6650 6050 6450
Wire Wire Line
	6050 5750 7000 5750
Wire Wire Line
	6050 6450 7000 6450
Connection ~ 6050 6450
Wire Wire Line
	6050 6450 6050 5750
Wire Wire Line
	5650 6400 5650 6850
Text Notes 4900 7250 0    50   ~ 0
Bleeder resistor
Wire Notes Line
	5550 6950 5900 6950
Wire Notes Line
	5900 6950 5900 7250
Wire Notes Line
	4900 7250 5900 7250
Wire Notes Line
	5550 6950 5550 7250
Connection ~ 2700 6850
Wire Notes Line
	2000 7400 3700 7400
Wire Wire Line
	1150 7650 1150 7550
Connection ~ 1600 7550
Wire Wire Line
	1150 7550 1250 7550
Wire Wire Line
	2800 6750 3900 6750
Wire Wire Line
	2100 6750 2800 6750
Wire Wire Line
	2100 7050 2450 7050
Wire Wire Line
	1100 7650 1150 7650
Wire Wire Line
	1100 7450 1150 7450
Wire Wire Line
	1250 7450 1300 7450
Connection ~ 1300 7450
NoConn ~ 3550 6150
NoConn ~ 3550 6050
NoConn ~ 3550 5950
NoConn ~ 3550 5850
NoConn ~ 3550 5150
NoConn ~ 3550 4650
NoConn ~ 3550 4750
NoConn ~ 3550 4850
NoConn ~ 3550 4950
NoConn ~ 4950 4650
NoConn ~ 4950 4750
NoConn ~ 4950 4850
NoConn ~ 4950 4950
NoConn ~ 4950 5950
NoConn ~ 4950 6150
NoConn ~ 4950 6250
NoConn ~ 4950 6350
Wire Wire Line
	7500 4500 7700 4500
Wire Wire Line
	7500 4700 7900 4700
Wire Wire Line
	6500 4700 5650 4700
Wire Wire Line
	6500 4600 5550 4600
Wire Wire Line
	5450 4500 6500 4500
Wire Wire Line
	7500 4600 7800 4600
Wire Wire Line
	1600 5500 1600 5050
Text Label 1600 5050 2    50   ~ 0
GREEN
Text Label 1800 5250 2    50   ~ 0
RED
Wire Wire Line
	1600 7550 2500 7550
Connection ~ 2500 7550
Wire Wire Line
	2600 7450 4400 7450
Connection ~ 2600 7450
Wire Wire Line
	2600 3100 2600 7450
Wire Wire Line
	2600 3100 2950 3100
Wire Wire Line
	2400 3000 4300 3000
$Comp
L Connector:Barrel_Jack J1
U 1 1 5D4C1CE8
P 800 7550
F 0 "J1" H 857 7875 50  0000 C CNN
F 1 "Barrel_Jack" H 857 7784 50  0000 C CNN
F 2 "Nazrim:TerminalBlock_2_P5.08mm" H 850 7510 50  0001 C CNN
F 3 "~" H 850 7510 50  0001 C CNN
	1    800  7550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 2200 5700 2200
Wire Wire Line
	6100 1600 6100 1800
Wire Wire Line
	5900 1700 5900 1800
$Comp
L Motor:Stepper_Motor_bipolar M1RT1
U 1 1 5D422859
P 6000 2100
F 0 "M1RT1" H 6188 2224 50  0000 L CNN
F 1 "Stepper_Motor_bipolar" H 6188 2133 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 6010 2090 50  0001 C CNN
F 3 "http://www.infineon.com/dgdl/Application-Note-TLE8110EE_driving_UniPolarStepperMotor_V1.1.pdf?fileId=db3a30431be39b97011be5d0aa0a00b0" H 6010 2090 50  0001 C CNN
	1    6000 2100
	1    0    0    -1  
$EndComp
Connection ~ 5000 2900
Wire Wire Line
	5000 2900 5000 3000
Wire Wire Line
	5000 2800 5000 2900
Wire Wire Line
	6250 1150 6250 2900
Wire Wire Line
	2950 2500 2950 3100
$Comp
L power:+12V #PWR0101
U 1 1 5DEE3B31
P 6600 7450
F 0 "#PWR0101" H 6600 7300 50  0001 C CNN
F 1 "+12V" H 6615 7623 50  0000 C CNN
F 2 "" H 6600 7450 50  0001 C CNN
F 3 "" H 6600 7450 50  0001 C CNN
	1    6600 7450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5DEE509F
P 6600 7550
F 0 "#PWR0102" H 6600 7300 50  0001 C CNN
F 1 "GND" H 6605 7377 50  0000 C CNN
F 2 "" H 6600 7550 50  0001 C CNN
F 3 "" H 6600 7550 50  0001 C CNN
	1    6600 7550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5DF056DC
P 5250 6850
F 0 "#PWR0103" H 5250 6600 50  0001 C CNN
F 1 "GND" H 5255 6677 50  0000 C CNN
F 2 "" H 5250 6850 50  0001 C CNN
F 3 "" H 5250 6850 50  0001 C CNN
	1    5250 6850
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0104
U 1 1 5DF16493
P 5250 6750
F 0 "#PWR0104" H 5250 6600 50  0001 C CNN
F 1 "+5V" V 5265 6878 50  0000 L CNN
F 2 "" H 5250 6750 50  0001 C CNN
F 3 "" H 5250 6750 50  0001 C CNN
	1    5250 6750
	0    1    1    0   
$EndComp
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5DF45B54
P 4400 7450
F 0 "#FLG0101" H 4400 7525 50  0001 C CNN
F 1 "PWR_FLAG" H 4400 7623 50  0000 C CNN
F 2 "" H 4400 7450 50  0001 C CNN
F 3 "~" H 4400 7450 50  0001 C CNN
	1    4400 7450
	1    0    0    -1  
$EndComp
Connection ~ 4400 7450
Wire Wire Line
	2500 7550 6050 7550
$Comp
L Switch:SW_SPST SW2
U 1 1 5D751942
P 800 7050
F 0 "SW2" H 800 7285 50  0000 C CNN
F 1 "SW_SPST" H 800 7194 50  0000 C CNN
F 2 "Nazrim:TerminalBlock_2_P5.08mm" H 800 7050 50  0001 C CNN
F 3 "~" H 800 7050 50  0001 C CNN
	1    800  7050
	1    0    0    -1  
$EndComp
Connection ~ 2950 2500
Wire Wire Line
	2950 750  2950 2500
Wire Wire Line
	2850 1200 2850 1400
Connection ~ 750  1900
Connection ~ 750  750 
Wire Wire Line
	750  750  750  1900
Wire Wire Line
	1050 1200 1150 1200
Wire Wire Line
	1050 1300 1050 1200
Wire Wire Line
	1150 1300 1050 1300
$Comp
L Nazrim:A4988 A2TT1
U 1 1 5D393B78
P 1550 1600
F 0 "A2TT1" H 1600 2481 50  0000 C CNN
F 1 "Pololu_Breakout_A4988" H 1600 2390 50  0000 C CNN
F 2 "Nazrim:A4988" H 1825 850 50  0001 L CNN
F 3 "https://www.pololu.com/product/2980/pictures" H 1650 1300 50  0001 C CNN
	1    1550 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 1600 2350 1600
Wire Wire Line
	2350 1600 2350 1700
Wire Wire Line
	2050 1700 2350 1700
Wire Wire Line
	2250 1300 2650 1300
Wire Wire Line
	5700 2000 5600 2000
Wire Wire Line
	5600 2000 5600 2100
Wire Wire Line
	5300 2100 5600 2100
Wire Wire Line
	5500 1700 5900 1700
Wire Wire Line
	5500 1700 5500 1900
Wire Wire Line
	6100 1600 5400 1600
Wire Wire Line
	2150 1200 2850 1200
Wire Wire Line
	4400 7450 6250 7450
Wire Wire Line
	6250 7450 6350 7450
Connection ~ 6250 7450
Connection ~ 6250 2900
Wire Wire Line
	6250 2900 6250 7450
$Comp
L Nazrim:Nokia5110-Display DS1
U 1 1 5D424E93
P 7000 4200
F 0 "DS1" H 7000 4225 50  0000 C CNN
F 1 "Nokia5110-Display" H 7000 4134 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 7000 4200 50  0001 C CNN
F 3 "" H 7000 4200 50  0001 C CNN
	1    7000 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 4400 7600 4400
Wire Wire Line
	6500 4400 5350 4400
Wire Wire Line
	5350 6050 5050 6050
Wire Wire Line
	4950 6050 5050 6050
Connection ~ 5050 6050
Wire Wire Line
	5050 4150 5050 6050
Wire Wire Line
	6050 7550 6150 7550
Wire Wire Line
	6150 7550 6600 7550
Connection ~ 6150 7550
Wire Wire Line
	6150 3000 6150 7550
Wire Wire Line
	6050 5350 6050 3200
Wire Wire Line
	7600 4400 7600 4850
Wire Wire Line
	5350 4400 5350 5050
Wire Wire Line
	4650 3600 3100 3600
Wire Wire Line
	4650 3950 4650 3600
Wire Wire Line
	4500 3950 4650 3950
Wire Wire Line
	5050 4150 4500 4150
$Comp
L Device:Rotary_Encoder_Switch SW1
U 1 1 5D38BDC8
P 4200 4050
F 0 "SW1" H 4200 4417 50  0000 C CNN
F 1 "Rotary_Encoder_Switch" H 4200 4326 50  0000 C CNN
F 2 "Nazrim:Rotary_Encoder_Switch" H 4050 4210 50  0001 C CNN
F 3 "~" H 4200 4310 50  0001 C CNN
	1    4200 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 6850 3800 6850
Wire Wire Line
	5400 2000 5300 2000
Wire Wire Line
	5400 1600 5400 2000
Wire Wire Line
	5300 1900 5500 1900
Wire Wire Line
	2050 1500 2250 1500
Wire Wire Line
	2250 1500 2250 1300
Wire Wire Line
	2150 1200 2150 1600
Wire Wire Line
	2150 1600 2050 1600
Wire Wire Line
	1900 6750 1950 6750
Wire Wire Line
	1150 7450 1150 7050
Wire Wire Line
	1150 7050 1000 7050
Wire Wire Line
	600  7050 600  6750
Wire Wire Line
	600  6750 1250 6750
Wire Wire Line
	1250 6750 1250 7450
$Comp
L Device:C C1
U 1 1 5D33643F
P 1450 7300
F 0 "C1" V 1702 7300 50  0000 C CNN
F 1 "10μ" V 1611 7300 50  0000 C CNN
F 2 "Capacitor_THT:CP_Radial_D4.0mm_P2.00mm" H 1488 7150 50  0001 C CNN
F 3 "~" H 1450 7300 50  0001 C CNN
	1    1450 7300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1300 6750 1300 7300
Wire Wire Line
	1300 7300 1300 7450
Connection ~ 1300 7300
$Comp
L Connector:Screw_Terminal_01x02 J2
U 1 1 5D55C1AE
P 3900 7150
F 0 "J2" V 3772 7230 50  0000 L CNN
F 1 "Screw_Terminal_01x02" V 3863 7230 50  0000 L CNN
F 2 "Nazrim:TerminalBlock_2_P5.08mm" H 3900 7150 50  0001 C CNN
F 3 "~" H 3900 7150 50  0001 C CNN
	1    3900 7150
	0    1    1    0   
$EndComp
Wire Wire Line
	3900 6950 3900 6750
Connection ~ 3900 6750
Wire Wire Line
	3900 6750 5250 6750
Wire Wire Line
	3800 6950 3800 6850
Connection ~ 3800 6850
Wire Wire Line
	3800 6850 5250 6850
Text Notes 3750 7700 0    50   ~ 0
Used to power the ESP32 via USB.
Wire Notes Line
	3700 7700 5100 7700
Wire Notes Line
	3700 6950 4850 6950
Wire Notes Line
	4850 6950 4850 7200
Wire Notes Line
	4850 7200 3700 7200
Wire Notes Line
	3700 6950 3700 7700
Wire Wire Line
	1600 7300 1750 7300
Wire Wire Line
	1300 7450 2600 7450
Wire Wire Line
	1750 7000 1950 7000
Wire Wire Line
	1950 7000 1950 6750
Wire Wire Line
	2100 7050 2100 7300
Wire Wire Line
	2100 7300 1750 7300
Connection ~ 2100 7050
Connection ~ 1750 7300
Wire Wire Line
	2100 6750 2000 6750
Wire Wire Line
	2000 6750 2000 7000
Wire Wire Line
	2000 7000 1950 7000
Connection ~ 2100 6750
Connection ~ 1950 7000
Wire Wire Line
	1550 3600 1550 3200
Wire Wire Line
	1050 3200 6050 3200
Wire Wire Line
	1750 2400 1750 2500
Connection ~ 1750 2500
Wire Wire Line
	4800 2900 4400 2900
Wire Wire Line
	4400 2700 4400 2900
Wire Wire Line
	4800 2800 4800 2900
Wire Wire Line
	2500 3100 2500 7550
Wire Wire Line
	1750 2500 1750 3100
Wire Wire Line
	2700 3600 2700 6850
Connection ~ 1550 2700
Wire Wire Line
	1550 2400 1550 2700
Wire Wire Line
	1550 2700 1550 3200
Wire Wire Line
	1550 2700 4400 2700
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 5D4E8F3F
P 1250 7550
F 0 "#FLG0102" H 1250 7625 50  0001 C CNN
F 1 "PWR_FLAG" H 1250 7723 50  0000 C CNN
F 2 "" H 1250 7550 50  0001 C CNN
F 3 "~" H 1250 7550 50  0001 C CNN
	1    1250 7550
	-1   0    0    1   
$EndComp
Connection ~ 1250 7550
Wire Wire Line
	1250 7550 1600 7550
Wire Notes Line
	1350 7000 1350 7400
Wire Notes Line
	1650 7750 1650 7400
$EndSCHEMATC
