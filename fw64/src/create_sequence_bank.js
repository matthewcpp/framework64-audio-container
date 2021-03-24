const fs = require("fs");
const path = require("path");

const fw64 = require("./fw64");
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

    const filteredInsFile = path.join(fw64.generalMidiDir, "gm.ins");
    const json2insCommandArgs = [fw64.generalMidiJsonFile, filteredInsFile];

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
        json2insCommandArgs.push(midi0File);
    }

    fs.closeSync(wineScript);

    await audioUtil.runScript(wineScriptPath);

    // creates the compiled sbk file
    console.log("wine", sbcCommandArgs.join(' '));
    await execFile("wine", sbcCommandArgs);

    // creates a filtered ins file containing only the needed sounds for the created sequence bank
    console.log(fw64.json2ins, json2insCommandArgs.join(' '));
    await execFile(fw64.json2ins, json2insCommandArgs);

    // create the ctl and tbl files for the filtered instrument, then copy to output dir
    const icCommand = [sgiTools.ic, "-Ogm", "gm.ins"];
    console.log("wine", icCommand);
    await execFile("wine", icCommand, {cwd: fw64.generalMidiDir});
    fs.copyFileSync(path.join(fw64.generalMidiDir, "gm.ctl"), path.join(destDir, "gm.ctl"));
    fs.copyFileSync(path.join(fw64.generalMidiDir, "gm.tbl"), path.join(destDir, "gm.tbl"));
}

if (require.main === module) {
    main();
}
