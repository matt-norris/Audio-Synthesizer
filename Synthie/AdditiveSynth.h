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

    void SetNote(CNote* note);

    void SetFreq(double f) { m_sinewave.SetFreq(f); }
    void SetAmplitude(double a) { m_sinewave.SetAmplitude(a); }
    void SetDuration(double d) { m_duration = d; }

    CAdditiveSynth(void);
    ~CAdditiveSynth(void);
private:
    CSineWave   m_sinewave;
    double m_duration;
    double m_time;
    CAR        m_ar;
    double m_freq;

    // Vector to hold the sound def, the set of amplitudes
    std::vector<double> m_sound_def;
    // Vector to hold the harmonics
    std::vector<CSineWave> m_harmonics;
    // Vector to hold the harmonics AR components
    std::vector<CAR> m_har_AR;
};

