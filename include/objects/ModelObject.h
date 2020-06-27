#pragma once
#include <objects/SkipObject.h>


namespace Skip {

    class ModelObject : public SkipObject
    {
    public:
        ModelObject();
        ~ModelObject();

        std::string _modelPath;

        void loadModel();
    private:

    };
}

