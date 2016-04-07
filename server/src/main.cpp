/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "CoAP.h"

#include <list>
#include <map>
#include <string>

int main() {
  auto messaging = CoAP::newMessaging();

  auto name = std::string("coap_server");
  auto dynamic = std::map<int, std::string>();
  auto dynamic_index = 0;
  int counter = 0;
  std::list<std::weak_ptr<CoAP::Notifications>> notificationObservers;

  messaging->requestHandler()
      .onUri("/name")
          .onGet([&name](const Path& path){
            return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload(name);
          })
          .onPut([&name](const Path& path, const std::string& payload){
            return CoAP::RestResponse().withCode(CoAP::Code::Changed);
          })
      .onUri("/dynamic")
          .onPost([&dynamic, &dynamic_index](const Path& path, const std::string& payload){
            dynamic.insert(std::make_pair(++dynamic_index, payload));
            return CoAP::RestResponse().withCode(CoAP::Code::Created).withPayload(std::to_string(dynamic_index));
          })
      .onUri("/dynamic/?")
          .onGet([&dynamic](const Path& path){
            auto payload = std::string("???");
            if (path.partCount() == 2) {
              auto index = stoi(path.part(1));
              auto it = dynamic.find(index);
              if (it != dynamic.end()) return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload(it->second);
            }
            return CoAP::RestResponse().withCode(CoAP::Code::NotFound);
          })
          .onDelete([&dynamic](const Path& path) {
            if (path.partCount() == 2) {
              auto index = stoi(path.part(1));
              auto it = dynamic.find(index);
              if (it != dynamic.end()) {
                dynamic.erase(it);
                return CoAP::RestResponse().withCode(CoAP::Code::Deleted);
              }
            }
            return CoAP::RestResponse().withCode(CoAP::Code::NotFound);
          })
      .onUri("/observable")
           .onObserve([&notificationObservers, &counter](const Path& path, std::weak_ptr<Observable<CoAP::RestResponse>> observer){
             notificationObservers.emplace_back(observer);
             return CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload(std::to_string(++counter));
           });

  for(;;) {
    messaging->loopOnce();
    for(auto& notifications : notificationObservers) {
      auto sp = notifications.lock();
      if (sp) sp->onNext(CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload(std::to_string(counter)));
    }
    ++counter;
  }
}
