#pragma once
#include "Instrument.h"
class CEffects :
    public CInstrument
{
public:
    CEffects(void);
    virtual ~CEffects(void);
    virtual void Process(double* frameIn, double* frameOut) = 0;
    virtual void SetNote(CNote* note) = 0;
};

