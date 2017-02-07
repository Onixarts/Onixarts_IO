# Onixarts_IO
Input and output helper library for Arduino, simplify common IO tasks implementation. This library is used by [Hapcanuino](https://github.com/Onixarts/Hapcanuino) project.


## Library dependencies
- [Onixarts_TaskManager](https://github.com/Onixarts/Onixarts_TaskManager) - delivers delay and blink functions

## Digital Output Features

- On, Off, Toggle, Blink functions
- Configurable active level (LOW, HIGH)
- Every function execution can be Delayed
- No timer used (using millis())
- Not suitable for high time precision tasks


## Basic usage

See HelloWorld_DigitalOutput example for full code.

Include main library and dependecies:

```C++
#include <OnixartsIO.h>
#include <OnixartsTaskManager.h>
```

Library used namespaces to organize types and consts, so put a default namespace and declare LED obect as built in diode.

```C++
using namespace Onixarts::Tools;

IO::DigitalOutput led(LED_BUILTIN, true);
//IO::DigitalOutput led(LED_BUILTIN);
```

First constructor parameter is the output PIN, second is active level. True is de default value and it can be ignored. It means that active level is HIGH. Passing false will cause set active level to LOW value on output pin.

In `setup()` call `Init()` method which will setup pin mode.

```C++
void setup()
{
	Serial.begin(115200);
	Serial.println("Onixarts IO Digital Output demo");

	led.Init();
}
```

In `loop()` function You should call `Update()` method, which will handle delay and blink tasks. This method do not block program execution and should be call as often as possible. 
Do not put Arduino delay() function in loop() because it will affect delay and blink accuracy.

```C++
void loop()
{
	led.Update();
}
```

Now You can use `digitalOutput` like shown below:

```C++
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
```