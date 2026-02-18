# EMS Synthi Hi-Fli (JUCE)

A software recreation of the **EMS Synthi Hi-Fli**, built as a
standalone application and audio plugin using **JUCE**.\
This project focuses on faithfully replicating the original hardware
control layout while implementing a scalable parameter
architecture for future DSP development.

## Overview

The EMS Synthi Hi-Fli was a distinctive guitar effects processor known
for its modulation-based treatments, routing flexibility, and
unconventional control structure.

This project recreates the original panel layout and control behavior in
software form.\
The current development stage focuses on user interface implementation,
parameter management, and real-time smoothing infrastructure. DSP
processing is currently under development.

## Features

-   Full hardware-inspired control layout (1000 × 658)
-   Custom LookAndFeel implementation
-   Filmstrip-based rotary knobs
-   SVG-based fader and switch rendering
-   Host automation compatibility via APVTS
-   Parameter smoothing for zipper control

## Parameter Architecture

The plugin uses **AudioProcessorValueTreeState (APVTS)** for:

-   Parameter definition
-   State management
-   Host recall and automation
-   Scalable control mapping

Continuous parameters operate primarily on a 0--100% scale and are
internally normalized for DSP use.\
Output gain is defined in decibels and converted to linear gain
internally.

All continuous parameters use `juce::LinearSmoothedValue` with a 20ms
smoothing duration to ensure click-free automation and real-time
stability.

## Custom LookAndFeel

The interface uses a fully custom rendering system:

-   128x128 filmstrip-based rotary knobs (101 frames)
-   Optional notched knob variants
-   SVG-based fader and switch thumbs
-   Property-driven UI styling (position, switch mode, sizing)

The editor uses a fixed layout to preserve the original panel
proportions and visual structure.

## Technologies Used

-   C++
-   JUCE Framework
-   AudioProcessorValueTreeState
-   Custom LookAndFeel implementation
-   Filmstrip rendering
-   SVG asset integration
-   Parameter smoothing techniques

## Purpose

This project demonstrates:

-   JUCE plugin architecture
-   Custom UI rendering systems
-   Object-oriented programming
-   Parameter state management
-   Real-time smoothing
-   Hardware-inspired software recreation

It serves as a portfolio example of modern C++ audio plugin development and object-oriented programming principles.

## Status

-   UI complete\
-   Parameter system implemented\
-   Smoothing infrastructure complete\
-   DSP implementation in progress

## Notes

This project is an independent software recreation for educational and
developmental purposes.\
The EMS Synthi Hi-Fli name and original hardware design belong to their
respective owners.
