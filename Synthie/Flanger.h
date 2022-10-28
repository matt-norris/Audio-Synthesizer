#pragma once
#include "Effects.h"
class CFlanger :
    public CEffects
{
public:
    //! Start the node generation
    virtual void Start() {};

    //! Cause one sample to be generated
    virtual bool Generate() {return false;}


    void SetNote(CNote* note);

    virtual void Process(double* frameIn, double* frameOut);

    CFlanger(void);

    virtual ~CFlanger(void);

    void SetWet(double w) { m_wet = w; }
    
private:
    double m_wet;
};

