#!/bin/sh
# shellcheck shell=sh

ssl_dir=/mnt/c/Users/gert-jan.timmer/Projects/wip/ssl
cert_file="${ssl_dir}/bmcd_cert.pem"
key_file="${ssl_dir}/bmcd_key.pem"

# Generate new self-signed X509 certs.
#
# Generate a new self-signed X509 cert and private key for use by BMCD.
# This will overwrite any existing certs, so be careful when using.
generate_certs() {
    echo "Generating new self-signed X509 certs.."
    mkdir -p $ssl_dir
    openssl req -x509 -newkey rsa:4096 -keyout $key_file \
        -out $cert_file -nodes -subj "/CN=Turing-Pi self signed"
    echo "Done"
}

# Check if cert and key exist
if [ -f "$cert_file" ] && [ -f "$key_file" ]; then
    # Check if either file is 0 bytes
    if [ ! -s "$cert_file" ] || [ ! -s "$key_file" ]; then
        echo "One of the files is empty. Regenerating certs.."
        rm -f "$cert_file" "$key_file"
        generate_certs
    else
        # Validate cert and key belong together as a pair
        cert_modulus=$(openssl x509 -noout -modulus -in "$cert_file" | openssl md5)
        key_modulus=$(openssl rsa -noout -modulus -in "$key_file" | openssl md5)
        if [ "$cert_modulus" = "$key_modulus" ]; then
            echo "Cert and key are valid and belong together."
        else
            echo "Cert and key do not match. Regenerating certs.."
            rm -f "$cert_file" "$key_file"
            generate_certs
        fi
    fi
else
    # If only one of them exists, assume error and regenerate
    if [ -f "$cert_file" ] || [ -f "$key_file" ]; then
        echo "Only one of the cert or key exists. Regenerating certs.."
        rm -f "$cert_file" "$key_file"
    fi
    generate_certs
fi

exit 0
