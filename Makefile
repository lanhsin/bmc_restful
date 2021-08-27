.PHONY: all install build libcgocommon clean

GO := /usr/local/go/bin/go
export GOROOT=/usr/local/go
export GOPATH=$(HOME)/go
export GOBIN=$(GOPATH)/bin
export GO111MODULE=on

all: install build

install: build
	/usr/bin/install -D $(GOPATH)/bin/go-server-server debian/ingrasys-restapi/usr/sbin/go-server-server
	/usr/bin/install -D $(GOPATH)/bin/generate_cert debian/ingrasys-restapi/usr/sbin/generate_cert

build: $(GOPATH)/bin/go-server-server $(GOPATH)/bin/generate_cert

$(GOPATH)/bin/go-server-server: libcgocommon $(GOPATH)/src/go-server-server/main.go
	cd $(GOPATH)/src/go-server-server && $(GO) get -v && $(GO) build -v -gcflags "-N -l"

$(GOPATH)/bin/generate_cert:
	mkdir -p 			   $(GOPATH)/src/generate_cert
	cp $(GOROOT)/src/crypto/tls/generate_cert.go $(GOPATH)/src/generate_cert
	cd $(GOPATH)/src/generate_cert && $(GO) build -o $@  generate_cert.go

$(GOPATH)/src/go-server-server/main.go:
	mkdir -p               $(GOPATH)/src
	cp -r go-server-server $(GOPATH)/src/go-server-server
	cp -r cgocommon        $(GOPATH)/src/cgocommon

libcgocommon:
	cd libcgocommon && sudo make install

clean:
	rm -rf $(GOPATH)
