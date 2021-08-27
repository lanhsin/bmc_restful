package restapi

import (
    "strings"
)

var trustedCertCommonNames []string

func Initialise() {
    InitialiseVariables()
}

func InitialiseVariables() {
    trustedCertCommonNames = strings.Split(*ClientCertCommonNameFlag, ",")
}