#pragma once
#include "AudioNode.h"

class CAR :
    public CAudioNode
{
public:
    //! Constructor
    CAR(void);

    virtual ~CAR(void);

    void SetSource(CAudioNode* node);
    void SetDuration(double dur);
    void Start();
    bool Generate();

private:
    double m_attack;
    double m_release;
    double m_duration;
    double m_time;
    CAudioNode* m_source;
};

