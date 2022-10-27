#pragma once
#include "Effects.h"
class CFlanger :
    public CEffects
{
public:
    //! Start the node generation
    virtual void Start();

    //! Cause one sample to be generated
    virtual bool Generate();

    CFlanger(void);
    virtual ~CFlanger(void);
    
};
