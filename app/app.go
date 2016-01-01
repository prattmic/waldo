package app

import (
	"fmt"
	"io/ioutil"
	"log"
	"net/http"

	"github.com/golang/protobuf/proto"
	pb "github.com/prattmic/waldo/proto/location"
)

func hello(w http.ResponseWriter, r *http.Request) {
	fmt.Fprint(w, "Hello, world!")
}

func echo(w http.ResponseWriter, r *http.Request) {
	if r.Method != "POST" {
		http.Error(w, "Only POST allowed", http.StatusMethodNotAllowed)
		return
	}

	d, err := ioutil.ReadAll(r.Body)
	if err != nil {
		log.Printf("Unable to read request body: %v", err)
		http.Error(w, "Internal Server Error", http.StatusInternalServerError)
		return
	}

	var loc pb.Location
	if err := proto.Unmarshal(d, &loc); err != nil {
		log.Printf("Unable to unmarshal protobuf: %v", err)
		http.Error(w, "Invalid protobuf", http.StatusBadRequest)
		return
	}

	log.Printf("Location: %+v", loc)

	response := pb.Location{
		Latitude:    proto.String(loc.GetLatitude()),
		Longitude:   proto.String(loc.GetLongitude()),
		AltitudeMsl: proto.String(loc.GetAltitudeMsl()),
		GroundSpeed: proto.String(loc.GetGroundSpeed()),
		Heading:     proto.String(loc.GetHeading()),
	}

	d, err = proto.Marshal(&response)
	if err != nil {
		log.Printf("Unable to marshal response: %v", err)
		http.Error(w, "Internal Server Error", http.StatusInternalServerError)
		return
	}

	if _, err = w.Write(d); err != nil {
		log.Printf("Unable to write response: %v", err)
		http.Error(w, "Internal Server Error", http.StatusInternalServerError)
		return
	}
}

func init() {
	http.HandleFunc("/", hello)
	http.HandleFunc("/echo", echo)
}
