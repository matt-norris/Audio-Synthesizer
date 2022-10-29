#pragma once
#include "Effects.h"
class CNoiseGate :
    public CEffects
{
public:
    //! Start the node generation
    virtual void Start() {};

    //! Cause one sample to be generated
    virtual bool Generate() { return false; }


    void SetNote(CNote* note);

    virtual void Process(double* frameIn, double* frameOut);

    CNoiseGate(void);

    virtual ~CNoiseGate(void);
private:
   double m_threshold;
};

