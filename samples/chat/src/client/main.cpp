
// ----------------------------------------------------------------------------
// This file is part of the Ducttape Project (http://ducttape-dev.org) and is
// licensed under the GNU LESSER PUBLIC LICENSE version 3. For the full license
// text, please see the LICENSE file in the root of this project or at
// http://www.gnu.org/licenses/lgpl.html
// ----------------------------------------------------------------------------

#include <iostream>
#include <ctime>

#include "Client.hpp"

int main(int argc, char** argv) {
    Client client;

    if(argc > 1)
        client.SetNick(argv[1]);
    else
        client.SetNick("chatter-" + dt::tostr(time(0)));

    if(argc > 2)
        client.SetServerIP(sf::IpAddress(argv[2]));

    client.Run(argc, argv);

    return 0;
}
