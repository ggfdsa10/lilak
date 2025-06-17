#ifndef STDSimTuningManager_HH
#define STDSimTuningManager_HH

#include "LKRun.h"
#include "LKTask.h"
#include "LKParameterContainer.h"

class STDSimTuningManager : public LKTask
{
    public:
        STDSimTuningManager();
        virtual ~STDSimTuningManager(){}

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    private:
};

#endif