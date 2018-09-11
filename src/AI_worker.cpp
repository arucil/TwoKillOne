#include <emscripten.h>
#include "GameAI.h"
#include "AI_worker.h"


extern "C" void runAI(char *data, int size) {
    auto request = reinterpret_cast<RequestData *>(data);
    GameAI ai;
    ai.think(request->board, request->player, request->milliseconds);

    ResponseData response { ai.getSrcPos(), ai.getDestPos() };
    emscripten_worker_respond(reinterpret_cast<char *>(&response), sizeof response);
}


