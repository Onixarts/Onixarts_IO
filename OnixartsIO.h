// Onixarts_IO is an input/output helper library for Arduino
// 
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

#ifndef _OnixartsIO_h
#define _OnixartsIO_h

#include "Arduino.h"
#include <OnixartsTaskManager.h>
#include <OnixartsFSM.h>

namespace Onixarts
{
	namespace Tools
	{
		namespace IO
		{
			namespace Event
			{
				static const byte None = 0;
				static const byte On = 1;
				static const byte Off = 2;
				static const byte Toggle = 3;
				static const byte Blink = 4;
			}

			class DigitalOutput: public Onixarts::Tools::TaskManager::Manager<2>
			{
			protected:
				byte m_outputPin;
				byte m_outputActiveLevel;
				bool m_isBlinkingModeEnabled;
				byte m_scheduledEvent;

				BEGIN_TASK(BlinkingTask, 1000, TaskManager::TaskState::Stopped, 1, DigitalOutput)
				void OnStart()
				{
					GetParent().On();
				}
				void OnExecute()
				{
					if (m_repeatIndex < m_repeatCount - 1)
						GetParent().Toggle();
				}
				void OnStop()
				{
					GetParent().Off();
				}
				END_TASK(blinkingTask)

				//----------------------------------------------------------

				BEGIN_TASK(DelayTask, 1000, TaskManager::TaskState::Stopped, 1, DigitalOutput)
				void OnExecute()
				{
					switch (GetParent().m_scheduledEvent)
					{
					case Event::On: GetParent().Activate(); break;
					case Event::Off: GetParent().Deactivate(); break;
					case Event::Toggle: GetParent().Toggle(); break;
					case Event::Blink: GetParent().blinkingTask.Start(); break;
					}
				}
				END_TASK(delayTask)

				void Activate();
				void Deactivate();
				virtual void OnStateChanged() {};
			public:

				DigitalOutput(byte outputPin, bool outputActiveLevelHigh = true);
				void SetOutputActiveLevel(bool outputActiveLevelHigh) { m_outputActiveLevel = outputActiveLevelHigh ? HIGH : LOW; }
				void Init();

				void On(unsigned long delay = 0);
				void Off(unsigned long delay = 0);
				void Toggle(unsigned long delay = 0);
				void Blink(unsigned long duration = 1000, int repeatCount = 1, unsigned long delay = 0);
				bool IsActive();
			};

			// ----------------------------------------------------------------------------------------------

			namespace DigitalInputEvent
			{
				const byte InputActive = 1;
				const byte InputInactive = 2;
				//const byte TimoutActive = 3;
				//const byte TimoutInactive = 4;
				const byte Timeout = 5;
			}

			class DigitalInput: public Onixarts::Tools::TaskManager::Manager<1>
			{
			protected:
				byte m_inputPin;
				bool m_enablePullUpResistor;
				byte m_inputActiveLevel;
				Onixarts::Tools::FSM::Machine m_fsm;
				//---------------------------------------------------------
				BEGIN_TASK(DebouncingTask, 20, TaskManager::TaskState::Stopped, 1, DigitalInput);
				void OnStop()
				{
					if (GetParent().IsInputActive())
					{
						GetParent().m_fsm.Notify(DigitalInputEvent::Timeout);
					}
				}
				END_TASK(debouncingTask)
				//---------------------------------------------------------
				//---------------------------------------------------------
				//---------------------------------------------------------
				BEGIN_STATE(DigitalInput, InactiveState, 1)
					TRANSITION(DigitalInputEvent::InputActive, debouncingState)
				};
				void OnEnter(byte event)
				{
					GetParent().debouncingTask.Stop();
				}
				END_STATE(inactiveState)
				//---------------------------------------------------------
				BEGIN_STATE(DigitalInput, DebouncingState, 2)
					TRANSITION(DigitalInputEvent::Timeout, pushedState)
					TRANSITION(DigitalInputEvent::InputInactive, inactiveState)
				};
				void OnEnter(byte event)
				{
					GetParent().debouncingTask.SetTaskInterval(20);
					GetParent().debouncingTask.Restart();
				}
				END_STATE(debouncingState)
				//---------------------------------------------------------
				BEGIN_STATE(DigitalInput, PushedState, 2)
					TRANSITION(DigitalInputEvent::Timeout, held400State)
					TRANSITION(DigitalInputEvent::InputInactive, inactiveState)
				};
				void OnEnter(byte event)
				{
					GetParent().Pressed();
					GetParent().debouncingTask.SetTaskInterval(400);
					GetParent().debouncingTask.Restart();
				}
				void OnExit(byte event)
				{
					if (event == DigitalInputEvent::InputInactive)
					{
						GetParent().Released();
						GetParent().ReleasedBefore400ms();
					}
				}
				END_STATE(pushedState)
				//---------------------------------------------------------
				BEGIN_STATE(DigitalInput, Held400State, 2)
					TRANSITION(DigitalInputEvent::Timeout, held1sState)
					TRANSITION(DigitalInputEvent::InputInactive, inactiveState)
				};
				void OnEnter(byte event)
				{
					GetParent().Held400ms();
					GetParent().debouncingTask.SetTaskInterval(600);
					GetParent().debouncingTask.Restart();
				}
				void OnExit(byte event)
				{
					if (event == DigitalInputEvent::InputInactive)
					{
						GetParent().Released();
						GetParent().ReleasedAfter400ms();
					}
				}
				END_STATE(held400State)
				//---------------------------------------------------------
				BEGIN_STATE(DigitalInput, Held1sState, 2)
					TRANSITION(DigitalInputEvent::InputInactive, inactiveState)
					TRANSITION(DigitalInputEvent::Timeout, held4sState)
				};
				void OnEnter(byte event)
				{
					GetParent().Held1s();
					GetParent().debouncingTask.SetTaskInterval(3000);
					GetParent().debouncingTask.Restart();
				}
				void OnExit(byte event)
				{
					if (event == DigitalInputEvent::InputInactive)
					{
						GetParent().Released();
						GetParent().ReleasedAfter1s();
					}
				}
				END_STATE(held1sState)
				//---------------------------------------------------------
				BEGIN_STATE(DigitalInput, Held4sState, 1)
					TRANSITION(DigitalInputEvent::InputInactive, inactiveState)
				};
				void OnEnter(byte event)
				{
					GetParent().Held4s();
				}
				void OnExit(byte event)
				{
					GetParent().Released();
					GetParent().ReleasedAfter4s();
				}
				END_STATE(held4sState)

				void Pressed() { OnPressed(); };
				void Released() { OnReleased(); };
				void ReleasedBefore400ms() { OnReleasedBefore400ms();  };
				void Held400ms() { OnHeld400ms(); };
				void ReleasedAfter400ms() { OnReleasedAfter400ms(); };
				void Held1s() { OnHeld1s(); };
				void ReleasedAfter1s() { OnReleasedAfter1s(); };
				void Held4s() { OnHeld4s(); };
				void ReleasedAfter4s() { OnReleasedAfter4s(); };

				virtual void OnPressed() {};
				virtual void OnReleased() {};
				virtual void OnReleasedBefore400ms() {};
				virtual void OnHeld400ms() {};
				virtual void OnReleasedAfter400ms() {};
				virtual void OnHeld1s() {};
				virtual void OnReleasedAfter1s() {};
				virtual void OnHeld4s() {};
				virtual void OnReleasedAfter4s() {};
				//virtual void OnStateChanged() {};
				bool IsInputActive();
			public:
				DigitalInput(byte inputPin, bool enablePullUpResistor = false, byte inputActiveLevel = HIGH);
				void Init();
				void Update();

				bool IsPressed();
			};
		}
	} 
}

#endif
