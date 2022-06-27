#!/bin/python3
from ast import And
import asyncio
from pydoc import cli
from asyncua import Client, ua
from asyncua.crypto.security_policies import SecurityPolicyBasic256Sha256
from cryptography import x509
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.x509.oid import NameOID
from cryptography.hazmat.primitives import hashes
import datetime
import hashlib
import logging
import os
import socket
from typing import List

logging.basicConfig(level=logging.INFO)
_logger = logging.getLogger('asyncua')

# create directory for certificates
dir_path = os.path.dirname(os.path.realpath(
    __file__)) + "/../build/certificates"
os.makedirs(dir_path, exist_ok=True)
# define path to client application certificate and key
client_cert = dir_path + "/peer-certificate-example-1.pem"
private_key = dir_path + "/peer-private-key-example-1.pem"


def application_to_strings(app):
    result = []
    result.append(('Application URI', app.ApplicationUri))
    optionals = [
        ('Product URI', app.ProductUri),
        ('Application Name', app.ApplicationName.to_string()),
        ('Application Type', str(app.ApplicationType)),
        ('Gateway Server URI', app.GatewayServerUri),
        ('Discovery Profile URI', app.DiscoveryProfileUri),
    ]
    for (n, v) in optionals:
        if v:
            result.append((n, v))
    for url in app.DiscoveryUrls:
        result.append(('Discovery URL', url))
    return result  # ['{}: {}'.format(n, v) for (n, v) in result]


def cert_to_string(der):
    if not der:
        return '[no certificate]'
    try:
        from opcua.crypto import uacrypto
    except ImportError:
        return "{0} bytes".format(len(der))
    cert = uacrypto.x509_from_der(der)
    return uacrypto.x509_to_string(cert)


def endpoint_to_strings(ep: ua.EndpointDescription):
    result = [('Endpoint URL', ep.EndpointUrl)]
    result += application_to_strings(ep.Server)
    result += [
        ('Server Certificate', cert_to_string(ep.ServerCertificate)),
        ('Security Mode', str(ep.SecurityMode)),
        ('Security Policy URI', ep.SecurityPolicyUri)
    ]
    for tok in ep.UserIdentityTokens:
        result += [
            ('User policy', tok.PolicyId),
            ('  Token type', str(tok.TokenType))]
        if tok.IssuedTokenType or tok.IssuerEndpointUrl:
            result += [
                ('  Issued Token type', tok.IssuedTokenType),
                ('  Issuer Endpoint URL', tok.IssuerEndpointUrl)]
        if tok.SecurityPolicyUri:
            result.append(('  Security Policy URI', tok.SecurityPolicyUri))
    result += [
        ('Transport Profile URI', ep.TransportProfileUri),
        ('Security Level', ep.SecurityLevel)]
    return result


async def get_endpoints(url: str) -> List[ua.EndpointDescription]:
    """
    Returns the available endpoints of the server
    """
    client = Client(url=url)
    client.security_policy.Mode = ua.MessageSecurityMode.None_
    try:
        await client.connect_socket()
        await client.send_hello()
        await client.open_secure_channel()
        return await client.get_endpoints()
    except:
        return []
    finally:
        client.disconnect_socket()


async def print_endpoints(url: str) -> None:
    print('Get endpoints')
    endpoints = await get_endpoints(url)
    print('Got endpoints')

    for i, ep in enumerate(endpoints, start=1):
        print('Endpoint {0}:'.format(i))
        for (n, v) in endpoint_to_strings(ep):
            print('  {0}: {1}'.format(n, v))
        print('')


async def cert_gen2(applicationUri,
                   commonName,
                   domainComponent=socket.gethostname(),
                   countryName=u"DE",
                   stateOrProvinceName=None,
                   localityName=None,
                   organizationName=None,
                   password: bytes = b"passphrase"):
    """
    Generates self signed cert
    """
    # Various details about who we are.
    nameAttributes = []
    nameAttributes.append(x509.NameAttribute(NameOID.COMMON_NAME, commonName))
    if domainComponent != None and domainComponent != "":
        nameAttributes.append(x509.NameAttribute(
            NameOID.DOMAIN_COMPONENT, domainComponent))
    if countryName != None and countryName != "":
        nameAttributes.append(x509.NameAttribute(
            NameOID.COUNTRY_NAME, countryName))
    if stateOrProvinceName != None and stateOrProvinceName != "":
        nameAttributes.append(x509.NameAttribute(
            NameOID.STATE_OR_PROVINCE_NAME, stateOrProvinceName))
    if localityName != None and localityName != "":
        nameAttributes.append(x509.NameAttribute(
            NameOID.LOCALITY_NAME, localityName))
    if organizationName != None and organizationName != "":
        nameAttributes.append(x509.NameAttribute(
            NameOID.ORGANIZATION_NAME, organizationName))

    await cert_gen(applicationUri, x509.Name(nameAttributes), password)


async def cert_gen(applicationUri: str, name: x509.Name, password: bytes = b"passphrase"):
    # Generate our key
    key = rsa.generate_private_key(
        public_exponent=65537,
        key_size=2048,
    )

    # Write our key to disk for safe keeping
    with open(private_key, "wb") as f:
        f.write(key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.TraditionalOpenSSL,
            encryption_algorithm=serialization.BestAvailableEncryption(
                password),
        ))

    # For a self-signed certificate the subject and issuer are always the same.
    subject = issuer = name

    # create serial number
    serial_number = x509.random_serial_number()

    # create cert
    cert = x509.CertificateBuilder().subject_name(
        subject
    ).issuer_name(
        issuer
    ).public_key(
        key.public_key()
    ).serial_number(
        serial_number
    ).not_valid_before(
        datetime.datetime.utcnow()
    ).not_valid_after(
        # Our certificate will be valid for 10 days
        datetime.datetime.utcnow() + datetime.timedelta(days=10)
    ).add_extension(
        # opc ua mandatory
        x509.SubjectAlternativeName([x509.UniformResourceIdentifier(
            applicationUri), x509.DNSName(socket.gethostname())]),
        critical=False
    ).add_extension(
        x509.BasicConstraints(ca=True, path_length=0),
        critical=True
    ).add_extension(
        x509.KeyUsage(digital_signature=True, key_encipherment=True, data_encipherment=True, key_cert_sign=True,
                      content_commitment=True, crl_sign=False, decipher_only=False, encipher_only=False, key_agreement=False),
        critical=True
    ).add_extension(
        x509.ExtendedKeyUsage([
            x509.OID_SERVER_AUTH,
            x509.OID_CLIENT_AUTH
        ]),
        critical=True
    ).add_extension(
        x509.SubjectKeyIdentifier.from_public_key(key.public_key()),
        critical=False
    ).add_extension(
        x509.AuthorityKeyIdentifier(
            key_identifier=hashlib.sha1(key.public_key().public_bytes(
                serialization.Encoding.PEM,
                serialization.PublicFormat.PKCS1,
            )).digest(),

            authority_cert_issuer=[x509.DirectoryName(value=issuer)],
            authority_cert_serial_number=serial_number
        ),
        critical=False
    ).sign(key, hashes.SHA256())  # Sign our certificate with our private key
    # Write our certificate out to disk.
    with open(client_cert, "wb") as f:
        f.write(cert.public_bytes(serialization.Encoding.PEM))


async def main():
    applicationName = "ggolbik.opcua.freeopcua.opcuaasyncio"
    applicationUri = u"urn:debian:FreeOpcUa:Peter"
    await cert_gen2(applicationUri, commonName=applicationName)

    url = 'opc.tcp://192.168.178.15:4840'
    await print_endpoints(url)

    client = Client(url)
    client.name = applicationName
    client.application_uri = applicationUri
    client.set_user('admin')
    client.set_password("31fc29a3")

    # set_security will establish a connection to verify settings
    await client.set_security(
        policy=SecurityPolicyBasic256Sha256,
        certificate=client_cert,
        private_key=private_key,
        private_key_password=u"passphrase",
        mode=ua.MessageSecurityMode.SignAndEncrypt,
    )

    async with client:
        # Client has a few methods to get proxy to UA nodes that should always be in address space such as Root or Objects
        # Node objects have methods to read and write node attributes as well as browse or populate address space
        _logger.info('Children of root are: %r', await client.nodes.root.get_children())

        # get namespaces
        namespaces = await client.get_namespace_array()
        print(namespaces)

        uri = 'http://opcfoundation.org/UA/'
        idx = await client.get_namespace_index(uri)

        await read_value(client)

async def read_value(client: Client) -> None:
    uri = "http://phoenixcontact.com/OpcUA/PLCnext/GlobalDataSpace/"
    idx = await client.get_namespace_index(uri)

    var = client.get_node(ua.NodeId("Arp.Plc.Eclr/DynamicCounter", idx))
    print("My variable", var, await var.read_value())

    # get a specific node knowing its node id
    # var = client.get_node(ua.NodeId(1002, 2))
    # var = client.get_node("ns=3;i=2002")
    # var = await client.nodes.root.get_child(["0:Objects", f"{idx}:MyObject", f"{idx}:MyVariable"])
    # print("My variable", var, await var.read_value())
    # print(var)
    # await var.read_data_value() # get value of node as a DataValue object
    # await var.read_value() # get value of node as a python builtin
    # await var.write_value(ua.Variant([23], ua.VariantType.Int64)) #set node value using explicit data type
    # await var.write_value(3.9) # set node value using implicit data type

if __name__ == '__main__':
    asyncio.run(main())
