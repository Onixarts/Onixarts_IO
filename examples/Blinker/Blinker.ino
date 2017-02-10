// Onixarts_IO Hello World Digital Output blinker demo. Built in LED will blink each second 5 times.
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

IO::DigitalOutput led;

// task manager executing OnTaskExecute function each 1000ms, running by default
TaskManager::Manager<1> manager;
TaskManager::Task task1(OnTaskExecute, 1000, TaskManager::TaskState::Running);

void setup()
{
	Serial.begin(115200);
	Serial.println("Onixarts IO blinker demo");

	led.Init(LED_BUILTIN);

	manager.AddTask(task1);
	//	led.Blink(50,20,3000);
}

void loop()
{
	led.Update();
	manager.Update();
}

void OnTaskExecute()
{
	led.Blink(50, 5);
}
