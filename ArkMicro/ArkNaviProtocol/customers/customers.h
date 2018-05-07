#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Shenglong.h"
#include "YCZX_BMW.h"
#include "YCZX_AUDI.h"
#include "XZD.h"
#include "YXCX.h"
#include "YAOXI.h"
#include "ark169.h"

static const struct CustomerProtocol* customer_array[] = {
    &ark169,
    &shenglong,
    &yczx_bmw,
    &yczx_audi,
    &xzd,
    &yxcx,
    &yaoxi,
};

#endif //CUSTOMER_H
