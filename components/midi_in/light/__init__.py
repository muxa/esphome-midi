import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import (
    CONF_NUM_LEDS,
    CONF_OUTPUT_ID,
)

CODEOWNERS = ["@muxa"]

midi_in_ns = cg.esphome_ns.namespace("midi_in")

TemplateLightOutput = midi_in_ns.class_(
    "TemplateLightOutput", light.AddressableLight, light.AddressableLightState
)

CONFIG_SCHEMA = light.ADDRESSABLE_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(TemplateLightOutput),
        cv.Required(CONF_NUM_LEDS): cv.positive_not_null_int,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):

    cg.add_global(midi_in_ns.using)

    var = cg.new_Pvariable(config[CONF_OUTPUT_ID], config[CONF_NUM_LEDS])
    await cg.register_component(var, config)
    await light.register_light(var, config)
