#ifndef ARKPROTOCOLWRAPPER_H
#define ARKPROTOCOLWRAPPER_H

#include "ArkNaviProtocol.h"

#define ArkProtocolWrapperHandler(flag) const_cast<ArkProtocolWrapper&>(protocol).handler = flag;

class ArkProtocolWrapper
{
public:
    ArkProtocolWrapper();
    explicit ArkProtocolWrapper(const struct ArkProtocol& protocol);
    explicit ArkProtocolWrapper(const ArkProtocolWrapper& protocol);
    ~ArkProtocolWrapper();
    enum ArkType type;
    enum ArkDirection direction;
    void* data;
    bool handler;
    bool filter;
private:
    unsigned char count;
};

struct ArkCar* arkProtocolWrapperToArkCar(const ArkProtocolWrapper &protocol);
struct ArkRadio* arkProtocolWrapperToArkRadio(const ArkProtocolWrapper &protocol);
struct ArkKey* arkProtocolWrapperToArkKey(const ArkProtocolWrapper &protocol);
struct ArkControl* arkProtocolWrapperToArkControl(const ArkProtocolWrapper &protocol);
struct ArkBlueTooth* arkProtocolWrapperToArkBluetooth(const ArkProtocolWrapper &protocol);
#endif // ARKPROTOCOLWRAPPER_H
