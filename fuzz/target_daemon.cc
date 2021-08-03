#include "daemon.hh"
#include "serialise.hh"
#include "store-api.hh"
#include "worker-protocol.hh"

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <thread>

// TODO: for debugging, to remove
#include <errno.h>

// TODO: don't recreate / rebind / relisten / reconnect with sockets each time (?)

// TODO: custom mutator to
extern "C" int LLVMFuzzerCustomMutator(..., max size) {

}


static const char * DAEMON_SOCKET_FILE = "/home/pamplemousse/fuzzable-daemon";

typedef struct ClientData {
    sockaddr_un * daemon;
    const uint8_t * data;
    size_t size;
} ClientData;


// TODO: checkout src/libstore/remote-store.cc:RemoteStore::initConnection for help
void * commmunicateWithDaemon(void *arg) {
    using namespace nix;

    // Extract the arguments.
    ClientData * clientData = (ClientData *) arg;
    sockaddr_un * daemon = clientData->daemon;

    // Create the socket for the client to communicate with the daemon.
    AutoCloseFD clientSocket = socket(PF_UNIX, SOCK_STREAM, 0);
    if (!clientSocket)
        throw Error("harness: client socket: failed to create");
    if (connect(clientSocket.get(), (struct sockaddr *) daemon, sizeof(*daemon)) == -1)
        throw Error("harness: client socket: failed to connect to daemon");

    FdSource from(clientSocket.get());
    FdSink to(clientSocket.get());

    // TODO: send magic bytes
    // 0x6e697863
    // Sending magic byte.
    to << WORKER_MAGIC_1;
    to.flush();

    unsigned int magic = readInt(from);
    // if (magic != WORKER_MAGIC_2) throw Error("protocol mismatch");


    from >> conn.daemonVersion;
    to << PROTOCOL_VERSION;
    // if (cpu != -1)
    //     to << 1 << cpu;
    // else
    to << 0;

    // if (GET_PROTOCOL_MINOR(conn.daemonVersion) >= 11)
    //     conn.to << false;
    //
    // auto ex = conn.processStderr();
    // if (ex) std::rethrow_exception(ex);

    pthread_exit(NULL);
}

// See `src/libstore/daemon.cc` for inspiration.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

    if (Size < 10)
        return 0

    using namespace nix;
    using namespace daemon;

    // TODO: why?
    // if (Size < 10) return 0;

    // Create the socket with which the daemon receives its content.
    AutoCloseFD daemonSocket = socket(PF_UNIX, SOCK_STREAM, 0);
    if (!daemonSocket)
        throw SysError("harness: daemon socket: failed to create");

    struct sockaddr_un daemon;
    daemon.sun_family = AF_UNIX;
    strcpy(daemon.sun_path, DAEMON_SOCKET_FILE);

    if (bind(daemonSocket.get(), (struct sockaddr *) &daemon, sizeof(daemon)) == -1)
        throw SysError("harness daemon socket: cannot bind");
    if (chmod(DAEMON_SOCKET_FILE, 0666) == -1)
        throw SysError("harness: daemon socket: failed changing permissions on file");
    if (listen(daemonSocket.get(), 5) == -1)
        throw SysError("harness: daemon socket: cannot listen");

    // Simulate a client communicating with the daemon in a separate thread.
    pthread_t clientID;
    ClientData threadArgument = { &daemon, Data, Size };
    int client = pthread_create(&clientID, NULL, commmunicateWithDaemon, (void *) &threadArgument);
    if (client)
       throw SysError("harness: failed to create thread");

    // Accept connections to the daemon.
    AutoCloseFD listeningDaemon = accept(daemonSocket.get(), 0, 0);
    if (!listeningDaemon)
        throw SysError("harness: daemon socket: failed accepting connection");

    // Handle the connection.
    FdSource from(listeningDaemon.get());
    FdSink to(listeningDaemon.get());

    try {
        processConnection(openStore("dummy://"), from, to, Trusted, NotRecursive, [&](Store & _){});
    // Some errors are legitimate, so we want to gracefully return when they are raised.
    } catch(const Error &) {
        return 0;
    }

    // TODO: cleanup!
    unlink(DAEMON_SOCKET_FILE);

    return 0;
}
