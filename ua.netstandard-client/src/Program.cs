using System;
using System.Security.Cryptography.X509Certificates;
using Opc.Ua;
using Opc.Ua.Configuration;

namespace ggolbik.opcua.uadotnetstandardclient;

public class Program
{
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    /// <param name="args"></param>
    public static void Main(string[] args)
    {
        Console.WriteLine("Hello World! {0}", Directory.GetCurrentDirectory());
        Task t = Run();
        t.Wait();
    }

    private async static Task Run()
    {
        string applicationName = "ggolbik.opcua.uadotnetstandardclient";
        string applicationUri = "urn:debian:OPCFoundation:Peter";
        //string configSectionName = "ggolbik.opcua.uadotnetstandardclient";
        string? certificatePassword = null;
        bool autoAccept = true;
        CertificatePasswordProvider certificatePasswordProvider = new CertificatePasswordProvider(certificatePassword);
        Uri serverUrl = new Uri("opc.tcp://192.168.178.15:4840");

        ApplicationInstance application = new ApplicationInstance
        {
            ApplicationName = applicationName,
            ApplicationType = ApplicationType.Client, // https://reference.opcfoundation.org/v104/Core/docs/Part4/7.1/
            //ConfigSectionName = configSectionName, // will be used if config is loaded without given filePath "${ConfigSectionName}.Config.xml"
            CertificatePasswordProvider = certificatePasswordProvider,
        };

        // load the application configuration from file.
        await application.LoadApplicationConfiguration(AppContext.BaseDirectory + "/config.xml", silent: true);
        // update the application configuration
        application.ApplicationConfiguration.SecurityConfiguration = new SecurityConfiguration
        {
            ApplicationCertificate = new CertificateIdentifier { StoreType = @"Directory", StorePath = @"%AppData%\OPC Foundation\CertificateStores\MachineDefault", SubjectName = Utils.Format(@"CN={0}, DC={1}", application.ApplicationConfiguration.ApplicationName, Utils.GetHostName()) },
            TrustedIssuerCertificates = new CertificateTrustList { StoreType = @"Directory", StorePath = @"%AppData%\OPC Foundation\CertificateStores\UA Certificate Authorities" },
            TrustedPeerCertificates = new CertificateTrustList { StoreType = @"Directory", StorePath = @"%AppData%\OPC Foundation\CertificateStores\UA Applications" },
            RejectedCertificateStore = new CertificateTrustList { StoreType = @"Directory", StorePath = @"%AppData%\OPC Foundation\CertificateStores\RejectedCertificates" },
            AutoAcceptUntrustedCertificates = true,
            AddAppCertToTrustedStore = true
        };
        application.ApplicationConfiguration.ApplicationUri = applicationUri;
        application.ApplicationConfiguration.ApplicationName = applicationName;
        var cert = CreateSelfSignedCertificate(application.ApplicationConfiguration);
        application.ApplicationConfiguration.SecurityConfiguration.ApplicationCertificate = new CertificateIdentifier(cert);
        //SaveCert(cert, "/home/ggolbik/git/basics-opc-ua/ua.netstandard-client/build/cert");
        UserIdentity? user = new UserIdentity("admin", "31fc29a3");

        using (var client = DiscoveryClient.Create(application.ApplicationConfiguration, serverUrl))
        {
            var endpoints = client.GetEndpoints(null);
        }

        // create the UA Client object and connect to configured server.
        TextWriter output = Console.Out;
        using (UAClient uaClient = new UAClient(
            application.ApplicationConfiguration, output, ClientBase.ValidateResponse)
        {
            AutoAccept = autoAccept
        })
        {
            // connect
            bool connected = await uaClient.ConnectAsync(serverUrl.ToString(), user, null, true);
            if (connected && uaClient.Session != null)
            {
                Console.Out.WriteLine("Connected!");

                // enable subscription transfer
                uaClient.Session.TransferSubscriptionsOnReconnect = true;

                var namespaces = uaClient.Session.NamespaceUris;
                Console.WriteLine(String.Join(";", namespaces.ToArray().Cast<string>().ToArray()));

                string uri = "http://opcfoundation.org/UA/";
                var idx = namespaces.GetIndex(uri);

                uri = "http://phoenixcontact.com/OpcUA/PLCnext/GlobalDataSpace/";
                idx = namespaces.GetIndex(uri);

                var node = uaClient.Session.ReadNode(new NodeId("Arp.Plc.Eclr/DynamicCounter", (ushort)idx));
                Console.WriteLine(node.ToString());

                var value = uaClient.Session.ReadValue(new NodeId("Arp.Plc.Eclr/DynamicCounter", (ushort)idx));
                Console.WriteLine(value.ToString());

/*
                // Run tests for available methods on reference server.
                var samples = new ClientSamples(output, ClientBase.ValidateResponse);
                samples.ReadNodes(uaClient.Session);
                samples.WriteNodes(uaClient.Session);
                samples.Browse(uaClient.Session);
                samples.CallMethod(uaClient.Session);
                samples.SubscribeToDataChanges(uaClient.Session, 120_000);
*/
                uaClient.Disconnect();
            }
            else
            {
                output.WriteLine("Could not connect to server!");
            }
        }
    }

    /// <summary>
    /// Creates the self signed application certificate
    /// </summary>
    /// <param name="configuration"></param>
    /// <returns></returns>
    private static X509Certificate2? CreateSelfSignedCertificate(ApplicationConfiguration configuration)
    {
        string commonName = configuration.ApplicationName;
        string domainComponent = Utils.GetHostName();
        string subjectName = Utils.Format("CN={0}, DC={1}", commonName, domainComponent);
        return CertificateFactory.CreateCertificate(configuration.ApplicationUri, configuration.ApplicationName, subjectName, new string[0]).CreateForRSA();
    }

    /// <summary>
    /// Saves the certificate to a file.
    /// </summary>
    /// <param name="certificate"></param>
    /// <param name="filename">The filename without extension</param>
    private static void SaveCert(X509Certificate2 certificate, string filename)
    {
        // Create PFX (PKCS #12) with private key
        File.WriteAllBytes(filename + ".pfx", certificate.Export(X509ContentType.Pfx, "P@55w0rd"));

        // Create Base 64 encoded CER (public key only)
        File.WriteAllText(filename + ".cer",
            "-----BEGIN CERTIFICATE-----\r\n"
            + Convert.ToBase64String(certificate.Export(X509ContentType.Cert), Base64FormattingOptions.InsertLineBreaks)
            + "\r\n-----END CERTIFICATE-----");
    }

}
