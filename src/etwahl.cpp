#include "keypress.h"
#include "RtMidi.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

#include <climits>
#include <csignal>
#include <cstdlib>

static RtMidiIn *midiin = new RtMidiIn();

static void cleanup()
{
    delete midiin;
    exit(0);
}

static void interruptHandler(int signal)
{
    cleanup();
}

void midiHandler(double timeStamp, std::vector< unsigned char > *message, void *userData)
{
    int nBytes;
    nBytes = message->size();

    for(int i = 0; i < nBytes; i++)
        std::cout <<
            std::setw(2) << std::setfill('0') << std::hex << (int)message->at(i);

    std::cout << "    " << timeStamp << std::endl;

    const char *keystring = "Shift_L";

    if((int)message->at(2) == 127)
    {
        key(keystring, true);
    }
    else
    {
        key(keystring, false);
    }
}

int main()
{
    int portNum;

    // Install an interrupt handler function.
    (void) signal(SIGINT, interruptHandler);

    // Set MidiIn callback.
    midiin->setCallback(midiHandler);

    // Check MIDI inputs.
    unsigned int nPorts = midiin->getPortCount();
    std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
    std::string portName;

    for (int i=0; i < nPorts; i++) {
        try {
            portName = midiin->getPortName(i);
        }
        catch ( RtMidiError &error ) {
            error.printMessage();
            cleanup();
        }
        std::cout << "  Input Port #" << i+1 << ": " << portName << '\n';
    }

    // Get port number.
    std::cin >> portNum;

    midiin->openPort(portNum - 1);
    std::cout << "Reading MIDI from port " << portNum << "... Press Enter to quit.\n";

    std::cin.ignore(INT_MAX, '\n');
    std::cin.get();

    cleanup();

    return 0;
}

