const INPUT_PINS = 8;
const OUTPUT_PINS = 5;

function set_output_pins(pins) {
  for (let pin = 0; pin < OUTPUT_PINS; pin++) {
    console.log(INPUT_PINS + pin, (pins & (1 << pin)) != 0);
  }
}

// for (let i = 0; i < 2 ** 5 - 1; i++) {
// }

set_output_pins(31);
