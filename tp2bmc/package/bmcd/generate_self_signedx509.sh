#!/bin/sh

echo "generating new self signed X509 certs.."

ssl_dir=/etc/ssl/certs/
mkdir -p $ssl_dir
openssl req -x509 -newkey rsa:4096 -keyout $ssl_dir/bmcd_key.pem \
    -out $ssl_dir/bmcd_cert.pem -nodes -subj "/CN=Turing-Pi self signed"

echo "Done"
