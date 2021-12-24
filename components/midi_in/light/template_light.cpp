#include "template_light.h"
#include "esphome/core/log.h"

namespace esphome {
namespace midi_in {

static const char *const TAG = "midi_in";

void TemplateLightOutput::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Template light...");  
}
void TemplateLightOutput::dump_config() {
  ESP_LOGCONFIG(TAG, "Template light:");
  ESP_LOGCONFIG(TAG, "  Num LEDs: %u", this->num_leds_);
  //ESP_LOGCONFIG(TAG, "  Max refresh rate: %u", *this->max_refresh_rate_);
}
void TemplateLightOutput::write_state(light::LightState *state) {
  this->mark_shown_();
}

}  // namespace midi_in
}  // namespace esphome