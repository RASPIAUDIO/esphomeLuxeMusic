"""Compile the actual IP5306 component against an in-memory I2C bus."""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
STUB = r"""
#pragma once
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <optional>
#define ESP_LOGW(...) ((void) 0)
namespace esphome {
template<class T> using optional = std::optional<T>;
namespace setup_priority { constexpr float DATA = 600; }
class PollingComponent {
 public:
  virtual void setup() {}
  virtual void update() {}
  virtual float get_setup_priority() const { return 0; }
};
namespace binary_sensor {
class BinarySensor {
 public:
  bool state = false, valid = false;
  void publish_state(bool value) { state = value; valid = true; }
  void invalidate_state() { valid = false; }
};
}
namespace sensor {
class Sensor {
 public:
  float state = NAN;
  void publish_state(float value) { state = value; }
};
}
namespace i2c {
enum ErrorCode { ERROR_OK, ERROR_UNKNOWN };
class I2CDevice {
 public:
  bool ack = true, fail_write = false, ignore_write = false;
  int fail_read = -1, writes = 0;
  uint8_t regs[256]{};
  ErrorCode write(const uint8_t *, size_t size) {
    // Detection must send an address only, not any configuration bytes.
    return ack && size == 0 ? ERROR_OK : ERROR_UNKNOWN;
  }
  bool read_byte(uint8_t reg, uint8_t *out) {
    if (!ack || reg == fail_read) return false;
    *out = regs[reg]; return true;
  }
  bool write_byte(uint8_t reg, uint8_t value) {
    if (!ack || fail_write) return false;
    writes++;
    if (!ignore_write) regs[reg] = value;
    return true;
  }
};
}
}
"""

with tempfile.TemporaryDirectory(prefix="luxe-ip5306-test-") as folder:
    temp = Path(folder)
    (temp / "stub.h").write_text(STUB)
    for relative in (
        "core/component.h", "core/helpers.h", "core/log.h",
        "components/i2c/i2c.h", "components/binary_sensor/binary_sensor.h",
        "components/sensor/sensor.h",
    ):
        header = temp / "esphome" / relative
        header.parent.mkdir(parents=True, exist_ok=True)
        header.write_text('#include "stub.h"\n')
    binary = temp / "test_ip5306"
    subprocess.run(
        ["g++", "-std=c++17", "-Wall", "-Wextra", "-Werror",
         "-I", str(temp), "-I", str(ROOT),
         str(ROOT / "tests/test_ip5306.cpp"), "-o", str(binary)],
        check=True,
    )
    subprocess.run([str(binary)], check=True)
print("IP5306: absence, recovery, decoding, reserved bits and write failures OK")
