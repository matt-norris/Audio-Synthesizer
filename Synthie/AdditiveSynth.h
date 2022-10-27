#pragma once
#include "Instrument.h"
#include "Instrument.h"
#include "SineWave.h"
#include "AR.h"
#include <vector>
using namespace std;
class CAdditiveSynth :
    public CInstrument
{
public:
    virtual void Start();
    virtual bool Generate();
    void StartCrossfade(std::vector<CSineWave> crossfade_harmonics);
    void SetNote(CNote* note);

    void SetFreq(double f) { m_sinewave.SetFreq(f); }
    void SetAmplitude(double a) { m_sinewave.SetAmplitude(a); }
    void SetDuration(double d) { m_duration = d; }
    void SetVibratoOn(bool v) { m_vibrato_on = v; }
    CAdditiveSynth(void);
    ~CAdditiveSynth(void);

private:
    CSineWave   m_sinewave;
    double m_duration;
    double m_time;
    double m_freq;
    bool m_sustain;
    bool m_vibrato_on;
    double m_vibrato_depth;
    double m_vibrato_freq;
    bool m_crossfading;
    // Vector to hold the sound def, the set of amplitudes
    std::vector<double> m_sound_def;
    // Vector to hold the cross fade sound def, the set of amplitudes
    std::vector<double> m_sound_def_cross;
    // Vector to hold the harmonics
    std::vector<CSineWave> m_harmonics;
    // Vector to hold the crossfade harmonics
    std::vector<CSineWave> m_crossfade_harmonics;
};

