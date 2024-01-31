package restapi

import (
    "strconv"
    "net/http"
    "github.com/gorilla/mux"
    "cgocommon"
)

func ConfigHWmonAllGet(w http.ResponseWriter, r *http.Request) {
    w.Header().Set("Content-Type", "application/json; charset=UTF-8")

    Hwmon_pt := cgocommon.HwmonAllGet();
    defer Hwmon_pt.Delete()

    Hwmon_string := Hwmon_pt.GetString()
    if len(Hwmon_string) == 0 {
        Hwmon_string = "No device exist"
    }
    w.Header().Set("Content-Length", strconv.Itoa(len(Hwmon_string)))
    WriteRequestResponse(w, Hwmon_string, http.StatusOK)
}

func ConfigHWmonGet(w http.ResponseWriter, r *http.Request) {
    w.Header().Set("Content-Type", "application/json; charset=UTF-8")

    vars := mux.Vars(r)
    var HwmonId string = vars["hwmon_id"]

    Hwmon_pt := cgocommon.HwmonGet(HwmonId);
    defer Hwmon_pt.Delete()

    Hwmon_string := Hwmon_pt.GetString()
    if len(Hwmon_string) == 0 {
        Hwmon_string = "No device exist"
    }
    w.Header().Set("Content-Length", strconv.Itoa(len(Hwmon_string)))
    WriteRequestResponse(w, Hwmon_string, http.StatusOK)
}
