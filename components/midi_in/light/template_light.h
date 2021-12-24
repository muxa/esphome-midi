#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/addressable_light.h"

namespace esphome {
namespace midi_in {

class TemplateLightOutput : public light::AddressableLight {
 public:
  explicit TemplateLightOutput(int num_leds) {
    num_leds_ = num_leds;
    this->leds_ = new Color[num_leds];  // NOLINT
    for (int i = 0; i < num_leds; i++)
      this->leds_[i] = Color::BLACK;
    this->effect_data_ = new uint8_t[num_leds];  // NOLINT
  }

  inline int32_t size() const override { return this->num_leds_; }

  light::LightTraits get_traits() override {
    light::LightTraits traits;
    traits.set_supported_color_modes({light::ColorMode::RGB});
    return traits;
  }

  void setup() override;
  void dump_config() override;
  void write_state(light::LightState *state) override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  void clear_effect_data() override {
    for (int i = 0; i < this->size(); i++)
      this->effect_data_[i] = 0;
  }

 protected:
  light::ESPColorView get_view_internal(int32_t index) const override {
    return {&this->leds_[index].r,      &this->leds_[index].g, &this->leds_[index].b, nullptr,
            &this->effect_data_[index], &this->correction_};
  }
  Color *leds_{nullptr};
  uint8_t *effect_data_{nullptr};
  int num_leds_{0};
};

}  // namespace midi_in
}  // namespace esphome