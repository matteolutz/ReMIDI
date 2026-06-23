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
/* -------------------------------------------------------------------------- */

/* ------------ Forward declaration for setup and loop functions ------------ */
void setup();
void loop();
/* -------------------------------------------------------------------------- */

using MidiTransport = MIDI_NAMESPACE::SerialMIDI<SoftwareSerial>;
SoftwareSerial midiSerial(MIDI_RX_PIN, MIDI_TX_PIN);
MIDI_NAMESPACE::MidiInterface<MidiTransport> MIDI((MidiTransport &)midiSerial);

remidi::ReMIDIControl controls[] = {
    // Echo
    remidi::createSwitchControl(1, {.inputPin = ECHO_CONTROL_INPUT_PIN, .inputPullup = ECHO_CONTROL_INPUT_PULLUP, .outputPin = ECHO_CONTROL_OUTPUT_PIN, .outputActiveLow = false}),
    // Tremolo
    remidi::createSwitchControl(2, {.inputPin = TREMOLO_CONTROL_INPUT_PIN, .inputPullup = TREMOLO_CONTROL_INPUT_PULLUP, .outputPin = TREMOLO_CONTROL_OUTPUT_PIN, .outputActiveLow = false}),
};

remidi::ReMIDIControlList controlList = remidi::createControlList(controls);

remidi::ReMIDI<MidiTransport> reMIDI(MIDI, controlList);

void setup()
{
    reMIDI.setLearnButton(LEARN_BUTTON_PIN, LEARN_BUTTON_PULLUP);
    reMIDI.begin();
}

void loop()
{
    reMIDI.update();
}