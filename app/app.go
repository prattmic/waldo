package app

import (
	"fmt"
	"net/http"

	pb "github.com/prattmic/waldo/proto/location"
)

func handler(w http.ResponseWriter, r *http.Request) {
	var loc pb.Location
	fmt.Fprint(w, "Hello, world!: ", loc)
}

func init() {
	http.HandleFunc("/", handler)
}
