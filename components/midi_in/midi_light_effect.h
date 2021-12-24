#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/addressable_light_effect.h"
#include "midi_in.h"

namespace esphome {
namespace midi_in {

class MidiLightEffect : public light::AddressableLightEffect {
 public:
  MidiLightEffect(midi_in::MidiInComponent *midi, const std::string &name);

 public:
  void start() override;
  void stop() override;
  void apply(light::AddressableLight &it, const Color &current_color) override;

 public:
  void set_start_note(uint8_t start_note) { this->start_note_ = start_note; }
  void set_keys(uint8_t keys) { this->keys_ = keys; }
  void set_note_on_fade(uint32_t note_on_fade) { this->note_on_fade_ = note_on_fade; }
  void set_note_off_fade(uint32_t note_off_fade) { this->note_off_fade_ = note_off_fade; }
  void set_foreground_light(light::AddressableLightState *foreground_light) { this->foreground_light_ = foreground_light; }
  void set_background_light(light::AddressableLightState *background_light) { this->background_light_ = background_light; }

 protected:
  midi_in::MidiInComponent *midi_;

 protected:
  uint8_t start_note_;
  uint8_t keys_;
  uint32_t note_on_fade_;
  uint32_t note_off_fade_;
  float bleed_{0.0};
  light::AddressableLightState *foreground_light_;
  light::AddressableLightState *background_light_;

 protected:
    // This looks crazy, but it reduces to 6x^5 - 15x^4 + 10x^3 which is just a smooth sigmoid-like
    // transition from 0 to 1 on x = [0, 1]
    static float smoothed_progress(float x) { return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f); }

    float get_interpolated_opacity_(const int i, const uint32_t now)
    {
        float p = this->get_progress_(this->led_transitions_[i].start_time, this->led_transitions_[i].length, now);
        // float v = MidiLightEffect::smoothed_progress(p);
        return esphome::lerp(p, this->led_transitions_[i].fg_opacity_start, this->led_transitions_[i].fg_opacity_target);
    }

    Color get_interpolated_color_(const int i, const uint32_t now, const Color &current_color)
    {
        float opacity = this->get_interpolated_opacity_(i, now);

        Color fg_color;
        if (this->fg_light_ != nullptr) {
            fg_color = this->fg_light_->get(i).get();
        } else {
            fg_color = current_color;
        }
        Color bg_color;
        if (this->bg_light_ != nullptr) {
            bg_color = this->bg_light_->get(i).get();
        } else {
            bg_color = Color::BLACK;
        }

        return this->interpolate_color_(bg_color, fg_color, opacity);
    }

    /// The progress of this transition, on a scale of 0 to 1.
    float get_progress_(uint32_t start_time, uint32_t length, uint32_t now) {
        if (now < start_time)
            return 0.0f;
        if (now >= start_time + length)
            return 1.0f;

        return clamp((now - start_time) / float(length), 0.0f, 1.0f);
    }

    Color interpolate_color_(Color start_color, Color target_color, float completion) {
        if (completion <= 0.0)
            return start_color;
        if (completion >= 1.0)
            return target_color;
        float r = esphome::lerp(completion, float(start_color.r), float(target_color.r));
        float g = esphome::lerp(completion, float(start_color.g), float(target_color.g));
        float b = esphome::lerp(completion, float(start_color.b), float(target_color.b));
        return Color(static_cast<uint8_t>(roundf(r)), static_cast<uint8_t>(roundf(g)), static_cast<uint8_t>(roundf(b)));
    }

 protected:
  enum NoteStatus : uint8_t {
      OFF         = 0x00,
      PRESSED     = 0x01,
      SUSTAINED   = 0x02,
  };

  struct ColorTransition
  {
      float fg_opacity_start;
      float fg_opacity_target;

      uint32_t start_time;
      uint32_t length;
  };

 protected:

  std::vector<ColorTransition> led_transitions_;

  NoteStatus note_statuses_[128];
  uint32_t note_on_time_[128];

 private:
  light::AddressableLight *fg_light_{nullptr};
  light::AddressableLight *bg_light_{nullptr};
};

}  // namespace midi_in
}  // namespace esphome