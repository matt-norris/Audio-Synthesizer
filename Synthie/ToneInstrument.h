#pragma once
#include "Instrument.h"
#include "SineWave.h"
#include "AR.h"

class CToneInstrument :
    public CInstrument
{
public:
    virtual void Start();
    virtual bool Generate();

    void SetNote(CNote* note);

    void SetFreq(double f) { m_sinewave.SetFreq(f); }
    void SetAmplitude(double a) { m_sinewave.SetAmplitude(a); }
    void SetDuration(double d) { m_duration = d; }

    CToneInstrument(void);
    ~CToneInstrument(void);
private:
    CSineWave   m_sinewave;
    double m_duration;
    double m_time;
    CAR        m_ar;
};

