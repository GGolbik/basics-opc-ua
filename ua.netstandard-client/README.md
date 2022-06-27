# UA .NETStandard Client

Client based on the [official OPC UA .NET Standard Stack from the OPC Foundation](https://github.com/OPCFoundation/UA-.NETStandard).

# Table of Contents

* [Add Package](#add-package)
* [ConsoleReferenceClient](#consolereferenceclient)
* [Configuration](#configuration)
  * [Application Instance](#application-instance)
  * [Config File](#config-file)
  * [Application Certificate](#application-certificate)
  * [Logging](#logging)
* [Discovery and Security Configuration](#discovery-and-security-configuration)
  * [Security Policies](#security-policies)

# Add Package

~~~
dotnet add package OPCFoundation.NetStandard.Opc.Ua.Client 
~~~

# ConsoleReferenceClient

We took the `UAClient.cs` and `ClientSamples.cs` from the `UA-.NETStandard/Applications/ConsoleReferenceClient` OPC UA .NET Standard and made little modifications as noted in the files.

# Configuration

## Application Instance

- https://documentation.unified-automation.com/uasdknet/2.6.5/html/L2BaseLibAppInstance.html
- https://documentation.unified-automation.com/uasdknet/2.6.5/html/L2BaseLibConfigSchema.html

//ApplicationInstance.MessageDlg => A dialog which asks for user input.

## Config File

You can load a configuration from file or do it programmatically.

## Application Certificate

[Doc](https://github.com/OPCFoundation/UA-.NETStandard/blob/783c20f487d9dd5833c041c324573904d15d94ac/Docs/Certificates.md)

Is it possible to run an OPC UA application without an application certificate?
> No, because the current version of the OPC UA specification requires that all OPC UA applications have a certificate that is exchanged and validated when creating a Session.

This is not correct for `MessageSecurityPolicy.None`:
> If the securityPolicyUri is NONE and none of the UserTokenPolicies requires encryption, the Client shall ignore the ApplicationInstanceCertificate. (see [Part4/5.4.4](https://reference.opcfoundation.org/v104/Core/docs/Part4/5.4.4/)).

There is a requirement that the information provided by the Server in its EndpointDescriptions must match the URI in the `SubjectAltName` of the Certificate e.g. `URL=urn:PXCSW-N0017:UnifiedAutomation:UaExpert`.
It's important to provide a valid application URI.
If the uri is missing or invalid, the connection might be declined.

Similarly, the Client must provide a URI that matches the SubjectAltName of its Certificate when it calls CreateSession.

## Logging

[Docs](https://github.com/OPCFoundation/UA-.NETStandard/blob/master/Docs/Logging.md)

You can set the OPC UA stack logger with `Utils.SetLogger(...);`. See [ConsoleUtils](https://github.com/OPCFoundation/UA-.NETStandard/blob/93e549ad4bebfc1e0feb9b712a35de7c3dbf148a/Applications/ConsoleReferenceServer/ConsoleUtils.cs#L272)


# Discovery and Security Configuration

https://documentation.unified-automation.com/uasdknet/3.1.3/html/L2UaDiscoveryConnect.html

## Security Policies

A server has to provide (at least one) endpoint(s) where clients can connect to. A client uses the Discovery URL (normally identical to the Endpoint URL) to request the list of endpoints including the security configuration from a server. This request through the GetEndpoints service always works without security. The Discovery URL is normally provided by Discovery servers.

~~~C#
ApplicationInstance application =; // ...
Uri serverUrl = new Uri("opc.tcp://192.168.178.15:4840");
UserIdentity? user = new UserIdentity("admin", "31fc29a3");

// Connect to the server's discovery endpoint and find the available configuration.
using (var client = DiscoveryClient.Create(application.ApplicationConfiguration, serverUrl))
{
    var endpoints = client.GetEndpoints(null);
    EndpointDescription? endpoint = null;
    foreach (var e in endpoints)
    {
        if (e.SecurityMode == MessageSecurityMode.None)
        {
            endpoint = e;
            user = null;
            break;
        }
    }
    // ...
    bool connected = await uaClient.ConnectAsync(serverUrl.ToString(), user, endpoint);
}
~~~