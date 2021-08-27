package restapi

type ErrorInner struct {
    Code    int      `json:"code"`
    SubCode *int     `json:"sub-code,omitempty"`
    Message string   `json:"message"`
    Fields  []string `json:"fields,omitempty"`
    Details string   `json:"details,omitempty"`
}

type ErrorModel struct {
    Error ErrorInner `json:"error"`
}

type MissingValueError struct {
    Field string
}

type InvalidFormatError struct {
    Field   string
    Message string
}

func (e *MissingValueError) Error() string {
    return "JSON missing field: " + (*e).Field
}

func (e *InvalidFormatError) Error() string {
    return (*e).Message
}

