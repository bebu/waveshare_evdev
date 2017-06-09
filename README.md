# waveshare_evdev
Inspired from https://github.com/110yd/wshare-touchscreen. All props to him.

He uses some obscure lib which didnt work at all at my box, also i think he forgot BTN_TOUCH synthesis, so i hacked this up.
Only deps libudev and libevdev and a more or less decent gcc

actually touchscreen interface is rather easy, a kernel driver should be rather easy to implement... perhaps when this proofs
not good enough.

