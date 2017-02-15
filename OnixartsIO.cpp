#include "OnixartsIO.h"
//
using namespace Onixarts::Tools::IO;

DigitalOutput::DigitalOutput(byte outputPin, bool outputActiveLevelHigh = true)
	: m_outputPin(outputPin)
	, m_outputActiveLevel(outputActiveLevelHigh ? HIGH : LOW)
	, blinkingTask(this)
	, delayTask(this)
	, m_isBlinkingModeEnabled(false)
	, m_scheduledEvent(Event::None)
{
}
void DigitalOutput::Init()
{
	pinMode(m_outputPin, OUTPUT);

	AddTask(blinkingTask);
	AddTask(delayTask);

	Deactivate();
}

void DigitalOutput::On(unsigned long delay)
{
	if (delay > 0)
	{
		m_scheduledEvent = Event::On;
		delayTask.SetTaskInterval(delay);
		delayTask.Restart();
	}
	else
	{
		Activate();
		m_scheduledEvent = Event::None;
	}
}

void DigitalOutput::Off(unsigned long delay)
{
	if (delay > 0)
	{
		m_scheduledEvent = Event::Off;
		delayTask.SetTaskInterval(delay);
		delayTask.Restart();
	}
	else
	{
		Deactivate();
		m_scheduledEvent = Event::None;
	}
}

void DigitalOutput::Toggle(unsigned long delay)
{
	if (delay > 0)
	{
		m_scheduledEvent = Event::Toggle;
		delayTask.SetTaskInterval(delay);
		delayTask.Restart();
	}
	else
	{
		if (IsActive())
			Deactivate();
		else
			Activate();
		m_scheduledEvent = Event::None;
	}
}

void DigitalOutput::Blink(unsigned long duration, int repeatCount, unsigned long delay)
{
	blinkingTask.SetTaskInterval(duration);
	blinkingTask.SetRepeatCount(2*repeatCount);
	if (delay > 0)
	{
		m_scheduledEvent = Event::Blink;
		delayTask.SetTaskInterval(delay);
		delayTask.Restart();
	}
	else
	{
		blinkingTask.Restart();
		m_scheduledEvent = Event::None;
	}
}

void DigitalOutput::Activate()
{
	if (!IsActive())
	{
		digitalWrite(m_outputPin, m_outputActiveLevel);
		OnStateChanged();
	}
}

void DigitalOutput::Deactivate()
{
	if (IsActive())
	{
		digitalWrite(m_outputPin, !m_outputActiveLevel);
		OnStateChanged();
	}
}

bool DigitalOutput::IsActive()
{
	return digitalRead(m_outputPin) == m_outputActiveLevel;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

DigitalInput::DigitalInput(byte inputPin, bool enablePullUpResistor, byte inputActiveLevel)
	: debouncingTask(this)
	, m_inputPin(inputPin)
	, m_enablePullUpResistor(enablePullUpResistor)
	, m_inputActiveLevel(inputActiveLevel)
	, inactiveState(this)
	, pushedState(this)
	, held400State(this)
	, held1sState(this)
	, held4sState(this)
	, debouncingState(this)
{
}
void DigitalInput::Init()
{
	pinMode(m_inputPin, m_enablePullUpResistor ? INPUT_PULLUP : INPUT);
	AddTask(debouncingTask);
	m_fsm.SetCurrentState(inactiveState);
}

void DigitalInput::Update()
{
	if (IsInputActive())
		m_fsm.Notify(DigitalInputEvent::InputActive);
	else 
		m_fsm.Notify(DigitalInputEvent::InputInactive);
	debouncingTask.Update(millis());
}

bool DigitalInput::IsInputActive()
{
	return digitalRead(m_inputPin) == m_inputActiveLevel;
}


bool DigitalInput::IsPressed()
{
	Onixarts::Tools::FSM::State* state = m_fsm.GetCurrentState();
	if (state == NULL)
		return false;
	if (state == &inactiveState || state == &debouncingState)
		return false;

	return true;
}

//void DigitalInput::OnPressed() 
//{
//	Serial.println("Pressed");
//}
//void DigitalInput::OnReleased()
//{
//	Serial.println("released");
//}
//
//void DigitalInput::OnReleasedBefore400ms()
//{
//	Serial.println("Released before 400ms");
//}
//
//void DigitalInput::OnHeld400ms()
//{
//	Serial.println("Held 400");
//}
//
//void DigitalInput::OnReleasedAfter400ms()
//{
//	Serial.println("released after 400 ms");
//}
//
//void DigitalInput::OnHeld1s()
//{
//	Serial.println("Held 1s");
//}
//
//void DigitalInput::OnReleasedAfter1s()
//{
//	Serial.println("released after 1s");
//}
//
//void DigitalInput::OnHeld4s()
//{
//	Serial.println("Held 4s");
//}
//
//void DigitalInput::OnReleasedAfter4s()
//{
//	Serial.println("released after 4s");
//}

