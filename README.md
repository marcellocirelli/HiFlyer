# EMS Synthi Hi-Fli (JUCE)

A software recreation of the **EMS Synthi Hi-Fli**, built as a
standalone application and audio plugin using **JUCE**.\
This project focuses on faithfully replicating the original hardware
signal processing, control layout, and functionality.

## Overview

The EMS Synthi Hi-Fli was a distinctive guitar effects processor known
for its modulation-based treatments, routing flexibility, and
unconventional control structure.

The current development stage focuses on DSP
implementation.

## Features

-   Circuitry modeling based on original schematics
-   Custom LookAndFeel implementation
-   Filmstrip-based rotary knobs
-   SVG-based fader and switch rendering
-   Host automation compatibility via APVTS / state-saving via XML
-   Parameter smoothing for zipper control

## Technologies Used

-   C++
-   JUCE Framework

## Purpose

This project demonstrates:

-   JUCE plugin architecture
-   Object-oriented programming
-   UI/UX design
-   Real-time modeled DSP

It serves as a portfolio example of modern C++ audio plugin development and object-oriented programming principles.

## Completed:

-   Basic UI
-   Parameters
-   Control Modulation function generator
-   Treatments (Vibrato, Phasing 1, Phasing 2, Waa, Waw, Meow)
-   Top Boost
-   Attack/Decay detector
-   Sub Octave + Buzz
-   Ramp Oscillator
-   Growl Modulation
-   Fuzz
-   Ring Modulator
-   Cleanup / Refactor

## TODO:

-   UI overhaul (OpenGL?)

## Notes

The EMS Synthi Hi-Fli name and original hardware design belong to their
respective owners.
