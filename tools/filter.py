#!/usr/bin/env python

import argparse

from numpy import sin, arange, pi
from scipy.signal import lfilter, firwin

def plot_signals(sample_rate, numtaps, fir_coeff):
    from matplotlib.pylab import axes, plot, title, xlabel, show, figure, grid

    # create the input signal
    nsamples = 320

    F_1KHz = 100.
    A_1KHz = 1.0

    F_15KHz = 9000.
    A_15KHz = 0.5

    t = arange(nsamples) / sample_rate
    signal = A_1KHz * sin(2*pi*F_1KHz*t) + A_15KHz*sin(2*pi*F_15KHz*t)

    # Use lfilter to filter the signal with the FIR filter
    filtered_signal = lfilter(fir_coeff, 1.0, signal)

    # The first N-1 samples are "corrupted" by the initial conditions
    warmup = numtaps - 1

    # The phase delay of the filtered signal
    delay = (warmup / 2) / sample_rate

    # Plot the signals
    figure(1)
    plot(t, signal)
    # Plot just the "good" part of the filtered signal.  The first N-1
    # samples are "corrupted" by the initial conditions.
    plot(t[warmup:]-delay, filtered_signal[warmup:], 'g', linewidth=4)
    grid(True)
    show()


def main(args):
    # Create a FIR filter
    sample_rate = 16000.

    # The Nyquist rate of the signal.
    nyq_rate = sample_rate / 2.

    # The cutoff frequency of the filter: 6KHz
    cutoff_hz = 6000.0

    # Length of the filter (number of coefficients, i.e. the filter order + 1)
    numtaps = 23

    # Use firwin to create a lowpass FIR filter
    fir_coeff = firwin(numtaps, cutoff_hz/nyq_rate)

    print("static const float low_pass_coefficients[NUMBER_OF_COEFFICIENTS] = {%s}"
          % ', '.join(["%+.10f" % x for x in fir_coeff]))

    if args.plot:
        plot_signals(sample_rate, numtaps, fir_coeff)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate FIR filter coefficients.")
    parser.add_argument('--plot', action="store_true", help="Plot the input and output signals.")
    args = parser.parse_args()
    main(args)
