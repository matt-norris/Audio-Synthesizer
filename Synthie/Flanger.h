#pragma once
#include "Effects.h"
#include <vector>
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

    void Clear();

    CFlanger(void);

    virtual ~CFlanger(void);

    void SetUpDelayQueue();

    void SetWet(double w) { m_wet = w; }
    
private:
    double m_wet;
    const int QUEUESIZE = int(GetSampleRate() * 10);
    double m_var_delay = .25;
    double m_sample_delay;
    // Create queue to hold delayed samples
    std::vector<double> m_queue;


    int m_wrloc;
    int m_rdloc;
    bool m_on;
    double m_x;
    double m_freq;
};

