//
// Created by saransh on 7/13/25.
//

#include "serial_io.h"

namespace Cloverwatch {

    template class Serial_DMAasync<DEVICE_DT_GET(DT_NODELABEL(uart1)), 1024, 1024, 4>;

}