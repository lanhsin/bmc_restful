package restapi

import "flag"

var LogLevelFlag = flag.String("loglevel", "info", "Set's minimum log level, valid values are: trace, debug, info, warning, error, alert")
var LogFileFlag = flag.String("logfile", "/dev/stderr", "Set's the output for the log")
var HttpFlag = flag.Bool("enablehttp", true, "Enable http endpoint")
var HttpsFlag = flag.Bool("enablehttps", false, "Enable https endpoint")
var ClientCertFlag = flag.String("clientcert", "", "Client cert file")
var ClientCertCommonNameFlag = flag.String("clientcertcommonname", "SonicCLient", "Comma separated list of trusted common names in the client cert file")
var ServerCertFlag = flag.String("servercert", "", "Server cert file")
var ServerKeyFlag = flag.String("serverkey", "", "Server key file")