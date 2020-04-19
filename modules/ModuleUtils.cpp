#include "ModuleUtils.h"

namespace modules {

DataForwarderBase::DataForwarderBase(miliseconds interval)
    : interval(interval)
{
  timer.setInterval(interval);
  timer.setTimerType(Qt::CoarseTimer);
}

}