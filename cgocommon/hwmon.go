package cgocommon

// #cgo LDFLAGS: -lcgocommon -lstdc++
// #include <capi/hwmon.h>
// #include <stdlib.h>
import "C"

import (
    "unsafe"
)

import "fmt"

type HwmonConnector struct {
    ptr unsafe.Pointer
}

func HwmonAllGet() HwmonConnector {
    hwmonC := C.hwmon_all_get()
    fmt.Print("HwmonAllGet\n")
    return HwmonConnector{ptr: unsafe.Pointer(hwmonC)}
}

func HwmonGet(hwmonId string) HwmonConnector {
    hwmonIdC := C.CString(hwmonId)
    defer C.free(unsafe.Pointer(hwmonIdC))
    hwmonC := C.hwmon_get(hwmonIdC)
    fmt.Print("HwmonGet\n")
    return HwmonConnector{ptr: unsafe.Pointer(hwmonC)}
}

func (hwmon HwmonConnector) GetString() string {
    return C.GoString(C.hwmon_connector_t(hwmon.ptr))
}

func (hwmon HwmonConnector) Delete() {
    fmt.Print("hwmon Delete\n")
    C.hwmon_connector_delete(C.hwmon_connector_t(hwmon.ptr))
}

