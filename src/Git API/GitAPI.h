#pragma once

struct UserCreds {
    String username;
    String token;
};

void fetchData();
void runServer();
void handleConnections();