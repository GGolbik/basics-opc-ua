
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <signal.h>

#include <memory> // std::shared_ptr; std::make_shared
#include <string> // std::string

#include "ggolbik/opcua/OpcUaServer.h"
#include "ggolbik/opcua/logging/ConsoleLogger.h"
#include "ggolbik/opcua/logging/RootLogger.h"

using namespace ggolbik::opcua;
using namespace ggolbik::opcua::logging;

const std::string MainLogTag = "MAIN";
const std::string MainCategory = "";
static OpcUaServer server;

void signalHandler(int sig) { server.stop(); }

int main(int argc, char **argv) {
  ::signal(SIGINT, signalHandler); /* catch ctrl-c */

  std::shared_ptr<RootLogger> rootLogger =
      std::make_shared<RootLogger>(Level::info);
  rootLogger->addLogger("ConsoleLogger", std::make_shared<ConsoleLogger>());

  rootLogger->log(MainLogTag, Level::info, MainCategory,
                  "Server will be configured.");

  server.configure(rootLogger);

  rootLogger->log(MainLogTag, Level::info, MainCategory,
                  "Server will be started.");

  if (!server.start()) {
    rootLogger->log(MainLogTag, Level::error, MainCategory,
                    "Failed to start the server.");
    return 1;
  } else {
    rootLogger->log(MainLogTag, Level::info, MainCategory,
                    "Server has been stopped.");
    return 0;
  }
}
