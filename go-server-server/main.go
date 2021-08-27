package main

import (
    // WARNING!
    // Change this to a fully-qualified import path
    // once you place this file into your project.
    // For example,
    //
    //    sw "github.com/myname/myrepo/go"
    //
    "github.com/vharitonsky/iniflags"
    sw "go-server-server/go"
    "log"
    "net/http"
    "crypto/tls"
    "crypto/x509"
    "io/ioutil"
    "os"
    "os/signal"
    "syscall"
    "context"
    "sync"
    "time"
)

const CERT_MONITOR_FREQUENCY = 3600 * time.Second

func StartHttpServer(handler http.Handler) {
    log.Printf("info: http endpoint started")
    log.Fatal(http.ListenAndServe(":8093", handler))
}

// prepareServerCertificate function parses --cert and --key parameter
// values. Both cert and private key PEM files are loaded  into a
// tls.Certificate objects. Exits the process if files are not
// specified or not found or corrupted.
func prepareServerCertificate() []tls.Certificate {
	if *sw.ServerCertFlag == "" {
		log.Fatalf("Server certificate file not specified")
	}

	if *sw.ServerKeyFlag == "" {
		log.Fatalf("Server private key file not specified")
	}

	log.Printf("info: Server certificate file: %s", *sw.ServerCertFlag)
	log.Printf("info: Server private key file: %s", *sw.ServerKeyFlag)

	certificate, err := tls.LoadX509KeyPair(*sw.ServerCertFlag, *sw.ServerKeyFlag)
	if err != nil {
		log.Fatalf("Failed to load server cert/key -- ", err)
	}

	return []tls.Certificate{certificate}
}

// prepareCACertificates function parses --ca parameter, which is the
// path to CA certificate file. Loads file contents to a x509.CertPool
// object. Returns nil if file name is empty (not specified). Exists
// the process if file path is invalid or file is corrupted.
func prepareCACertificates() *x509.CertPool {
	if *sw.ClientCertFlag == "" { // no CA file..
		return nil
	}

	log.Printf("info: Client CA certificate file: %s", *sw.ClientCertFlag)

	caCert, err := ioutil.ReadFile(*sw.ClientCertFlag)
	if err != nil {
		log.Fatalf("Failed to load CA certificate file -- ", err)
	}

	caPool := x509.NewCertPool()
	ok := caPool.AppendCertsFromPEM(caCert)
	if !ok {
		log.Fatalf("Invalid CA certificate")
	}

	return caPool
}

// getTLSClientAuthType function parses the --client_auth parameter.
// Returns corresponding tls.ClientAuthType value. Exits the process
// if value is not valid ('none', 'cert' or 'auth')
func getTLSClientAuthType() tls.ClientAuthType {

    if *sw.ClientCertFlag == "" { // no CA file..
		return tls.RequestClientCert
	} else {
		return tls.RequireAndVerifyClientCert
    }
}

func getPreferredCurveIDs() []tls.CurveID {
	return []tls.CurveID{
		tls.CurveP521,
		tls.CurveP384,
		tls.CurveP256,
	}
}

func getPreferredCipherSuites() []uint16 {
	return []uint16{
		tls.TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
		tls.TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
		tls.TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305,
		tls.TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305,
		tls.TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
		tls.TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
	}
}

func StartHttpsServer(handler http.Handler, messenger <-chan int, wgroup *sync.WaitGroup) {
    defer wgroup.Done()
    for {
        // Setup HTTPS client cert the server trust and validation policy
        tlsConfig := tls.Config{
            ClientAuth:               getTLSClientAuthType(),
            Certificates:             prepareServerCertificate(),
            ClientCAs:                prepareCACertificates(),
            MinVersion:               tls.VersionTLS12,
            CurvePreferences:         getPreferredCurveIDs(),
            PreferServerCipherSuites: true,
            CipherSuites:             getPreferredCipherSuites(),
        }
        tlsConfig.BuildNameToCertificate()

        server := &http.Server{
            Addr:      ":8083",
            Handler:   handler,
            TLSConfig: &tlsConfig,
        }

        log.Printf("info: https endpoint started")

        // Listening should happen in a go-routine to prevent blocking
        go func() {
            if err := server.ListenAndServeTLS("", ""); err != nil {
                log.Println(err)
            }
        }()

        value := <-messenger
        log.Printf("info: HTTPS Signal received. Shutting down...")
        if err := server.Shutdown(context.Background()); err != nil {
            log.Printf("trace: HTTPS server Shutdown: %v", err)
        } else {
            log.Printf("info: HTTPS Server shutdown successful!")
        }
        switch value {
        case 0:
            // Signal from signal_handler
            log.Printf("info: Terminating...")
            os.Exit(0)
        }
    }
}

func signal_handler(messenger chan<- int, wgroup *sync.WaitGroup) {
    defer wgroup.Done()
    sigchannel := make(chan os.Signal, 1)
    signal.Notify(sigchannel,
        syscall.SIGTERM,
        syscall.SIGQUIT)

    <-sigchannel
    messenger <- 0
    return
}

func monitor_certs(messenger chan<- int, wgroup *sync.WaitGroup) {
    defer wgroup.Done()

    var prev_client_cert_mtime, prev_server_cert_mtime, prev_sever_key_mtime time.Time
    if *sw.ClientCertFlag != "" {
        client_cert_finfo, _ := os.Lstat(*sw.ClientCertFlag)
        prev_client_cert_mtime := client_cert_finfo.ModTime()
	    log.Printf("trace: Last modified time of %s is %d", client_cert_finfo.Name(), prev_client_cert_mtime.Unix())
    }

    if *sw.ServerCertFlag != "" {
        server_cert_finfo, _ := os.Lstat(*sw.ServerCertFlag)
        prev_server_cert_mtime := server_cert_finfo.ModTime()
        log.Printf("trace: Last modified time of %s is %d", server_cert_finfo.Name(), prev_server_cert_mtime.Unix())
    }

    if *sw.ServerKeyFlag != "" {
        sever_key_finfo, _ := os.Lstat(*sw.ServerKeyFlag)
        prev_sever_key_mtime := sever_key_finfo.ModTime()
        log.Printf("trace: Last modified time of %s is %d", sever_key_finfo.Name(), prev_sever_key_mtime.Unix())
    }

    time.Sleep(CERT_MONITOR_FREQUENCY)

    for {
        reload := false
        if *sw.ClientCertFlag != "" {
            client_cert_finfo, _ := os.Lstat(*sw.ClientCertFlag)
            client_cert_mtime := client_cert_finfo.ModTime()
            log.Printf("trace: Last modified time of %s is %d", client_cert_finfo.Name(), client_cert_mtime.Unix())
            if client_cert_mtime != prev_client_cert_mtime {
                log.Printf("info: Last modified time of %s changed from %d to %d", client_cert_finfo.Name(), prev_client_cert_mtime.Unix(), client_cert_mtime.Unix())
                reload = true
            }
            prev_client_cert_mtime = client_cert_mtime
    }

    if *sw.ServerCertFlag != "" {
        server_cert_finfo, _ := os.Lstat(*sw.ServerCertFlag)
        server_cert_mtime := server_cert_finfo.ModTime()
        log.Printf("trace: Last modified time of %s is %d", server_cert_finfo.Name(), server_cert_mtime.Unix())
        if server_cert_mtime != prev_server_cert_mtime {
            log.Printf("info: Last modified time of %s changed from %d to %d", server_cert_finfo.Name(), prev_server_cert_mtime.Unix(), server_cert_mtime.Unix())
            reload = true
        }
        prev_server_cert_mtime = server_cert_mtime
    }

    if *sw.ServerKeyFlag != "" {
        sever_key_finfo, _ := os.Lstat(*sw.ServerKeyFlag)
        sever_key_mtime := sever_key_finfo.ModTime()
        log.Printf("trace: Last modified time of %s is %d", sever_key_finfo.Name(), sever_key_mtime.Unix())
        if sever_key_mtime != prev_sever_key_mtime {
            log.Printf("info: Last modified time of %s changed from %d to %d", sever_key_finfo.Name(), prev_sever_key_mtime.Unix(), sever_key_mtime.Unix())
            reload = true
        }
        prev_sever_key_mtime = sever_key_mtime
    }

        if reload == true {
            log.Printf("info: Certs have rolled! Reload needed!")
            messenger <- 1
        }
        time.Sleep(CERT_MONITOR_FREQUENCY)
    }
}

func main() {
    iniflags.Parse()

    sw.InitLogging()
    var wgroup sync.WaitGroup
    var messenger = make(chan int, 1)

    log.Printf("info: server started")

    sw.Initialise()
    router := sw.NewRouter()

    if (!*sw.HttpFlag && !*sw.HttpsFlag) {
        log.Fatal("Both http and http endpoints are disabled.")
    }

    if (*sw.HttpFlag) {
        go StartHttpServer(router)
    }

    if (*sw.HttpsFlag) {
        wgroup.Add(1)
        go StartHttpsServer(router, messenger, &wgroup)

        wgroup.Add(1)
        go monitor_certs(messenger, &wgroup)
    }

    wgroup.Add(1)
    go signal_handler(messenger, &wgroup)

    wgroup.Wait()
}
