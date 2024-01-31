package restapi

import (
    "encoding/json"
    "io/ioutil"
    "log"
    "net/http"
)

func WriteRequestError(w http.ResponseWriter, code int, message string, fields []string, details string) {
    e := ErrorInner{
        Code:    code,
        Message: message,
        Fields:  fields,
        Details: details,
    }

    b, err := json.Marshal(ErrorModel{e})
    if err != nil {
        w.WriteHeader(http.StatusInternalServerError)
        b = []byte(`
{
  "error": {
    "code": 500,
    "message": "Internal service error"
  }
}`)
    } else {
        w.WriteHeader(code)
    }

    log.Printf(
        "error: Request ends with error message %s",
        b,
    )

    w.Write(b)
}


func WriteRequestErrorWithSubCode(w http.ResponseWriter, code int, sub_code int, message string, fields []string, details string) {
    e := ErrorInner{
        Code:     code,
        SubCode: &sub_code,
        Message:  message,
        Fields:   fields,
        Details:  details,
    }

    b, err := json.Marshal(ErrorModel{e})
    if err != nil {
        w.WriteHeader(http.StatusInternalServerError)
        b = []byte(`
{
  "error": {
    "code": 500,
    "message": "Internal service error"
  }
}`)
    } else {
        w.WriteHeader(code)
    }

    log.Printf(
        "error: Request ends with error message %s",
        b,
    )

    w.Write(b)
}

func WriteJsonError(w http.ResponseWriter, err error) {
    switch t := err.(type) {
    case *json.SyntaxError:
        WriteRequestError(w, http.StatusBadRequest, "Malformed arguments for API call", []string{}, "Invalid character in JSON")
    case *json.UnmarshalTypeError:
        WriteRequestError(w, http.StatusBadRequest, "Malformed arguments for API call", []string{t.Field}, "JSON field does not match required type")
    case *MissingValueError:
        WriteRequestError(w, http.StatusBadRequest, "Malformed arguments for API call", []string{t.Field}, "Missing JSON field")
    case *InvalidFormatError:
        WriteRequestError(w, http.StatusBadRequest, "Malformed arguments for API call", []string{t.Field}, t.Message)
    default:
        WriteRequestError(w, http.StatusBadRequest, "Malformed arguments for API call", []string{}, "Failed to decode JSON")
    }
}

/*func WriteRequestResponse(w http.ResponseWriter, jsonObj interface{}, code int) {
    b, err := json.Marshal(jsonObj)
    if err != nil {
        WriteRequestError(w, http.StatusInternalServerError, "Internal service error", []string{}, "")
    } else {
        w.WriteHeader(code)
        w.Write(b)
    }
}*/

func WriteRequestResponse(w http.ResponseWriter, str string, code int) {
    var b []byte = []byte(str)

    w.WriteHeader(code)
    w.Write(b)
}


func ReadJSONBody(w http.ResponseWriter, r *http.Request, attr interface{}) error {
    body, err := ioutil.ReadAll(r.Body)
    if err != nil {
        WriteRequestError(w, http.StatusInternalServerError, "Internal service error", []string{}, "")
        return err
    }

    log.Printf(
        "debug: request: body: %s",
        body,
    )

    err = json.Unmarshal(body, attr)
    if err != nil {
        WriteJsonError(w, err)
        return err
    }

    return nil
}

