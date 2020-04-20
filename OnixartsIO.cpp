#include "OnixartsIO.h"
//
using namespace Onixarts::Tools::IO;

DigitalOutput::DigitalOutput(byte outputPin, bool outputActiveLevelHigh)
	: m_outputPin(outputPin)
	, m_outputActiveLevel(outputActiveLevelHigh ? HIGH : LOW)
	, m_isBlinkingModeEnabled(false)
	, m_scheduledEvent(Event::None)
	, blinkingTask(this)
	, delayTask(this)
{
}
void DigitalOutput::Init()
{
	digitalWrite(m_outputPin, !m_outputActiveLevel);
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

SimpleDigitalInput::SimpleDigitalInput(byte inputPin, bool enablePullUpResistor, byte inputActiveLevel)
	: m_inputPin(inputPin)
	, m_enablePullUpResistor(enablePullUpResistor)
	, m_inputActiveLevel(inputActiveLevel)
	, m_currentState(DigitalInputState::Inactive)
{
}
void SimpleDigitalInput::Init()
{
	pinMode(m_inputPin, m_enablePullUpResistor ? INPUT_PULLUP : INPUT);
}

void SimpleDigitalInput::Update()
{
	if (IsInputActive())
	{
		if (m_currentState == DigitalInputState::Inactive)
		{
			m_currentState = DigitalInputState::Pressed;
		}
	}
	else
	{
		if (m_currentState >= DigitalInputState::Pressed)
			Released();

		if (m_currentState > DigitalInputState::Inactive)
		{
			m_currentState = DigitalInputState::Inactive;
		}
	}

	if (m_currentState == DigitalInputState::Pressed)
	{
		Pressed();
		m_currentState++;
	}
}

bool SimpleDigitalInput::IsInputActive()
{
	return digitalRead(m_inputPin) == m_inputActiveLevel;
}

bool SimpleDigitalInput::IsPressed()
{
	return m_currentState > 1;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

DigitalInput::DigitalInput(byte inputPin, bool enablePullUpResistor, byte inputActiveLevel)
	: SimpleDigitalInput(inputPin, enablePullUpResistor, inputActiveLevel )
	,debouncingTask(this)
{
}

void DigitalInput::Update()
{
	if (IsInputActive())
	{
		if( m_currentState == DigitalInputState::Inactive)
		{
			m_currentState++;
			debouncingTask.SetTaskInterval(20);
			debouncingTask.Restart();
		}
	}
	else
	{
		switch (m_currentState)
		{
		case DigitalInputState::Pressed:
		case DigitalInputState::PressedAndHolding:
			Released();
			ReleasedBefore400ms();
			break;
		case DigitalInputState::Held400ms:
		case DigitalInputState::Held400msAndHolding:
			Released();
			ReleasedAfter400ms();
			break;
		case DigitalInputState::Held1s:
		case DigitalInputState::Held1sAndHolding:
			Released();
			ReleasedAfter1s();
			break;
		case DigitalInputState::Held4s:
		case DigitalInputState::Held4sAndHolding:
			Released();
			ReleasedAfter4s();
		}
		if (m_currentState > DigitalInputState::Inactive)
		{
			m_currentState = DigitalInputState::Inactive;
			debouncingTask.Stop();
		}
	}

	switch (m_currentState)
	{
	case DigitalInputState::Pressed:
		Pressed();
		debouncingTask.SetTaskInterval(400);
		debouncingTask.Restart();
		m_currentState++;
		break;
	case DigitalInputState::Held400ms:
		Held400ms();
		debouncingTask.SetTaskInterval(600);
		debouncingTask.Restart();
		m_currentState++;
		break;
	case DigitalInputState::Held1s:
		Held1s();
		debouncingTask.SetTaskInterval(3000);
		debouncingTask.Restart();
		m_currentState++;
		break;
	case DigitalInputState::Held4s:
		Held4s();
		m_currentState++;
	}
	debouncingTask.Update(millis());
}

