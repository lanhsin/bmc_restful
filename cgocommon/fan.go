package cgocommon

// #cgo LDFLAGS: -lcgocommon  -lstdc++
// #include <capi/fan.h>
// #include <stdlib.h>
import "C"

import (
    "unsafe"
)

import "fmt"

type FanConnector struct {
    ptr unsafe.Pointer
}

func FanGet(fanId string) FanConnector {
    fanIdC := C.CString(fanId)
    defer C.free(unsafe.Pointer(fanIdC))
    fanC := C.fan_get(fanIdC)
    fmt.Print("la\n")
    return FanConnector{ptr: unsafe.Pointer(fanC)}
}

func (fan FanConnector) GetString() string {
    return C.GoString(C.fan_connector_t(fan.ptr))
}

func (fan FanConnector) Delete() {
    fmt.Print("layo\n")
    C.fan_connector_delete(C.fan_connector_t(fan.ptr))
}

