const selfsigned = require("selfsigned");

const fs = require("fs/promises");

// Certificate options
const attrs = [{ name: "commonName", value: "localhost" }];

const options = {
    days: 365, // Validity
    altNames: [
    { type: 2, value: "localhost" },  // DNS name
    { type: 7, ip: "127.0.0.1" },     // IP address
    ],
};

function createCertificate() {
    // Generate the certificate
    return new Promise((resolve, reject) => {
        selfsigned.generate(attrs, options, (err, pems) => {
            if (err) {
                reject(err);
            } 
            resolve({
                key: pems.private,
                cert: pems.cert
            });
        });
    });
}

createCertificate().then((pems) => {
    return Promise.all([
        fs.writeFile("./cert/key.pem", pems.key),
        fs.writeFile("./cert/cert.pem", pems.cert)
    ]);
}).then(() => {
    console.log("Certificate created")
});
