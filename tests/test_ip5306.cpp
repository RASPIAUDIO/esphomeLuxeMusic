#include <cassert>
#include "components/luxe_ip5306/luxe_ip5306.h"

using namespace esphome;

int main() {
  luxe_ip5306::LuxeIP5306 chip;
  binary_sensor::BinarySensor available, charging, full, allowed;
  sensor::Sensor level, limit;
  chip.set_available(&available);
  chip.set_charging(&charging);
  chip.set_full(&full);
  chip.set_charge_allowed(&allowed);
  chip.set_level(&level);
  chip.set_current_limit(&limit);

  // Old Luxe without IP5306: unknown states, no write and no permanent failure.
  chip.ack = false;
  chip.setup();
  assert(!available.state && !charging.valid && !full.valid && !allowed.valid);
  assert(std::isnan(level.state) && std::isnan(limit.state));
  assert(!chip.charge_enabled().has_value() && chip.writes == 0);
  chip.set_charge_enabled(false);
  assert(chip.writes == 0);

  // Recovery: read existing settings without overwriting them at startup/poll.
  chip.ack = true;
  chip.regs[0x00] = 0xBD;
  chip.regs[0x70] = 0x08;
  chip.regs[0x71] = 0;
  chip.regs[0x24] = 0xF4;  // Reserved bits excluded; 50 + 20*100 = 2050.
  chip.regs[0x78] = 0xE5;  // Only high nibble is the gauge.
  chip.update();
  assert(available.state && charging.state && !full.state && allowed.state);
  assert(level.state == 25 && limit.state == 2050 && chip.writes == 0);

  chip.regs[0x71] = 8;
  chip.update();
  assert(full.state && !charging.state);  // A full battery is not still charging.

  const uint8_t codes[] = {0x00, 0x80, 0xC0, 0xE0, 0xF0};
  const int levels[] = {100, 75, 50, 25, 0};
  for (int i = 0; i < 5; i++) {
    chip.regs[0x78] = codes[i];
    chip.update();
    assert(level.state == levels[i]);
  }
  chip.regs[0x78] = 0x40;
  chip.update();
  assert(std::isnan(level.state) && available.state);
  chip.fail_read = 0x78;
  chip.update();
  assert(std::isnan(level.state) && available.state);
  chip.fail_read = -1;

  // Only bit 4 changes; repeated identical commands do not write.
  chip.set_charge_enabled(false);
  assert(chip.regs[0x00] == 0xAD && chip.writes == 1);
  assert(chip.charge_enabled().has_value() && !*chip.charge_enabled());
  chip.set_charge_enabled(false);
  assert(chip.writes == 1);
  chip.set_charge_enabled(true);
  assert(chip.regs[0x00] == 0xBD && chip.writes == 2 && *chip.charge_enabled());

  // Write error is not optimistically reported as success.
  chip.fail_write = true;
  chip.set_charge_enabled(false);
  assert(!available.state && !chip.charge_enabled().has_value());
  chip.fail_write = false;
  chip.update();
  assert(available.state && *chip.charge_enabled());

  // ACK alone is insufficient: an essential register read can still fail.
  chip.fail_read = 0x71;
  chip.update();
  assert(!available.state && !charging.valid && !full.valid && !allowed.valid);
  assert(std::isnan(limit.state));
  chip.fail_read = -1;
  chip.update();
  assert(available.state && chip.writes == 2);

  // A write that is acknowledged but ignored must expose the actual state.
  chip.ignore_write = true;
  chip.set_charge_enabled(false);
  assert(available.state && *chip.charge_enabled());
  return 0;
}
