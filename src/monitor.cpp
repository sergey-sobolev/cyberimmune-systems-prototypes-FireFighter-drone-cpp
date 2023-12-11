#include "broker.h"
#include "utils.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

/*
 */

using namespace std;

static const string me = "monitor";
Broker* broker;

/*
 * allowed communications list
 * "from", "to", "method"
 */
vector<tuple<string, string, string>> allowed = {
  { "fps", "communication", "start" },                     // 1
  { "communication", "fmac", "start" },                    // 2
  { "fmac", "eaic", "execute" },                             // 3
  { "fmac", "ccu", "execute" },                              // 4
  { "aggregation", "navigation", "get_coordinates" }, // 5, 16
  { "navigation", "aggregation", "coordinates" },     // 6, 17
  { "ccu", "aggregation", "get_coordinates" },                       // 9
  { "aggregation", "ccu", "coordinates" },                       // 10
  { "ccu", "movement", "move" },                          // 11
  { "movement", "ccu", "done" },                          // 13
  { "ccu", "extinguishing", "start_action" },                     // 14 start
  { "extinguishing", "eaic", "confirm_action" },                    // 15 start
  { "eaic", "aggregation", "get_coordinates" },                      // 18
  { "aggregation", "eaic", "coordinates" },                      // 19
  { "eaic", "extinguishing", "start_action" },                    // 21
  { "ccu", "situation", "is_action_running" },                         // 24
  { "situation", "ccu", "action_is_running" },                         // 26
  { "ccu", "extinguishing", "stop_action" },                     // 27 stop
  { "extinguishing", "eaic", "stop_action" },                    // 28 stop
  { "ccu", "communication", "started" },                     // 30 started
  { "communication", "fps", "started" }               // 31 started
};

void
Broker::on_packet(packet pkt)
{
  if (is_allowed(pkt, allowed)) {
    packet out_pkt = { pkt.from, pkt.to,   pkt.method,
                       pkt.val1, pkt.val2, pkt.val3 };
    string message = out_pkt.str();
    broker->publish(nullptr,
                    pkt.to.c_str(),
                    static_cast<int>(message.size()),
                    message.c_str());
  }
}

int
main(int argc, char** argv)
{
  cout << "start: " << me << endl;

  mosqpp::lib_init();
  broker = new Broker(me);
  broker->subscribe(NULL, me.c_str());
  broker->loop_forever();
  mosqpp::lib_cleanup();
  return 0;
}
