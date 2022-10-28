#pragma once
#include "AudioNode.h"
class CSineWave :
    public CAudioNode
{
public:
    //! Start audio generation
    virtual void Start();

    //! Generate one frame of audio
    virtual bool Generate();

    //! Set the sine wave frequency
    void SetFreq(double f) { m_freq = f; }

    //! Set the fundamental sine wave frequency
    void SetFundamentalFreq(double f) { m_fundamental_freq = f; }

    //! Set the sine wave amplitude
    void SetAmplitude(double a) { m_amp = a; }

    //! Set the fundamental sine wave amp
    void SetFundamentalAmplitude(double a) { m_fundamental_amp = a; }

    //! Set the sine wave frequency
    double GetFreq() { return m_freq; }

    //! Set the sine wave amplitude
    double GetAmplitude() { return m_amp; }

    //! Get the fundamentalsine wave frequency
    double GetFundamentalFreq() { return m_fundamental_freq; }

    //! Set the  fundamental sine wave amp
    double GetFundamentalAmplitude() { return m_fundamental_amp; }

    //! Constructor
    CSineWave(void);

    virtual ~CSineWave(void);

private:
    double m_freq;
    double m_amp;
    double m_phase;
    double m_fundamental_freq;
    double m_fundamental_amp;
};

