This program manages the registry entries that dictate where games will look for the freetrackclient and NPCLient dlls.
Usage: FTnoirRegistryTool64.exe cmd1 cmd2 ... cmdN  where cmd is one of -h, -v, -p, -x, -e ft|np|all, -d ft|np|all, -c or -l <path>
  -h                            Print help.
  -v                            Enable verbose output.
  -p                            Print current settings.
  -x                            Exclusive mode: When enabling a specific protocl then the other protocol
                                will be disabled automatically.
  -e np | ft | all              Enable npclient, freetrack or all protocols.
  -d np | ft | all              Disable npclient, freetrack or all protocols.
  -c                            With this option the current directory will be used.
  -l C:\custom\loc\             Set a custom location. Pass before any occurence of -e to be effective.
                                Do not use forward slashes although the registry keys use them.

Notes:
Without arguments the program enters an ineractive mode.

The commands are executed in the order of appearance on the commandline. Therefore you can combine -c or -d with one -e to enable one protocol exclusively.

By default the location for the client dlls is assumed to be this program's location. Use -l and -c to modify this behavior

The program assumes the provided dlls are located in a directory 'clientlibs' relative to this programs location. You can define a custom location via -l C:\o
ther\location\

Examples:
Enable npclient and freetrack:
        FTnoirRegistryTool64.exe -e all
Disabling npclient without touching anything else:
        FTnoirRegistryTool64.exe -d np
Prints the current state then enables freetrack without touching anything else and prints the new state:
        FTnoirRegistryTool64.exe -p -e ft -p
Enabling npclient exclusively:
        FTnoirRegistryTool64.exe -x -e np
Using a custom directory for NPClient:
        FTnoirRegistryTool64.exe -l C:\customdir\ -e np enables npclient exclusively:
Using a different custom directory for NPClient and FreeTrackClient:
        FTnoirRegistryTool64.exe -l C:\nppath\ -e np -l C:\ftpath\ -e ft
