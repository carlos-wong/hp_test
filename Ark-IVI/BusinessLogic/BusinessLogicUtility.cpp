#include "BusinessLogicUtility.h"
#include <QTimer>

class BusinessLogicUtilityPrivate
{
    Q_DISABLE_COPY(BusinessLogicUtilityPrivate)
public:
    explicit BusinessLogicUtilityPrivate(BusinessLogicUtility *parent);
    ~BusinessLogicUtilityPrivate();
private:
    BusinessLogicUtility* m_Parent;
};

BusinessLogicUtility::BusinessLogicUtility()
    : m_Private(new BusinessLogicUtilityPrivate(this))
{
}

BusinessLogicUtility::~BusinessLogicUtility()
{
}

BusinessLogicUtilityPrivate::BusinessLogicUtilityPrivate(BusinessLogicUtility *parent)
    : m_Parent(parent)
{
}

BusinessLogicUtilityPrivate::~BusinessLogicUtilityPrivate()
{
}
