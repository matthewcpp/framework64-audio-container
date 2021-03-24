const fs = require("fs");
const path = require("path");

const sgiTools = require("./sgi_tools");
const audioUtil = require("./audio_util");

const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

const srcDir = "src";
const destDir = "dest";
const jobDir = "job";

async function main() {
    const srcFiles = fs.readdirSync(srcDir);

    const sbkFile = path.join(destDir, "midi.sbk")
    const sbcCommandArgs = [sgiTools.sbc, `-O${sbkFile}`];

    const wineScriptPath = path.join(jobDir, "winescript.sh");
    const wineScript = fs.openSync(wineScriptPath, "w");

    for (const file of srcFiles) {
        if (path.extname(file) !== ".mid")
            continue;

        const basename = path.basename(file, ".mid");
        const srcFile = path.join(srcDir, file);
        const midi0File = path.join(jobDir, file);
        const compressedMidiFile = path.join(jobDir, basename + ".cmp");

        const midicvtCommand = `wine ${sgiTools.midicvt} -o -s "${srcFile}" "${midi0File}"`;
        audioUtil.writeScriptCommand(wineScript, midicvtCommand);

        const midicompCommand = `wine ${sgiTools.midicomp} "${midi0File}" "${compressedMidiFile}"`;
        audioUtil.writeScriptCommand(wineScript, midicompCommand);

        sbcCommandArgs.push(compressedMidiFile);
    }

    fs.closeSync(wineScript);

    await audioUtil.runScript(wineScriptPath);

    console.log(`wine ${sbcCommandArgs.join(' ')}`);
    await execFile("wine", sbcCommandArgs);
}

if (require.main === module) {
    main();
}
