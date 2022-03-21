#include "StreamController.hpp"
#include "CommandKey.hpp"
#include "CommandKey.cpp"


StreamController::StreamController(VideoStream* stream) {
    this->mainstream = stream;
    this->commands = std::map<CommandKey*, ICommand*>();

    // init basic commands
    InitBasicCommands();

    // create pthread to handle commands
    pthread_create(&(this->p), NULL, &StreamController::CommandsHandler, (void*) this);
}

StreamController::~StreamController() {}

void StreamController::CommandsList() {
    g_print("\n===========================\nCommands list:\nCommand\t\tDescription\n---------------------------\n");
    // Iterate over the map using Iterator till end.
    for (auto it = (this->commands).begin(); it != (this->commands).end(); it++) {
        g_print(it->first->getKey());
        g_print("\t\t");
        g_print(it->second->getDescription());
        g_print("\n");
    }
    g_print("\t\t(Press ctrl+c for exit)");
    g_print("\n===========================");
}

void StreamController::InitBasicCommands() {
    // p : Connect WebCam
    this->commands.emplace(new CommandKey("c"), new CommandConnectCam(this->mainstream));
    // p : Start playing
    this->commands.emplace(new CommandKey("p"), new CommandStart(this->mainstream));
    // s : Stop playing
    this->commands.emplace(new CommandKey("s"), new CommandStop(this->mainstream));
    // a : Pause playing
    this->commands.emplace(new CommandKey("a"), new CommandPause(this->mainstream));
    // r : Start record
    this->commands.emplace(new CommandKey("r"), new CommandRecord(this->mainstream));
    // d : Start display
    this->commands.emplace(new CommandKey("d"), new CommandDisplay(this->mainstream));
    // t : Status
    this->commands.emplace(new CommandKey("t"), new CommandStatus(this->mainstream));
}

void* StreamController::CommandsHandler(void *ptr) {
    gchar c;
    StreamController *current = (StreamController *)ptr;
    current->CommandsList();
    while (TRUE) {
    std::cout << "\n\t\t\t\tEnter command: ";
    std::cin >> c;
    CommandKey* key = new CommandKey(g_strndup(&c, 1));
    fflush(stdin);
    bool key_found = FALSE;
    for (auto it = (current->commands).begin(); it != (current->commands).end(); it++) {
        if (g_strcmp0(key->getKey(), it->first->getKey()) == 0) {
            it->second->execute();
            key_found = TRUE;
        }
    }
    if (!key_found) {
      g_print("Unidentified command.\n");
    }
  }
}