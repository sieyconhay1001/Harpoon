#include "IMatchFramework.h"
#include "UIEngine.h"
class IPanoramaUIEngine : IAppSystem
{
public:
    virtual void SetupUIEngine(void);
    virtual void ShutdownUIEngine(void);
    virtual UIEngine* AccessUIEngine(void);
    virtual void HandleInputEvent();
    virtual void unk1(); // calls this function in PanoramaUIEngine -- call    qword ptr [rax+0A0h]
    virtual void unk2();
};