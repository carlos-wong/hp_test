#ifndef BUSINESSLOGICUTILITY_H
#define BUSINESSLOGICUTILITY_H

#include "AutoConnect.h"
#include <QObject>
#include <QScopedPointer>

class BusinessLogicUtilityPrivate;
class BusinessLogicUtility
{
    Q_DISABLE_COPY(BusinessLogicUtility)
#ifdef g_BusinessLogicUtility
#undef g_BusinessLogicUtility
#endif
#define g_BusinessLogicUtility (BusinessLogicUtility::instance())
public:
    inline static BusinessLogicUtility* instance() {
        static BusinessLogicUtility businessLogicUtility;
        return &businessLogicUtility;
    }
private:
    BusinessLogicUtility();
    ~BusinessLogicUtility();
    friend class BusinessLogicUtilityPrivate;
    QScopedPointer<BusinessLogicUtilityPrivate> m_Private;
};

#endif // BUSINESSLOGICUTILITY_H
