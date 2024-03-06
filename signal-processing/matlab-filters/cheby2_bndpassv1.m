function Hd = cheby2_bndpassv1
%CHEBY2_BNDPASSV1 Returns a discrete-time filter object.

% MATLAB Code
% Generated by MATLAB(R) 9.14 and DSP System Toolbox 9.16.
% Generated on: 09-Jan-2024 04:20:58

% Chebyshev Type II Bandpass filter designed using FDESIGN.BANDPASS.

% All frequency values are in Hz.
Fs = 192;  % Sampling Frequency

Fstop1 = 0.1;         % First Stopband Frequency
Fpass1 = 0.5;         % First Passband Frequency
Fpass2 = 40;          % Second Passband Frequency
Fstop2 = 41;          % Second Stopband Frequency
Astop1 = 10;          % First Stopband Attenuation (dB)
Apass  = 1;           % Passband Ripple (dB)
Astop2 = 30;          % Second Stopband Attenuation (dB)
match  = 'stopband';  % Band to match exactly

% Construct an FDESIGN object and call its CHEBY2 method.
h  = fdesign.bandpass(Fstop1, Fpass1, Fpass2, Fstop2, Astop1, Apass, ...
                      Astop2, Fs);
Hd = design(h, 'cheby2', 'MatchExactly', match);

% [EOF]