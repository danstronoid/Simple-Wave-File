#ifndef AUDIOSAMPLE_H_INCLUDED
#define AUDIOSAMPLE_H_INCLUDED

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

#include <iostream>
#include <algorithm>
using namespace std;

struct AudioSample {
    double left{};
    double right{};

    AudioSample();
    AudioSample(double mono);
    AudioSample(double left, double right);

    // this method is used to clamp between 1 and -1
    AudioSample& clampValues();

    // easy way to invert the sample values
    AudioSample& invert();
};

// these overloaded operators are for convenience when doing processing
AudioSample operator+(const AudioSample &a, const AudioSample &b);
AudioSample operator+(const AudioSample &a, double b);
AudioSample operator+(double a, const AudioSample &b);

AudioSample operator-(const AudioSample &a, const AudioSample &b);
AudioSample operator-(const AudioSample &a, double b);
AudioSample operator-(double a, const AudioSample &b);

AudioSample operator*(const AudioSample &a, const AudioSample &b);
AudioSample operator*(const AudioSample &a, double b);
AudioSample operator*(double a, const AudioSample &b);

// use of the overloaded division operator should be avoided when possible
// because it is verrry easy to accidentally divide by zero
// these implementations are designed to be forgiving if that happens
AudioSample operator/(const AudioSample &a, const AudioSample &b);
AudioSample operator/(const AudioSample &a, double b);
AudioSample operator/(double a, const AudioSample &b);


#endif // AUDIOSAMPLE_H_INCLUDED
