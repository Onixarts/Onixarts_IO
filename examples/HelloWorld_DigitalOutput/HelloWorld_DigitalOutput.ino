// Onixarts_IO Hello World Digital Output demo. 
// 
// Code explanation: TODO
// Github: https://github.com/Onixarts/Onixarts_IO
// Author's site: http://onixarts.pl
// Contact: software@onixarts.pl
// 
// Copyright (C) 2017  Bartosz Rosa (onixarts.pl)
//
// This program is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.If not, see <http://www.gnu.org/licenses/>.

#include "Arduino.h"
#include <OnixartsIO.h>
#include <OnixartsTaskManager.h>

using namespace Onixarts::Tools;

// declare led object represents digital output. Pass putput pin number and optional active state (default true = HIGH, false = LOW)
IO::DigitalOutput led(LED_BUILTIN, true);

void setup()
{
	Serial.begin(115200);
	Serial.println("Onixarts IO Digital Output demo");

	led.Init();

	// switch output to active level
	led.On();

	// switch off after 5s
	led.Off(5000);

	// toggle led after 3s
	led.Toggle(3000);

	// blink led 10 times with 50ms duration after 5s
	led.Blink(50, 10, 5000);

	// change default active level to LOW, normally used once in setup to match board configuration 
	led.SetOutputActiveLevel(false);

	// On function cause LOW level on output pin
	led.On();

	// Off cause HIGH level on output pin
	led.Off(5000);
}

void loop()
{
	// update led internal time counters. Do not block loop() function for long time.
	led.Update();
}