#include <Arduino.h>
#include <SoftwareSerial.h>

#include <MIDI.h>

#include <ReMIDI.h>

/* ----------------------------- Pin Definitions ---------------------------- */
#define MIDI_RX_PIN 2
#define MIDI_TX_PIN 3 // will be unused, but required for SoftwareSerial constructor

#define ECHO_CONTROL_INPUT_PIN 4
#define ECHO_CONTROL_INPUT_PULLUP true
#define ECHO_CONTROL_OUTPUT_PIN 5

#define TREMOLO_CONTROL_INPUT_PIN 8
#define TREMOLO_CONTROL_INPUT_PULLUP true
#define TREMOLO_CONTROL_OUTPUT_PIN 9

#define LEARN_BUTTON_PIN 7
#define LEARN_BUTTON_PULLUP true

#define MIDI_IN_CHANNEL 1
/* -------------------------------------------------------------------------- */

/* ------------ Forward declaration for setup and loop functions ------------ */
void setup();
void loop();
/* -------------------------------------------------------------------------- */

SoftwareSerial midiSerial(MIDI_RX_PIN, MIDI_TX_PIN);
MIDI_CREATE_INSTANCE(SoftwareSerial, midiSerial, MIDI);

/**
 * This is the callback for ReMIDI to read MIDI messages
 * This allows for BYOM (Bring Your Own MIDI), where the user can choose how to receive MIDI messages.
 */
ReMIDIMessage readMidiMessage();

remidi::ReMIDIControl controls[] = {
    // Echo
    remidi::createSwitchControl(1, {.inputPin = ECHO_CONTROL_INPUT_PIN, .inputPullup = ECHO_CONTROL_INPUT_PULLUP, .outputPin = ECHO_CONTROL_OUTPUT_PIN, .outputActiveLow = false}),
    // Tremolo
    remidi::createSwitchControl(2, {.inputPin = TREMOLO_CONTROL_INPUT_PIN, .inputPullup = TREMOLO_CONTROL_INPUT_PULLUP, .outputPin = TREMOLO_CONTROL_OUTPUT_PIN, .outputActiveLow = false}),
};

remidi::ReMIDIControlList controlList = remidi::createControlList(controls);

remidi::ReMIDI reMIDI(readMidiMessage, controlList);

void setup()
{
    MIDI.begin(MIDI_IN_CHANNEL);

    reMIDI.setLearnButton(LEARN_BUTTON_PIN, LEARN_BUTTON_PULLUP);
    reMIDI.begin();
}

void loop()
{
    reMIDI.update();
}

ReMIDIMessage readMidiMessage()
{
#if REMIDI_SERIAL_MIDI_TEST
    if (Serial.available() > 0)
    {
        uint8_t pcByte = Serial.read();
        return ReMIDIMessage::createProgramChangeMessage(pcByte);
    };

#else
    if (MIDI.read())
    {
        ReMIDIMessage message;
        message.type = static_cast<ReMIDIMessageType>(MIDI.getType());
        message.data1 = MIDI.getData1();
        message.data2 = MIDI.getData2();

        return message;
    }
#endif

    return ReMIDIMessage::createInvalidMessage();
}