clear, clc, close all

load("test.mat")

%% Raw ECG

figure(1)
s = test;
% s = forearmv2 .* (-1);
% fs = 229;
fs = 192;
Ts = 1/fs;
N = length(s);
% n = 0 : N-1;                  % For sample domain
n = (0 : Ts : N*Ts - Ts).';
subplot(2,1,1), plot(n, s)
title("ECG Raw Time Domain")
xlabel("Time (sec)")

S = fft(s);
S(1) = 0;                       % Negate DC offset
fk = fs/N;
f = 0 : fk : N*fk - fk;
subplot(2,1,2), plot(f, abs(S))
title("ECG FFT Frequency Domain")
xlabel("Frequency (Hz)")

%% Filters

figure(2)
lowpass = 0.5;                  % Lowpass frequency (Hz)
highpass = 40;                  % Highpass frequency (Hz)
S((N/2 - round(highpass*N/fs)) : (N/2 + round(highpass*N/fs))) = 0;
S(1 : round(lowpass*N/fs)) = 0;
S(N - round(lowpass*N/fs) : N) = 0;
s_ideal = ifft(S);
subplot(3,1,1), plot(n, real(s_ideal))
title("ECG Ideal Bandpass Filter (F_{low}: 0.5Hz, F_{high}: 40Hz)")
xlabel("Time (sec)")

load("fir_v5_bandpass.mat")
y_bndpass = filter(b,1,s);
% y_bndpass(1:N/2) = -100;
subplot(3,1,2), plot(n, y_bndpass)
title("ECG Bandpass FIR - (F_{low}: 0.5Hz, F_{high}: 40Hz, Order: 224)")
xlabel("Time (sec)")

load("iir_60hz_notch_v5.mat")
y_notch = filter(b,a,s);
subplot(3,1,3), plot(n, y_notch)
title("ECG Notch IIR - (F_{notch}: 60Hz, Order: 2)")
xlabel("Time (sec)")

