#include <Arduino.h>
#include <SoftwareSerial.h>

#include <MIDI.h>
#include <ReMIDI.h>

/* ----------------------------- Pin Definitions ---------------------------- */
#define MIDI_RX_PIN 2
#define MIDI_TX_PIN 3

#define ECHO_CONTROL_INPUT_PIN 4
#define ECHO_CONTROL_OUTPUT_PIN 5

#define TREMOLO_CONTROL_INPUT_PIN 8
#define TREMOLO_CONTROL_OUTPUT_PIN 9

#define LEARN_BUTTON 7
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
    remidi::createSwitchControl(1, ECHO_CONTROL_INPUT_PIN, ECHO_CONTROL_OUTPUT_PIN),
    // Tremolo
    remidi::createSwitchControl(2, TREMOLO_CONTROL_INPUT_PIN, TREMOLO_CONTROL_OUTPUT_PIN)};

remidi::ReMIDIControlList controlList = remidi::createControlList(controls);

remidi::ReMIDI<MidiTransport> reMIDI(MIDI, controlList, LEARN_BUTTON);

void setup()
{
    reMIDI.begin();
}

void loop()
{
    reMIDI.update();
}