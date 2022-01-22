#include "OpcUaServer.h"

#include "logging/UaLogger.h"
#include "nodesets/di.h"
#include "nodesets/example_nodeset.h"

#define UA_NSDI_NAMESPACE ("http://opcfoundation.org/UA/DI/")
#define UA_NSDI_DEVICETYPE (1002)
#define UA_NSDI_DEVICESET (5001)

#define UA_NSMY_NAMESPACE ("http://ggolbik.de/example_nodeset/")

namespace ggolbik {
namespace opcua {

const std::string OpcUaServer::LogTag = "OpcUaServer";

OpcUaServer::~OpcUaServer() { this->stop(); }

bool OpcUaServer::configure(std::shared_ptr<ILogger> logger) {
  if (this->running) {
    return false;
  }
  this->loggerPtr = logger;
  if (logger) {
    const std::string sdkLogTag = "OPC UA SDK";
    this->uaLoggerPtr = std::make_unique<UaLogger>(sdkLogTag, logger);
  } else {
    this->uaLoggerPtr.reset();
  }
  return true;
}

bool OpcUaServer::createServer() {
  // init config
  UA_ServerConfig config = {};

  // Creates config on the default port 4840 with no server
  // certificate.
  UA_StatusCode status = UA_ServerConfig_setDefault(&config);
  if (status != UA_STATUSCODE_GOOD) {
    return false;
  }

  // set logger
  if (this->uaLoggerPtr) {
    config.logger = this->uaLoggerPtr->toUaLogger();
  }

  // Create your own UA_AccessControl instance to implement a custom security
  // behavior. See https://open62541.org/doc/current/plugin_accesscontrol.html
  // config.accessControl = UA_AccessControl();

  // create a server
  this->uaServerPtr = UaServerPtr(UA_Server_newWithConfig(&config));

  return static_cast<bool>(this->uaServerPtr);
}

bool OpcUaServer::start() {
  if (!this->uaLoggerPtr) {
    return false;
  }

  if (this->loggerPtr)
    this->loggerPtr->log(OpcUaServer::LogTag, Level::info, "",
                         "Creating UA_Server.");

  if (!this->createServer()) {
    return false;
  }

  if (this->loggerPtr)
    this->loggerPtr->log(OpcUaServer::LogTag, Level::info, "",
                         "Adding DI namespace.");

  bool diAvailable = this->addNamespaceDi();

  if (this->loggerPtr)
    this->loggerPtr->log(OpcUaServer::LogTag, Level::info, "",
                         "Adding MY namespace.");

  bool myAvailable = this->addNamespaceMy();

  if (this->loggerPtr)
    this->loggerPtr->log(OpcUaServer::LogTag, Level::info, "",
                         "Adding default instance.");

  if (!this->addDefaultInstance()) {
    if (this->loggerPtr)
      this->loggerPtr->log(OpcUaServer::LogTag, Level::error, "",
                           "Failed to add default instance.");
  }

  if (diAvailable) {
    if (this->loggerPtr)
      this->loggerPtr->log(OpcUaServer::LogTag, Level::info, "",
                           "Adding DI instance.");
    if (!this->addDiInstance()) {
      if (this->loggerPtr)
        this->loggerPtr->log(OpcUaServer::LogTag, Level::error, "",
                             "Failed to add DI instance.");
    }
  }

  if (myAvailable) {
    if (this->loggerPtr)
      this->loggerPtr->log(OpcUaServer::LogTag, Level::info, "",
                           "Adding MY instance.");
    if (!this->addMyNsInstance()) {
      if (this->loggerPtr)
        this->loggerPtr->log(OpcUaServer::LogTag, Level::error, "",
                             "Failed to add MY instance.");
    }
  }

  this->running = true;
  if (this->loggerPtr)
    this->loggerPtr->log(OpcUaServer::LogTag, Level::info, "",
                         "Start UA_Server.");

  UA_StatusCode status = UA_Server_run(this->uaServerPtr.get(), &this->running);
  if (status != UA_STATUSCODE_GOOD) {
    this->stop();
    return false;
  } else {
    return true;
  }
}
void OpcUaServer::stop() {
  if (running && this->loggerPtr)
    this->loggerPtr->log(OpcUaServer::LogTag, Level::info, "",
                         "Stop UA_Server.");
  this->running = false;
  this->uaServerPtr.reset();
}

bool OpcUaServer::addNamespaceDi() {
  if (!this->uaServerPtr) {
    return false;
  }
  // add di nodeset
  UA_StatusCode status = ::di(this->uaServerPtr.get());
  return status == UA_STATUSCODE_GOOD;
}

bool OpcUaServer::addNamespaceMy() {
  if (!this->uaServerPtr) {
    return false;
  }
  // add my nodeset
  UA_StatusCode status = ::example_nodeset(this->uaServerPtr.get());
  return status == UA_STATUSCODE_GOOD;
}

bool OpcUaServer::addDefaultInstance() {
  if (!this->uaServerPtr) {
    return false;
  }

  /* Add a variable node */
  /* 1) Define the node attributes */
  UA_VariableAttributes attr = UA_VariableAttributes_default;
  std::string displayNameLocale = "en-US";
  std::string displayNameText = "the answer";
  attr.displayName =
      UA_LOCALIZEDTEXT(displayNameLocale.data(), displayNameText.data());
  UA_Int32 myInteger = 42;
  UA_Variant_setScalar(&attr.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);

  /* 2) Define where the node shall be added with which browsename */
  std::string newNodeIdText = "the.answer";
  UA_NodeId newNodeId = UA_NODEID_STRING(1, newNodeIdText.data());
  UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
  UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
  UA_NodeId variableType = UA_NODEID_NULL; /* take the default variable type */
  std::string browseNameText = "the answer";
  UA_QualifiedName browseName = UA_QUALIFIEDNAME(1, browseNameText.data());

  /* 3) Add the node */
  UA_StatusCode status = UA_Server_addVariableNode(
      this->uaServerPtr.get(), newNodeId, parentNodeId, parentReferenceNodeId,
      browseName, variableType, attr, NULL, NULL);
  return status == UA_STATUSCODE_GOOD;
}

// https://open62541.org/doc/current/tutorial_server_object.html
bool OpcUaServer::addDiInstance() {
  if (!this->uaServerPtr) {
    return false;
  }
  UA_UInt16 nsIndex =
      UA_Server_addNamespace(this->uaServerPtr.get(), UA_NSDI_NAMESPACE);

  // Define the object type for "Device"
  UA_NodeId deviceTypeId;
  UA_ObjectTypeAttributes dtAttr = UA_ObjectTypeAttributes_default;
  std::string typeDisplayNameLocale = "en-US";
  std::string typeDisplayNameText = "RaspberryPi";
  std::string typeBrowseNameText = "RaspberryPi";
  dtAttr.displayName = UA_LOCALIZEDTEXT(typeDisplayNameLocale.data(),
                                        typeDisplayNameText.data());
  UA_StatusCode status = UA_Server_addObjectTypeNode(
      this->uaServerPtr.get(),                         //
      UA_NODEID_NULL,                                  //
      UA_NODEID_NUMERIC(nsIndex, UA_NSDI_DEVICETYPE),  //
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),       //
      UA_QUALIFIEDNAME(1, typeBrowseNameText.data()),  //
      dtAttr,                                          //
      NULL,                                            //
      &deviceTypeId                                    //
  );
  if (status != UA_STATUSCODE_GOOD) {
    this->loggerPtr->log(OpcUaServer::LogTag, Level::error, "",
                         "Failed to add object type node: %s",
                         UA_StatusCode_name(status));
    return false;
  }

  // Define the object "Device"
  UA_NodeId createdNodeId;
  UA_ObjectAttributes object_attr = UA_ObjectAttributes_default;

  std::string descriptionLocale = "en-US";
  std::string descriptionText = "This device";
  object_attr.description =
      UA_LOCALIZEDTEXT(descriptionLocale.data(), descriptionText.data());
  std::string displayNameLocale = "en-US";
  std::string displayNameText = "This device";
  object_attr.displayName =
      UA_LOCALIZEDTEXT(displayNameLocale.data(), displayNameText.data());

  UA_NodeId newNodeId = UA_NODEID_NUMERIC(1, 0);
  UA_NodeId parentNodeId = UA_NODEID_NUMERIC(nsIndex, UA_NSDI_DEVICESET);
  UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT);
  std::string browseNameText = "ThisDevice";
  UA_QualifiedName browseName = UA_QUALIFIEDNAME(1, browseNameText.data());

  status = UA_Server_addObjectNode(this->uaServerPtr.get(),  //
                                   newNodeId,                //
                                   parentNodeId,             //
                                   parentReferenceNodeId,    //
                                   browseName,               //
                                   deviceTypeId,             // typeDefinition
                                   object_attr,              // attr
                                   NULL,                     //
                                   &createdNodeId            //
  );

  if (status != UA_STATUSCODE_GOOD) {
    this->loggerPtr->log(OpcUaServer::LogTag, Level::error, "",
                         "Failed to add object node: %s",
                         UA_StatusCode_name(status));
    return false;
  }
  // device is created with the mandatory variables which contains default
  // values.

  if (!this->setManufacturer(createdNodeId)) {
    return false;
  }

  // UA_NodeId variableNodeId;
  // UA_ValueCallback callback;
  // UA_Server_setVariableNode_valueCallback(this->uaServerPtr.get(),
  // variableNodeId, callback);

  return status == UA_STATUSCODE_GOOD;
}

bool OpcUaServer::setManufacturer(UA_NodeId& nodeId) {
  /* Find the NodeId of the status child variable */
  UA_RelativePathElement rpe;
  UA_RelativePathElement_init(&rpe);
  rpe.referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
  rpe.isInverse = false;
  rpe.includeSubtypes = false;
  std::string varName = "Manufacturer";
  UA_UInt16 nsIndex =
      UA_Server_addNamespace(this->uaServerPtr.get(), UA_NSDI_NAMESPACE);
  rpe.targetName = UA_QUALIFIEDNAME(nsIndex, varName.data());

  UA_BrowsePath bp;
  UA_BrowsePath_init(&bp);
  bp.startingNode = nodeId;
  bp.relativePath.elementsSize = 1;
  bp.relativePath.elements = &rpe;

  UA_BrowsePathResult bpr =
      UA_Server_translateBrowsePathToNodeIds(this->uaServerPtr.get(), &bp);
  if (bpr.statusCode != UA_STATUSCODE_GOOD || bpr.targetsSize < 1) {
    this->loggerPtr->log(OpcUaServer::LogTag, Level::error, "",
                         "Failed to find Manufacturer node: %s",
                         UA_StatusCode_name(bpr.statusCode));
    UA_BrowsePathResult_clear(&bpr);
    return false;
  }

  /* Set the status value */
  UA_LocalizedText manufacturerName;
  manufacturerName.locale = UA_String_fromChars("en-US");
  manufacturerName.text = UA_String_fromChars("GGolbik");
  UA_Variant manufacturerNameValue;
  UA_Variant_init(&manufacturerNameValue);
  UA_Variant_setScalar(&manufacturerNameValue, &manufacturerName,
                       &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);

  UA_StatusCode status = UA_Server_writeValue(this->uaServerPtr.get(),
                                              bpr.targets[0].targetId.nodeId,
                                              manufacturerNameValue);
  UA_BrowsePathResult_clear(&bpr);
  if (status != UA_STATUSCODE_GOOD) {
    this->loggerPtr->log(OpcUaServer::LogTag, Level::error, "",
                         "Failed to write Manufacturer: %s",
                         UA_StatusCode_name(status));
    return false;
  }
  return true;
}

std::string OpcUaServer::toString(UA_String& str) {
  return std::string(reinterpret_cast<char const*>(str.data), str.length);
}

bool OpcUaServer::addMyNsInstance() {
  if (!this->uaServerPtr) {
    return false;
  }
  UA_NodeId createdNodeId;
  UA_ObjectAttributes object_attr = UA_ObjectAttributes_default;

  std::string descriptionLocale = "en-US";
  std::string descriptionText = "A pump!";
  object_attr.description =
      UA_LOCALIZEDTEXT(descriptionLocale.data(), descriptionText.data());
  std::string displayNameLocale = "en-US";
  std::string displayNameText = "Pump1";
  std::string qualifiedNameText = "Pump1";
  object_attr.displayName =
      UA_LOCALIZEDTEXT(displayNameLocale.data(), displayNameText.data());

  UA_UInt16 nsIndex =
      UA_Server_addNamespace(this->uaServerPtr.get(), UA_NSMY_NAMESPACE);
  UA_StatusCode status = UA_Server_addObjectNode(
      this->uaServerPtr.get(),                        // server
      UA_NODEID_NUMERIC(1, 0),                        // requestedNodeId
      UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),   // parentNodeId
      UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),       // referenceTypeId
      UA_QUALIFIEDNAME(1, qualifiedNameText.data()),  // browseName
      UA_NODEID_NUMERIC(nsIndex, 1002),               // typeDefinition
      object_attr,                                    // attr
      NULL,                                           // nodeContext
      &createdNodeId                                  // outNewNodeId
  );
  return status == UA_STATUSCODE_GOOD;
}

}  // namespace opcua
}  // namespace ggolbik