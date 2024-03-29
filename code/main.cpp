#include "mtsGalilController/mtsGalilController.h"

#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

int main(int argc, char* argv[])
{
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);

    cmnLogger::SetMaskClassMatching("osaTimeServer", CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    cmnLogger::SetMaskClass("cmnThrow", CMN_LOG_ALLOW_ALL);

    mtsTaskManager* componentManager = mtsTaskManager::GetInstance();

    mtsGalilController* galil_task = new mtsGalilController("GalilTask", 500.0 * cmn_ms);
    cmnLogger::SetMaskClass("mtsGalilController", CMN_LOG_ALLOW_ALL);

    // Add option parsing
    cmnCommandLineOptions options;
    std::string jsonGalilConfigFile;

    options.AddOptionOneValue(
        "j", "json-config",
        "json configuration file for galil controller",
        cmnCommandLineOptions::REQUIRED_OPTION, 
        &jsonGalilConfigFile
    );

    if (!options.Parse(argc, argv, std::cerr))
        return -1;

    std::cout << "Using config file: \"" << jsonGalilConfigFile << "\"\n";

    // Configure the task(s)
    galil_task->Configure(jsonGalilConfigFile);

    // Add components
    componentManager->AddComponent(galil_task);

    // Create components
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY, 2.0 * cmn_s);

    // Start componenets
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE, 2.0 * cmn_s);

    osaSleep(5.0*cmn_s);

    // Kill Components
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 10.0 * cmn_s);

    componentManager->Cleanup();

    delete galil_task;

    cmnLogger::Kill();

    return 0;

}