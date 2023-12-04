/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef SRC_SUPLA_DEVICE_SW_UPDATE_H_
#define SRC_SUPLA_DEVICE_SW_UPDATE_H_

#define SUPLA_MAX_URL_LENGTH 202

#include <SuplaDevice.h>

namespace Supla {
namespace Device {
class SwUpdate {
 public:
  static SwUpdate *Create(SuplaDeviceClass *sdc, const char *newUrl);
  virtual ~SwUpdate();

  virtual void start();
  virtual void iterate() = 0;

  void setUrl(const char *newUrl);
  virtual bool isStarted();
  virtual bool isFinished() = 0;
  virtual bool isAborted() = 0;
  void useBeta();

 protected:
  explicit SwUpdate(SuplaDeviceClass *sdc, const char *newUrl = nullptr);

  bool beta = false;
  bool started = false;
  SuplaDeviceClass *sdc = nullptr;

  char url[SUPLA_MAX_URL_LENGTH] = {};
};
};  // namespace Device
};  // namespace Supla

#endif  // SRC_SUPLA_DEVICE_SW_UPDATE_H_
