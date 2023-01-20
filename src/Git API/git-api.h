#pragma once

struct UserCreds {
    String username;
    String token;
};

void fetchData(char* username, char* token);
void runServer();
void handleConnections();