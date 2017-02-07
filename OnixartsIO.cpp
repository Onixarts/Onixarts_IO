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
