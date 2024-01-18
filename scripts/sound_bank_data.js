const path = require("path");

class SoundBankData {
    addSound(aifcFile) {
        const sound = {
            name: `se_sound${this.data.sounds.length}`,
            use: aifcFile,
            pan: 64,
            volume: 127,
            envelope: 0,
            keymap: 0
        };

        this.data.instruments[0].sounds.push(this.data.sounds.length);
        this.data.sounds.push(sound);
    }

    constructor() {
        this.data = {
            envelopes: [
                {
                    name: "se_env",
                    attackTime: 0,
                    attackVolume: 127,
                    decayTime: 4000000,
                    decayVolume: 0,
                    releaseTime: 200000
                }
            ],
            keymaps: [
                {
                    name: "se_keymap0",
                    detune: 0,
                    keyBase: 75,
                    keyMax: 63,
                    keyMin: 63,
                    velocityMax: 127,
                    velocityMin: 0
                }
            ],
            sounds: [

            ],
            instruments: [
                {
                    name: "se_instrument",
                    volume: 127,
                    pan: 64,
                    sounds: []
                }
            ],
            bank: {
                name: "se_bank",
                sampleRate: 44100,
                instruments: [
                    {
                        instrument: 0,
                        program: 0
                    }
                ]
            }
        };
    }
}

module.exports = SoundBankData;
