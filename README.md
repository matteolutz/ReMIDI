# ReMIDI

ReMIDI is a small Arduino library for turning MIDI Program Change messages into preset-driven control changes.

It is designed for hardware setups where a MIDI foot controller, keyboard, or DAW sends Program Change messages and the sketch responds by driving relays, LEDs, or other digital outputs to a stored state.

ReMIDI is still in very early development. The current focus is on Program Change-driven preset recall and learning, but the design is intentionally open for future MIDI features such as Control Change messages, analog output control, and other message-driven behaviors.

## What it does

ReMIDI lets you...

- map Program Change numbers to saved control states
- store presets in EEPROM
- learn presets from live control positions using a button
- restore those presets automatically when the matching Program Change arrives
- provide your own MIDI input source through a callback, so the library works with hardware MIDI, serial MIDI, or test stubs

## Features

- Built for Arduino and PlatformIO
- EEPROM-backed preset storage
- Per-control IDs so presets survive reordering as long as IDs stay the same
- Learn mode for capturing the current control states into a preset
- Startup reset path: hold the learn button while booting to clear the stored preset list
- Native test environment support for development on desktop

## Installation

### PlatformIO

Add the library to your project and include it from your sketch or source file:

```ini
lib_deps =
	https://github.com/matteolutz/ReMIDI.git
```

The library itself depends on [FortySevenEffects MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library).

### Arduino

Copy the `include/` and `src/` contents into your Arduino project, or install the repository as a local library if your workflow supports it.

## Quick Start

The basic flow is:

1. Define your controls.
2. Provide a function that reads the next MIDI message.
3. Create a `ReMIDI` instance.
4. Call `begin()` once in `setup()`.
5. Call `update()` repeatedly in `loop()`.

```cpp
#include <Arduino.h>
#include <MIDI.h>
#include <ReMIDI.h>

ReMIDIMessage readMidiMessage()
{
	if (MIDI.read())
	{
		return {
			static_cast<ReMIDIMessageType>(MIDI.getType()),
			MIDI.getData1(),
			MIDI.getData2(),
		};
	}

	return ReMIDIMessage::createInvalidMessage();
}

remidi::ReMIDIControl controls[] = {
	remidi::createSwitchControl(1, {.inputPin = 4, .inputPullup = true, .outputPin = 5, .outputActiveLow = false}),
	remidi::createSwitchControl(2, {.inputPin = 8, .inputPullup = true, .outputPin = 9, .outputActiveLow = false}),
};

remidi::ReMIDIControlList controlList = remidi::createControlList(controls);
remidi::ReMIDI reMIDI(readMidiMessage, controlList);

void setup()
{
	MIDI.begin(1);
	reMIDI.setLearnButton(7, true);
	reMIDI.begin();
}

void loop()
{
	reMIDI.update();
}
```

## Learning Presets

When the learn button is held, ReMIDI waits for a Program Change message. Once one arrives, it captures the current state of every configured control and stores the result as a preset for that Program Change number.

On the next matching Program Change, the stored control states are restored.

If the learn button is held during startup, ReMIDI clears the preset list in EEPROM before continuing.

## Public API

The main entry point is [`include/ReMIDI.h`](include/ReMIDI.h).

Key pieces:

- `remidi::ReMIDI` manages message polling, learning, and preset application.
- `remidi::ReMIDIControl` describes a control that can be saved and restored.
- `remidi::ReMIDIControlList` wraps a control array for lookup by control ID.
- `ReMIDIMessage` is the small message type returned by your MIDI read callback.
- `remidi::createSwitchControl(...)` creates the current supported control type.

The current control implementation is intentionally simple: switches map an input pin to an output pin, with optional pull-up and active-low behavior.

## Example Project

The [`examples/tapedelay/tapedelay.cpp`](examples/tapedelay/tapedelay.cpp) sketch shows a complete wiring example using:

- SoftwareSerial MIDI input
- two switch controls
- a learn button
- a MIDI Program Change source

It also demonstrates a useful testing pattern: defining `REMIDI_SERIAL_MIDI_TEST` to feed Program Change values from `Serial` instead of real MIDI hardware.

## Project Layout

- `include/` - public headers
- `src/` - implementation files
- `examples/` - usage examples
- `test/` - native and AVR test suites

## Notes

- The EEPROM preset format is simple and compact, but it assumes control IDs remain stable.
- ReMIDI currently supports switch-style controls; the type system is structured so additional control types can be added later.
- The library is early-stage software, so the API and feature set may evolve as more MIDI workflows are added.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for more informations.

---

_Note:_ This README was written with the help of generative AI.

> [matteolutz.de](https://matteolutz.de) &nbsp;&middot;&nbsp;
> GitHub [@matteolutz](https://github.com/matteolutz) &nbsp;&middot;&nbsp;
> Email [info@matteolutz.de](mailto:info@matteolutz.de)
