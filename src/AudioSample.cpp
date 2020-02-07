/*
    Simple Wave File
    Author: Daniel Schwartz

    -- AudioSample --

    The AudioSample struct provides an easy way for working with
    and manipulating raw audio data. This is defined as a struct
    and not a class because the goal is to be as flexible as possible
    in how audio data can be manipulated, so it makes sense to keep
    everything public.
*/

#include "AudioSample.h"

AudioSample::AudioSample(): left(0), right(0) {}

// constructor for a mono sample
AudioSample::AudioSample(double mono): left(mono), right(0) {
    clampValues();
}

// constructor for a stereo sample
AudioSample::AudioSample(double left, double right): left(left), right(right) {
    clampValues();
}

// this method is used to clamp between 1 and -1
AudioSample& AudioSample::clampValues() {
    left = clamp(left, -1.0, 1.0);
    right = clamp(right, -1.0, 1.0);
    return *this;
}

// easy way to invert the sample values
AudioSample& AudioSample::invert() {
    left *= -1;
    right *= -1;
    return *this;
}

// these overloaded operators are for convenience when doing processing
AudioSample operator+(const AudioSample &a, const AudioSample &b) {
    return AudioSample(a.left + b.left, a.right + b.right).clampValues();
}
AudioSample operator+(const AudioSample &a, double b) {
    return AudioSample(a.left + b, a.right + b).clampValues();
}
AudioSample operator+(double a, const AudioSample &b){
    return b + a;
}

AudioSample operator-(const AudioSample &a, const AudioSample &b) {
    return AudioSample(a.left - b.left, a.right - b.right).clampValues();
}
AudioSample operator-(const AudioSample &a, double b){
    return AudioSample(a.left - b, a.right - b).clampValues();
}
AudioSample operator-(double a, const AudioSample &b){
    return b - a;
}

AudioSample operator*(const AudioSample &a, const AudioSample &b) {
    return AudioSample(a.left * b.left, a.right * b.right).clampValues();
}
AudioSample operator*(const AudioSample &a, double b){
    return AudioSample(a.left * b, a.right * b).clampValues();
}
AudioSample operator*(double a, const AudioSample &b){
    return b * a;
}

// use of the overloaded division operator should be avoided when possible
// because it is verrry easy to accidentally divide by zero
// these implementations are designed to be forgiving if that happens
AudioSample operator/(const AudioSample &a, const AudioSample &b) {
    // return an empty audio sample rather than ending the program
    if (b.left == 0 || b.right == 0) {
        cout << "Zero division error" << endl;
        return AudioSample();
    }
    return AudioSample(a.left / b.left, a.right / b.right).clampValues();
}
AudioSample operator/(const AudioSample &a, double b) {
    // return an empty audio sample rather than ending the program
    if (b == 0) {
        cout << "Zero division error" << endl;
        return AudioSample();
    }
    return AudioSample(a.left / b, a.right / b).clampValues();
}
AudioSample operator/(double a, const AudioSample &b) {
    return b / a;
}
