#pragma once
#include "AudioNode.h"
class CEffects :
    public CAudioNode
{
public:
    CEffects(void);
    virtual ~CEffects(void);
    virtual void Process(double* frameIn, double* frameOut) = 0;
};

