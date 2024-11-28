const selfsigned = require("selfsigned");
const os = require("os");

const fs = require("fs/promises");

// Certificate options
const attrs = [{ name: "commonName", value: "localhost" }];

// Function to get all local IP addresses
function getHostnamesAndIPs() {
    const interfaces = os.networkInterfaces();
    const altNames = [
        { type: 2, value: "localhost" },  // DNS name
        { type: 7, ip: "127.0.0.1" },     // IP address
    ]; // Add localhost by default

    for (const iface of Object.values(interfaces)) {
        for (const config of iface) {
            if (config.family === "IPv4" && !config.internal) {
                altNames.push({ type: 7, ip: config.address }); // Add IP address
            }
        }
    }

    return altNames;
}


const options = {
    days: 365, // Validity
    altNames: getHostnamesAndIPs(), // Subject Alternative Names
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
