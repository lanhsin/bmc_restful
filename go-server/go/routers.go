package restapi

import (
	"fmt"
	"log"
	"net/http"
	"sync"
	"time"

	"github.com/gorilla/mux"
)

type Route struct {
	Name        string
	Method      string
	Pattern     string
	HandlerFunc http.HandlerFunc
}

type Routes []Route

var writeMutex sync.Mutex

func Middleware(inner http.Handler, name string) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		start := time.Now()
		log.Printf(
			"info: request: %s %s %s",
			r.Method,
			r.RequestURI,
			name,
		)

		if r.TLS == nil || CommonNameMatch(r) {
			log.Printf("trace: acquire server write lock")
			writeMutex.Lock()
			inner.ServeHTTP(NewLoggingResponseWriter(w), r)
			writeMutex.Unlock()
			log.Printf("trace: release server write lock")
		} else {
			WriteRequestError(NewLoggingResponseWriter(w), http.StatusUnauthorized,
				"Authentication Fail with untrusted client cert", []string{}, "")
		}

		log.Printf(
			"info: request: duration %s",
			time.Since(start),
		)
	})
}

func NewRouter() *mux.Router {
	router := mux.NewRouter().StrictSlash(true)
	for _, route := range routes {
		handler := Middleware(route.HandlerFunc, route.Name)

		router.
			Methods(route.Method).
			Path(route.Pattern).
			Name(route.Name).
			Handler(handler)
	}

	return router
}

func Index(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "BMC Restful API v1!")
}

var routes = Routes{
	Route{
		"Index",
		"GET",
		"/v1/",
		Index,
	},

	Route{
		"ConfigHWmonAllGet",
		"GET",
		"/openconfig-platform:components/component=hwmon",
		ConfigHWmonAllGet,
	},

	Route{
		"ConfigHWmonGet",
		"GET",
		"/openconfig-platform:components/component={hwmon_id}",
		ConfigHWmonGet,
	},
}
