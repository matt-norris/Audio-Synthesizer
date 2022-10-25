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

    //! Set the sine wave amplitude
    void SetAmplitude(double a) { m_amp = a; }

    //! Set the sine wave frequency
    double GetFreq() { return m_freq; }

    //! Constructor
    CSineWave(void);

    virtual ~CSineWave(void);

private:
    double m_freq;
    double m_amp;
    double m_phase;
    
};

